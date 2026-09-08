#!/usr/bin/env python3
# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
"""Collect firmware size per board target and render the CI pull request comment.

`--collect` must run after the build, `--render` afterwards;
.github/workflows/ci.yml drives both modes.

Flash comes from node.bin, the image that is actually flashed. RAM comes from
`arm-none-eabi-size` on Src/platform/<mcu>/node — the linker's own executable.
Not obj/node.elf, which the postprocess step (kocherga_image.py --side-patch)
rewrites into a single flat .data section, and not the map's .data/.bss output
sections, which miss ._user_heap_stack.

Every metric degrades to "n/a" on any error, so a failed build still comments.
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT_DIR = Path(__file__).resolve().parent.parent

# Padding and the kocherga descriptor move an image by a few bytes between
# otherwise identical builds, so a smaller move is not worth a reviewer's time.
SIGNIFICANT_SHARE = 0.01

# "RAM              0x20000000         0x00005000         xrw"
MEMORY_REGION_RE = re.compile(
    r"^(\S+)\s+0x([0-9a-fA-F]+)\s+0x([0-9a-fA-F]+)\s+(\S+)\s*$")
SIZE_TOOL = "arm-none-eabi-size"

# An alias builds <alias>_application, unless CI asked for a standalone image.
IMAGE_KINDS = ("application", "standalone")


def human_bytes(num):
    if num is None:
        return "n/a"
    value = float(num)
    for unit in ("B", "KiB", "MiB"):
        if abs(value) < 1024.0 or unit == "MiB":
            return f"{value:.0f} {unit}" if unit == "B" else f"{value:.1f} {unit}"
        value /= 1024.0
    return f"{value:.1f} MiB"


def run(cmd):
    """stdout of cmd, or None on any failure."""
    try:
        return subprocess.run(cmd, capture_output=True, text=True, timeout=60,
                              check=True).stdout
    except (subprocess.SubprocessError, OSError):
        return None


def load_json(path):
    """Parsed JSON at path, or None if it is missing or unreadable."""
    if not path:
        return None
    try:
        with open(path, encoding="utf-8") as handle:
            return json.load(handle)
    except (OSError, ValueError):
        return None


def release_targets(root=ROOT_DIR):
    """The board aliases the Makefile's `all` target builds.

    Reading the Makefile keeps the list from drifting away from what CI builds.
    The SITL aliases it also lists are host binaries: they carry no flash image
    and nobody reviews their size.
    """
    try:
        makefile = (root / "Makefile").read_text(encoding="utf-8")
    except OSError:
        return []
    match = re.search(r"^all:\s*(.*)$", makefile, re.MULTILINE)
    if not match:
        return []
    return [t for t in match.group(1).split()
            if t.startswith("rl_") and "sitl" not in t]


def parse_memory_regions(text):
    """{name: capacity_bytes} from the map's "Memory Configuration" table."""
    regions = {}
    lines = text.splitlines()
    try:
        start = next(i for i, line in enumerate(lines)
                     if line.startswith("Memory Configuration"))
    except StopIteration:
        return regions
    for line in lines[start + 1:]:
        if line.startswith("Linker script and memory map"):
            break
        match = MEMORY_REGION_RE.match(line)
        if match and match.group(1) != "*default*":
            regions[match.group(1)] = int(match.group(3), 16)
    return regions


def parse_capacities(path):
    """(flash_capacity, ram_capacity) from a linker map; None where unknown."""
    try:
        text = Path(path).read_text(encoding="utf-8", errors="replace")
    except OSError:
        return None, None

    regions = parse_memory_regions(text)
    # The RAM region is named RAM on stm32f103 but RAM_D1 on stm32h753xx.
    ram_capacity = next(
        (size for name, size in regions.items() if name != "FLASH"), None)
    return regions.get("FLASH"), ram_capacity


def elf_ram_bytes(elf):
    """.data + .bss of an ELF, as arm-none-eabi-size counts them, or None.

    `size` folds ._user_heap_stack into bss, which the map's own .data/.bss
    output sections leave out.
    """
    out = run([SIZE_TOOL, str(elf)])
    lines = out.strip().splitlines() if out else []
    if len(lines) < 2:
        return None
    fields = lines[1].split()
    try:
        return int(fields[1]) + int(fields[2])
    except (IndexError, ValueError):
        return None


def obj_dir(target, build_dir):
    """The obj dir of whichever image kind was built for this alias, or None."""
    for kind in IMAGE_KINDS:
        candidate = build_dir / f"{target}_{kind}" / "obj"
        if candidate.is_dir():
            return candidate
    return None


def measure(target, build_dir):
    """Size metrics for one board target; missing artifacts give None values."""
    obj = obj_dir(target, build_dir)
    if obj is None:
        return {"flash": None, "flash_capacity": None,
                "ram": None, "ram_capacity": None}

    flash = None
    try:
        flash = (obj / "node.bin").stat().st_size
    except OSError:
        pass

    # The platform dir is named after the MCU (stm32f103, stm32h753xx, ...) and
    # holds both the map and the executable the linker actually produced.
    maps = sorted(obj.glob("Src/platform/*/.map"))
    flash_capacity, ram_capacity = parse_capacities(maps[0]) if maps else (None, None)
    ram = elf_ram_bytes(maps[0].parent / "node") if maps else None

    return {
        "flash": flash,
        "flash_capacity": flash_capacity,
        "ram": ram,
        "ram_capacity": ram_capacity,
    }


def collect(build_dir, label):
    targets = release_targets()
    return {
        "label": label,
        "targets": {target: measure(target, build_dir) for target in targets},
    }


def pick_baseline(baseline_path):
    """(targets, label) — the cached baseline, or nothing until one exists.

    There is deliberately no hardcoded seed: a made up baseline would report
    deltas against numbers nobody measured. Until the first push to the default
    branch fills the cache, the report states that it has nothing to compare to.
    """
    data = load_json(baseline_path)
    targets = (data or {}).get("targets")
    if targets:
        return targets, data.get("label") or "baseline"
    return {}, ""


def percent(used, capacity):
    if not used or not capacity:
        return ""
    return f" ({100.0 * used / capacity:.1f}%)"


def significant(diff, base):
    return bool(base) and abs(diff) > SIGNIFICANT_SHARE * base


def delta_cell(current, base):
    """"🔴 +128 B" / "🟢 -64 B" / "➖ 0" comparing current against base."""
    if current is None or base is None:
        return "n/a"
    diff = current - base
    if diff == 0:
        return "➖ 0"
    emoji = ("🔴" if diff > 0 else "🟢") if significant(diff, base) else "➖"
    return f"{emoji} {'+' if diff > 0 else '-'}{human_bytes(abs(diff))}"


def movements(metrics, base):
    """(significant [(label, diff)], whether anything moved at all).

    The summary names only the significant moves; the rest still reach the table.
    """
    moved, any_move = [], False
    for key, label in (("flash", "flash"), ("ram", "RAM")):
        cur, prev = metrics.get(key), base.get(key)
        if cur is None or prev is None or cur == prev:
            continue
        any_move = True
        if significant(cur - prev, prev):
            moved.append((label, cur - prev))
    return moved, any_move


def summary(targets, baseline_targets, baseline_label):
    """The one line a reviewer sees before expanding: did anything grow, and by how much."""
    if not targets:
        return "⚠️ Firmware size: the build produced no artifacts"
    if not baseline_label:
        return "Firmware size measured (no baseline to compare against)"

    moved, grew, comparable, drifted = [], False, False, False
    for name, metrics in targets.items():
        base = baseline_targets.get(name) or {}
        comparable = comparable or bool(base)
        significant_moves, any_move = movements(metrics or {}, base)
        drifted = drifted or any_move
        diffs = []
        for label, diff in significant_moves:
            grew = grew or diff > 0
            diffs.append(f"{'+' if diff > 0 else '-'}{human_bytes(abs(diff))} {label}")
        if diffs:
            moved.append(f"{name} {', '.join(diffs)}")

    if not comparable:
        return "Firmware size measured (baseline covers no current target)"
    if not moved:
        return f"➖ Firmware size within {SIGNIFICANT_SHARE:.0%}" if drifted \
            else "➖ Firmware size unchanged"
    return f"{'🔴 Firmware size grew' if grew else '🟢 Firmware size shrank'}: {'; '.join(moved)}"


def render(current, baseline_targets, baseline_label):
    targets = current.get("targets") or {}
    # Collapsed, so the comment costs a reviewer one line unless they want the
    # breakdown. GitHub only renders the body as markdown after a blank line.
    lines = ["<details>",
             f"<summary>{summary(targets, baseline_targets, baseline_label)}</summary>",
             ""]

    if not targets:
        lines += ["The build produced no binaries to measure.", "", "</details>", ""]
        return "\n".join(lines)

    if baseline_label:
        lines += [f"Compared against `{baseline_label}`.", ""]
    else:
        lines += ["No baseline available yet, so no deltas are shown.", ""]

    lines += ["| Target | Flash | Δ flash | RAM | Δ RAM |",
              "| --- | ---: | ---: | ---: | ---: |"]
    # Makefile order, because it groups the boards the way a reviewer expects.
    for name in targets:
        metrics = targets[name] or {}
        base = baseline_targets.get(name) or {}
        flash = f"{human_bytes(metrics.get('flash'))}" \
                f"{percent(metrics.get('flash'), metrics.get('flash_capacity'))}"
        ram = f"{human_bytes(metrics.get('ram'))}" \
              f"{percent(metrics.get('ram'), metrics.get('ram_capacity'))}"
        lines.append(
            f"| `{name}` | {flash} | {delta_cell(metrics.get('flash'), base.get('flash'))} "
            f"| {ram} | {delta_cell(metrics.get('ram'), base.get('ram'))} |")

    lines += ["",
              "<sub>Flash is the size of the flashed `node.bin`; RAM is `data` + "
              "`bss` from `arm-none-eabi-size`, so it counts the reserved heap and "
              f"stack as well as statics. Moves under {SIGNIFICANT_SHARE:.0%} are "
              "not flagged.</sub>",
              "", "</details>", ""]
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--collect", action="store_true",
                        help="measure the board targets instead of rendering")
    parser.add_argument("--build-dir", default=str(ROOT_DIR / "build"),
                        help="build directory holding <target>_<image kind>/obj")
    parser.add_argument("--label", default="",
                        help="how the report should name these metrics, e.g. main@abc1234")
    parser.add_argument("--metrics", default="",
                        help="metrics JSON written by --collect")
    parser.add_argument("--baseline", default="",
                        help="baseline metrics JSON restored from the cache")
    parser.add_argument("--out", required=True, help="file to write")
    args = parser.parse_args()

    out = Path(args.out)
    if out.parent != Path(""):
        out.parent.mkdir(parents=True, exist_ok=True)

    if args.collect:
        payload = json.dumps(collect(Path(args.build_dir), args.label), indent=2)
        # The rendered table rounds to KiB, so the exact counts have to reach the
        # run log to stay usable for anything else.
        print(payload)
        out.write_text(payload + "\n", encoding="utf-8")
        return 0

    current = load_json(args.metrics) or {"targets": {}}
    baseline_targets, baseline_label = pick_baseline(args.baseline)
    out.write_text(render(current, baseline_targets, baseline_label), encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
