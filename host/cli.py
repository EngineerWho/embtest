# host/cli.py
# Entry point: flash → read → parse → report

import argparse
import subprocess
import sys
import os

from .serial_reader import SerialReader
from .parser        import parse
from .reporter      import write_junit_xml, print_summary


def flash(elf: str, cfg: str) -> None:
    """Flash firmware using OpenOCD."""
    print(f"[embtest] flashing {elf} ...")
    result = subprocess.run(
        [
            "openocd",
            "-f", cfg,
            "-c", f"program {elf} verify reset exit",
        ],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print("[embtest] flash FAILED:")
        print(result.stderr)
        sys.exit(1)
    print("[embtest] flash OK")


def main() -> None:
    p = argparse.ArgumentParser(
        prog="embtest",
        description="On-target embedded test runner",
    )
    p.add_argument(
        "--port",
        required=True,
        help="Serial port  e.g. /dev/ttyACM0",
    )
    p.add_argument(
        "--elf",
        required=True,
        help="Path to compiled firmware .elf",
    )
    p.add_argument(
        "--baud",
        default=9600,
        type=int,
        help="UART baud rate (default: 9600)",
    )
    p.add_argument(
        "--cfg",
        default="boards/stm32f4/flash.cfg",
        help="OpenOCD config file",
    )
    p.add_argument(
        "--output",
        default="junit-results.xml",
        help="JUnit XML output path",
    )
    p.add_argument(
        "--timeout",
        default=15.0,
        type=float,
        help="Seconds to wait for EMBTEST:END (default: 15)",
    )
    p.add_argument(
        "--no-flash",
        action="store_true",
        help="Skip flashing — just read serial output",
    )
    args = p.parse_args()

    # ── flash ──────────────────────────────────────────
    if not args.no_flash:
        flash(args.elf, args.cfg)
    else:
        print("[embtest] skipping flash (--no-flash)")

    # ── read ───────────────────────────────────────────
    print(f"[embtest] reading from {args.port} "
          f"at {args.baud} baud ...")
    print("[embtest] press RESET on board if needed\n")

    reader  = SerialReader(args.port,
                           args.baud,
                           args.timeout)
    lines   = reader.read_session()

    # ── parse ──────────────────────────────────────────
    session = parse(lines)

    if session is None:
        print("[embtest] ERROR: no valid session found "
              "in serial output")
        sys.exit(1)

    # ── report ─────────────────────────────────────────
    print_summary(session)
    write_junit_xml(session, args.output)

    # ── exit code ──────────────────────────────────────
    # non-zero exit makes CI fail when tests fail
    sys.exit(0 if session.failed == 0 else 1)


if __name__ == "__main__":
    main()
