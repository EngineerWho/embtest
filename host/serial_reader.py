# host/serial_reader.py
# Opens serial port, reads EMBTEST: lines from target

import serial
import time
from typing import List


class SerialReader:
    def __init__(self,
                 port: str,
                 baud: int = 9600,
                 timeout: float = 15.0):
        self.port    = port
        self.baud    = baud
        self.timeout = timeout

    def read_session(self) -> List[str]:
        """
        Opens the serial port and reads lines until
        EMBTEST:END is received or timeout expires.
        Returns all lines including non-protocol ones.
        """
        lines    = []
        started  = False
        deadline = time.time() + self.timeout

        with serial.Serial(self.port,
                           self.baud,
                           timeout=1.0) as ser:

            # flush anything already in the buffer
            ser.reset_input_buffer()

            while time.time() < deadline:
                raw = ser.readline()
                if not raw:
                    continue

                line = raw.decode("ascii",
                                  errors="replace").strip()

                if not line:
                    continue

                lines.append(line)
                print(f"  [{line}]")  # live feedback

                # wait for START before watching for END
                if line.startswith("EMBTEST:START"):
                    started = True

                if started and line.startswith("EMBTEST:END"):
                    return lines

        raise TimeoutError(
            f"No EMBTEST:END within {self.timeout}s "
            f"on {self.port} at {self.baud} baud"
        )
