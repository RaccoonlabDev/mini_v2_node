#!/usr/bin/env python3
"""Direct LAWICEL/CANUSB test for PIHER PST360G2 CANopen."""

import argparse
import time

import serial


BITRATE_COMMANDS = {
    10000: "S0",
    20000: "S1",
    50000: "S2",
    100000: "S3",
    125000: "S4",
    250000: "S5",
    500000: "S6",
    800000: "S7",
    1000000: "S8",
}


def parse_int(value):
    return int(value, 0)


def parse_hex_payload(value):
    value = value.replace(":", "").replace(" ", "")
    if len(value) % 2:
        raise argparse.ArgumentTypeError("payload must have an even number of hex digits")
    payload = bytes.fromhex(value)
    if len(payload) > 8:
        raise argparse.ArgumentTypeError("classic CAN payload cannot exceed 8 bytes")
    return payload


def parse_channel(value):
    if "@" not in value:
        return value, 1_000_000

    port, baudrate = value.rsplit("@", 1)
    return port, int(baudrate)


def read_response(ser, timeout):
    deadline = time.monotonic() + timeout
    response = bytearray()

    while time.monotonic() < deadline:
        if ser.in_waiting:
            byte = ser.read(1)
            if not byte:
                continue
            response += byte
            if byte == b"\r":
                break
        else:
            time.sleep(0.001)

    return bytes(response)


def send_command(ser, command, timeout, expect_response=True):
    ser.write(command.encode("ascii") + b"\r")
    ser.flush()

    if not expect_response:
        print(f"{command:<14} -> <not read>")
        return b""

    response = read_response(ser, timeout)
    printable = response.replace(b"\r", b"\\r")
    print(f"{command:<14} -> {printable!r}")
    return response


def standard_frame_command(can_id, payload):
    return f"t{can_id:03X}{len(payload):X}{payload.hex().upper()}"


def extended_frame_command(can_id, payload):
    return f"T{can_id:08X}{len(payload):X}{payload.hex().upper()}"


def parse_canusb_frame(response):
    try:
        frame = response.decode("ascii").strip()
    except UnicodeDecodeError:
        return None

    if len(frame) < 5:
        return None

    if frame[0] == "t":
        can_id_digits = 3
        is_extended = False
    elif frame[0] == "T":
        can_id_digits = 8
        is_extended = True
    else:
        return None

    dlc_pos = 1 + can_id_digits
    if len(frame) < dlc_pos + 1:
        return None

    try:
        can_id = int(frame[1:dlc_pos], 16)
        dlc = int(frame[dlc_pos], 16)
        data = bytes.fromhex(frame[dlc_pos + 1 : dlc_pos + 1 + dlc * 2])
    except ValueError:
        return None

    if len(data) != dlc:
        return None

    return {
        "raw": frame,
        "can_id": can_id,
        "dlc": dlc,
        "data": data,
        "is_extended": is_extended,
    }


def describe_piher_pdo(frame):
    if frame["is_extended"] or frame["can_id"] != 0x1FF or frame["dlc"] != 8:
        return ""

    data = frame["data"]
    angle_raw = (data[0] << 8) | data[1]
    angle_deg = angle_raw * 360.0 / 4095.0
    return f" angle_raw={angle_raw} angle_deg={angle_deg:.2f}"


def init_canusb(ser, bitrate, timeout):
    for _ in range(3):
        send_command(ser, "", timeout)

    version = send_command(ser, "V", timeout)
    if not version.startswith(b"V"):
        raise RuntimeError("CANUSB did not return a version response")

    send_command(ser, "C", timeout)

    bitrate_command = BITRATE_COMMANDS.get(bitrate)
    if bitrate_command is None:
        supported = ", ".join(str(rate) for rate in sorted(BITRATE_COMMANDS))
        raise RuntimeError(f"unsupported bitrate {bitrate}; choose one of {supported}")

    if send_command(ser, bitrate_command, timeout) != b"\r":
        raise RuntimeError(f"CANUSB rejected bitrate command {bitrate_command}")

    if send_command(ser, "O", timeout) != b"\r":
        raise RuntimeError("CANUSB rejected open command")

    send_command(ser, "F", timeout)


def listen(ser, duration, timeout, expected_can_id=None):
    deadline = time.monotonic() + duration
    frames = []

    while time.monotonic() < deadline:
        response = read_response(ser, min(timeout, max(0.0, deadline - time.monotonic())))
        if response:
            printable = response.replace(b"\r", b"\\r")
            frame = parse_canusb_frame(response)
            if frame:
                frames.append(frame)
                suffix = describe_piher_pdo(frame)
                print(
                    f"RX             -> {printable!r} "
                    f"id=0x{frame['can_id']:X} dlc={frame['dlc']} data={frame['data'].hex(' ').upper()}{suffix}"
                )
            else:
                print(f"RX             -> {printable!r}")

    if expected_can_id is None:
        return frames

    return [frame for frame in frames if frame["can_id"] == expected_can_id]


def startup_piher(ser, args):
    startup_payload = bytes([0x01, args.node_id])
    startup_command = extended_frame_command(0x00000000, startup_payload)

    print("Checking whether PIHER is already operational...")
    pdo_frames = listen(ser, args.verify_timeout, args.timeout, expected_can_id=0x1FF)
    if len(pdo_frames) >= args.min_pdo:
        print(f"PIHER already operational: received {len(pdo_frames)} TPDO frames")
        return True

    print(
        f"Starting PIHER with extended-wrapper NMT hack: {startup_command} "
        f"(logical NMT start node 0x{args.node_id:02X})"
    )
    total_pdo_frames = len(pdo_frames)

    for attempt in range(1, args.startup_retries + 1):
        response = send_command(ser, startup_command, args.timeout)
        if response != b"Z\r":
            printable = response.replace(b"\r", b"\\r") if response else response
            print(f"Startup command not accepted on attempt {attempt}: {printable!r}")
        else:
            print(f"Startup command accepted on attempt {attempt}")

        pdo_frames = listen(ser, args.verify_timeout, args.timeout, expected_can_id=0x1FF)
        total_pdo_frames += len(pdo_frames)
        if len(pdo_frames) >= args.min_pdo:
            print(f"PIHER startup verified: received {len(pdo_frames)} TPDO frames after attempt {attempt}")
            return True

        time.sleep(args.gap)

    print(f"PIHER startup not verified: received {total_pdo_frames} total TPDO frames")
    return False


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--channel",
        default="/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_066AFF505372485067051653-if02@1000000",
    )
    parser.add_argument("--bitrate", type=int, default=250000)
    parser.add_argument("--timeout", type=float, default=0.5)
    parser.add_argument("--listen", type=float, default=0.3)
    parser.add_argument("--count", type=int, default=20)
    parser.add_argument("--gap", type=float, default=0.2)
    parser.add_argument("--node-id", type=parse_int, default=0x7F)
    parser.add_argument("--can-id", type=parse_int, default=0x000)
    parser.add_argument("--payload", type=parse_hex_payload, default=bytes([0x01, 0x7F]))
    parser.add_argument("--extended", action="store_true")
    parser.add_argument(
        "--raw-send",
        action="store_true",
        help="send --can-id/--payload repeatedly instead of PIHER startup/verify",
    )
    parser.add_argument("--startup-retries", type=int, default=5)
    parser.add_argument("--verify-timeout", type=float, default=0.7)
    parser.add_argument("--min-pdo", type=int, default=3)
    args = parser.parse_args()

    port, serial_baudrate = parse_channel(args.channel)
    command_factory = extended_frame_command if args.extended else standard_frame_command

    print(f"Connecting: {port} @ {serial_baudrate}")
    print(f"CAN bitrate: {args.bitrate}")

    with serial.Serial(port, baudrate=serial_baudrate, timeout=args.timeout) as ser:
        ser.reset_input_buffer()
        init_canusb(ser, args.bitrate, args.timeout)

        if not args.raw_send:
            ok = startup_piher(ser, args)
            send_command(ser, "F", args.timeout)
            send_command(ser, "C", args.timeout)
            raise SystemExit(0 if ok else 1)

        for attempt in range(1, args.count + 1):
            payload = args.payload
            if args.can_id == 0x000 and payload == bytes([0x01, 0x7F]):
                payload = bytes([0x01, args.node_id])

            command = command_factory(args.can_id, payload)
            response = send_command(ser, command, args.timeout)
            if response not in (b"z\r", b"Z\r"):
                print(f"TX command was not accepted on attempt {attempt}")

            listen(ser, args.listen, args.timeout)
            time.sleep(args.gap)

        send_command(ser, "F", args.timeout)
        send_command(ser, "C", args.timeout)


if __name__ == "__main__":
    main()
