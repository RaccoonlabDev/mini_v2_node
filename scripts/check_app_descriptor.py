#!/usr/bin/env python3
"""Check whether a Kocherga app descriptor is present and valid in a firmware image."""

from __future__ import annotations

import argparse
import pathlib
import sys

from kocherga_image import ImageModel


DEFAULT_IMAGE = (
    "/home/ilia/Desktop/vsCode/Embedded-work/mini_v2_node/build/"
    "rl_node_v4_dronecan_application/obj/Src/platform/stm32h753xx/"
    "node-2.0.b53a8b1699465776.316b6ea83c552764.app.release.dirty.bin"
)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("image", nargs="?", default=DEFAULT_IMAGE, help="Firmware .bin image to inspect")
    args = parser.parse_args()

    image_path = pathlib.Path(args.image)
    if not image_path.is_file():
        print(f"ERROR: image not found: {image_path}", file=sys.stderr)
        return 2

    image = image_path.read_bytes()
    model = ImageModel.construct_from_image(image)
    if model is None:
        print(f"ERROR: app descriptor not found: {image_path}", file=sys.stderr)
        return 1

    descriptor = model.app_descriptor
    crc_ok = model.validate_app_descriptor()
    size_ok = descriptor.image_size == len(image)

    print(f"image: {image_path}")
    print(f"descriptor_offset: {model.app_descriptor_offset}")
    print(f"byte_order: {model.byte_order}")
    print(f"version: {descriptor.version[0]}.{descriptor.version[1]}")
    print(f"vcs_revision_id: {descriptor.vcs_revision_id:016x}")
    print(f"image_size: {descriptor.image_size} ({'ok' if size_ok else f'expected {len(image)}'})")
    print(f"image_crc: {descriptor.image_crc:016x} ({'ok' if crc_ok else 'invalid'})")
    print(f"flags: release={descriptor.flags.release} dirty={descriptor.flags.dirty}")
    print(f"mcu_family_id: {descriptor.mcu_family_id}")

    if not size_ok or not crc_ok:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
