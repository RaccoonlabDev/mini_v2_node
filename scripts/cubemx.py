#!/usr/bin/env python3
# Copyright (C) 2026 Dmitry Ponomarev <ponomarevda96@gmail.com>
# Distributed under the terms of the GPL v3 license, available in the file LICENSE.
"""Manage prebuilt STM32CubeMX HAL projects for the node-core framework.

CI cannot run STM32CubeMX, so generated HAL is produced once and distributed as
deterministic archives keyed by the .ioc sha256: ``<name>-<iochash>.tar.gz``.
The hash guarantees the HAL always matches its .ioc — a stale archive can't slip
through.

Subcommands:
  ensure   Make sure a generated project exists in --out: reuse it, restore a
           prebuilt archive (local cache or GitHub release), or generate it
           locally if STM32CubeMX is installed.
  package  Generate the project locally (needs STM32CubeMX) and write the
           <name>-<iochash>.tar.gz archive for distribution.

Config is a per-board manifest (cubemx.json) next to the board:

  {
    "name": "salt-pmu-gimbal",
    "ioc": "board/project.ioc",
    "store": { "repo": "<owner>/<repo>", "tag": "<release-tag>" }
  }

Paths in the manifest are resolved relative to the manifest file. The store is
optional and is supplied by the consuming app (node-core has no default store).
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys
import tarfile
import tempfile
import urllib.request
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_GENERATOR = SCRIPT_DIR / "generate_cubemx_hal.sh"
DEFAULT_CACHE_DIR = SCRIPT_DIR.parent / "release" / "cubemx"
STAMP_NAME = ".cubemx_ioc.sha256"

CUBEMX_CANDIDATES = (
    os.environ.get("STM32CUBEMX", ""),
    "/opt/stm32cubemx/STM32CubeMX",
    "/opt/st/STM32CubeMX/STM32CubeMX",
    str(Path.home() / "STM32CubeMX" / "STM32CubeMX"),
    "/Applications/STMicroelectronics/STM32CubeMX.app/Contents/MacOs/STM32CubeMX",
)


def fail(message: str) -> "NoReturn":  # type: ignore[name-defined]
    print(f"cubemx.py: {message}", file=sys.stderr)
    sys.exit(1)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(65536), b""):
            digest.update(chunk)
    return digest.hexdigest()


class Manifest:
    def __init__(self, path: Path) -> None:
        if not path.is_file():
            fail(f"manifest not found: {path}")
        data = json.loads(path.read_text())
        base = path.parent

        self.name = data.get("name")
        if not self.name:
            fail(f"manifest is missing 'name': {path}")

        ioc = data.get("ioc")
        if not ioc:
            fail(f"manifest is missing 'ioc': {path}")
        self.ioc = (base / ioc).resolve()
        if not self.ioc.is_file():
            fail(f"ioc file not found: {self.ioc}")

        store = data.get("store") or {}
        # Env overrides let CI point at a different store without editing files.
        self.repo = os.environ.get("CUBEMX_ARCHIVE_REPO") or store.get("repo")
        self.tag = os.environ.get("CUBEMX_CACHE_TAG") or store.get("tag")

    @property
    def ioc_hash(self) -> str:
        return sha256(self.ioc)

    def archive_name(self, ioc_hash: str) -> str:
        return f"{self.name}-{ioc_hash}.tar.gz"


def stamp_matches(out_dir: Path, ioc_hash: str) -> bool:
    stamp = out_dir / STAMP_NAME
    return stamp.is_file() and stamp.read_text().strip() == ioc_hash


def find_cubemx() -> str | None:
    for candidate in CUBEMX_CANDIDATES:
        if candidate and os.access(candidate, os.X_OK):
            return candidate
    return shutil.which("STM32CubeMX")


def extract(archive: Path, out_dir: Path) -> None:
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True)
    with tarfile.open(archive, "r:gz") as tar:
        try:
            tar.extractall(out_dir, filter="data")  # safe extraction (Python 3.12+)
        except TypeError:
            tar.extractall(out_dir)


def run_generator(generator: Path, ioc: Path, out_dir: Path) -> None:
    cubemx = find_cubemx()
    if not cubemx:
        fail("STM32CubeMX not found (set STM32CUBEMX or pass an installed path).")
    cmd = [str(generator), "-i", str(ioc), "-o", str(out_dir), "-m", cubemx, "--force"]
    print(f"Generating CubeMX project: {' '.join(cmd)}")
    subprocess.run(cmd, check=True)


def restore_from_local_cache(manifest: Manifest, ioc_hash: str, out_dir: Path,
                             cache_dir: Path) -> bool:
    archive = cache_dir / manifest.archive_name(ioc_hash)
    if not archive.is_file():
        return False
    print(f"Restoring from local cache: {archive}")
    extract(archive, out_dir)
    if not stamp_matches(out_dir, ioc_hash):
        fail(f"cached archive stamp does not match ioc hash: {archive}")
    return True


def download_from_store(manifest: Manifest, ioc_hash: str, out_dir: Path) -> bool:
    if not (manifest.repo and manifest.tag):
        return False
    archive_name = manifest.archive_name(ioc_hash)
    with tempfile.TemporaryDirectory() as tmp:
        dest = Path(tmp) / archive_name
        if not (_download_via_gh(manifest, archive_name, dest)
                or _download_via_url(manifest, archive_name, dest)):
            return False
        extract(dest, out_dir)
    if not stamp_matches(out_dir, ioc_hash):
        fail(f"downloaded archive stamp does not match ioc hash: {archive_name}")
    return True


def _download_via_gh(manifest: Manifest, archive_name: str, dest: Path) -> bool:
    if not shutil.which("gh"):
        return False
    print(f"Downloading {archive_name} via gh from {manifest.repo}@{manifest.tag}")
    # `gh release download` is unreliable for a single private asset, so resolve
    # the asset id via the API and stream it as octet-stream (works with GH_TOKEN).
    asset_id = subprocess.run(
        ["gh", "api", f"repos/{manifest.repo}/releases/tags/{manifest.tag}",
         "--jq", f'.assets[] | select(.name=="{archive_name}") | .id'],
        check=False, capture_output=True, text=True,
    ).stdout.strip()
    if not asset_id:
        return False
    with dest.open("wb") as handle:
        result = subprocess.run(
            ["gh", "api", f"repos/{manifest.repo}/releases/assets/{asset_id}",
             "-H", "Accept: application/octet-stream"],
            check=False, stdout=handle,
        )
    return result.returncode == 0 and dest.is_file() and dest.stat().st_size > 0


def _download_via_url(manifest: Manifest, archive_name: str, dest: Path) -> bool:
    # Works only for public releases; private ones need gh (with GH_TOKEN).
    url = f"https://github.com/{manifest.repo}/releases/download/{manifest.tag}/{archive_name}"
    print(f"Downloading {url}")
    try:
        with urllib.request.urlopen(url) as response, dest.open("wb") as handle:
            shutil.copyfileobj(response, handle)
    except OSError as error:
        print(f"  download failed: {error}", file=sys.stderr)
        return False
    return True


def cmd_ensure(args: argparse.Namespace) -> None:
    manifest = Manifest(Path(args.manifest).resolve())
    out_dir = Path(args.out).resolve()
    ioc_hash = manifest.ioc_hash
    cache_dir = Path(args.cache_dir).resolve()

    if stamp_matches(out_dir, ioc_hash):
        print(f"CubeMX project is up to date: {out_dir}")
        return
    if restore_from_local_cache(manifest, ioc_hash, out_dir, cache_dir):
        print(f"Restored from local cache: {out_dir}")
        return
    if find_cubemx():
        generate(Path(args.generator).resolve(), manifest.ioc, out_dir, ioc_hash)
        print(f"Generated locally: {out_dir}")
        return
    if download_from_store(manifest, ioc_hash, out_dir):
        print(f"Restored from release: {out_dir}")
        return
    fail(
        f"no CubeMX project for '{manifest.name}' ({ioc_hash}).\n"
        f"  Generate and publish it with: cubemx.py package --manifest {args.manifest} ...\n"
        f"  or install STM32CubeMX to generate locally."
    )


def find_project_root(out_dir: Path) -> Path:
    """The generated CubeMX project root — the dir holding Core/Src/main.c.

    CubeMX may nest the project under a ProjectName subdir, so locate it instead
    of assuming it sits directly at out_dir.
    """
    for main_c in sorted(out_dir.rglob("main.c")):
        if main_c.parent.name == "Src" and main_c.parent.parent.name == "Core":
            return main_c.parent.parent.parent
    fail(f"generated project has no Core/Src/main.c under {out_dir}")


def normalize_to_flat(out_dir: Path) -> None:
    """Make out_dir itself the project root (Core/, Drivers/, ... directly inside)
    with the stamp at its root. CubeMX may nest the project under a ProjectName
    subdir; flatten it so the build and the archive share the stock cube-project
    layout (STM32_CUBEMX_PROJECT_PATH can point straight at out_dir).
    """
    root = find_project_root(out_dir)
    if root == out_dir:
        return
    stamp = out_dir / STAMP_NAME
    if stamp.is_file():
        shutil.copy2(stamp, root / STAMP_NAME)
    tmp = out_dir.with_name(out_dir.name + ".flat.tmp")
    if tmp.exists():
        shutil.rmtree(tmp)
    shutil.move(str(root), str(tmp))
    shutil.rmtree(out_dir)
    shutil.move(str(tmp), str(out_dir))


def generate(generator: Path, ioc: Path, out_dir: Path, ioc_hash: str) -> None:
    """Generate the CubeMX project into out_dir and flatten it to the stock layout."""
    run_generator(generator, ioc, out_dir)
    normalize_to_flat(out_dir)
    if not stamp_matches(out_dir, ioc_hash):
        fail(f"generated project stamp does not match ioc hash: {out_dir}")


def cmd_package(args: argparse.Namespace) -> None:
    manifest = Manifest(Path(args.manifest).resolve())
    out_dir = Path(args.out).resolve()
    archive_dir = Path(args.archive_dir).resolve()
    ioc_hash = manifest.ioc_hash

    generate(Path(args.generator).resolve(), manifest.ioc, out_dir, ioc_hash)

    archive_dir.mkdir(parents=True, exist_ok=True)
    archive = archive_dir / manifest.archive_name(ioc_hash)
    with tarfile.open(archive, "w:gz") as tar:
        tar.add(out_dir, arcname=".")  # out_dir is now the flat project root
    print(f"Packaged archive: {archive}")

    if args.upload:
        _upload(manifest, archive)
    else:
        print("Upload it to the release with:")
        print(f"  gh release upload {manifest.tag} {archive} --repo {manifest.repo} --clobber")


def _upload(manifest: Manifest, archive: Path) -> None:
    if not (manifest.repo and manifest.tag):
        fail("manifest has no store.repo/store.tag to upload to.")
    if not shutil.which("gh"):
        fail("gh CLI is required for --upload.")
    # Create the release if it does not exist yet, then attach the archive.
    subprocess.run(
        ["gh", "release", "create", manifest.tag, "--repo", manifest.repo,
         "--title", manifest.tag, "--notes", "Prebuilt STM32CubeMX HAL archives."],
        check=False,
    )
    subprocess.run(
        ["gh", "release", "upload", manifest.tag, str(archive),
         "--repo", manifest.repo, "--clobber"],
        check=True,
    )
    print(f"Uploaded {archive.name} to {manifest.repo}@{manifest.tag}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)

    def add_common(p: argparse.ArgumentParser) -> None:
        p.add_argument("--manifest", required=True, help="path to cubemx.json")
        p.add_argument("--out", required=True, help="generated project output dir")
        p.add_argument("--generator", default=str(DEFAULT_GENERATOR),
                       help="CubeMX generator script (default: generate_cubemx_hal.sh)")
        p.add_argument("--cache-dir", default=str(DEFAULT_CACHE_DIR),
                       help="local archive cache dir")

    p_ensure = sub.add_parser("ensure", help="restore or generate the project")
    add_common(p_ensure)
    p_ensure.set_defaults(func=cmd_ensure)

    p_package = sub.add_parser("package", help="generate and archive the project")
    add_common(p_package)
    p_package.add_argument("--archive-dir", default=str(DEFAULT_CACHE_DIR),
                           help="where to write the archive (default: cache dir)")
    p_package.add_argument("--upload", action="store_true",
                           help="upload the archive to the release via gh")
    p_package.set_defaults(func=cmd_package)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
