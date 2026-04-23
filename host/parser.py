# host/parser.py
# Parses EMBTEST: protocol lines into Python objects

from dataclasses import dataclass, field
from typing import List, Optional


@dataclass
class TestResult:
    name:        str
    status:      str            # PASS | FAIL | TIMEOUT | FAULT
    duration_us: int = 0
    file:        Optional[str] = None
    line:        Optional[int] = None
    message:     Optional[str] = None


@dataclass
class TestSession:
    suite:   str
    results: List[TestResult] = field(default_factory=list)

    @property
    def passed(self) -> int:
        return sum(1 for r in self.results if r.status == "PASS")

    @property
    def failed(self) -> int:
        return sum(1 for r in self.results if r.status != "PASS")

    @property
    def total(self) -> int:
        return len(self.results)


def parse(lines: List[str]) -> Optional[TestSession]:
    """
    Parse a list of EMBTEST: protocol lines.
    Returns a TestSession or None if no START line found.
    """
    session = None

    for raw in lines:
        line = raw.strip()

        # Only process lines that start with EMBTEST:
        if not line.startswith("EMBTEST:"):
            continue

        parts = line.split(":")

        # Need at least EMBTEST + kind
        if len(parts) < 2:
            continue

        kind = parts[1]

        # ── EMBTEST:START:suite_name:count ──────────────
        if kind == "START":
            suite = parts[2] if len(parts) > 2 else "unknown"
            session = TestSession(suite=suite)

        # ── EMBTEST:PASS:test_name:duration_us ──────────
        elif kind == "PASS" and session is not None:
            name        = parts[2] if len(parts) > 2 else "unknown"
            duration_us = int(parts[3]) if len(parts) > 3 else 0
            session.results.append(
                TestResult(name=name, status="PASS",
                           duration_us=duration_us)
            )

        # ── EMBTEST:FAIL:name:dur:file:line:msg ─────────
        elif kind == "FAIL" and session is not None:
            name        = parts[2] if len(parts) > 2 else "unknown"
            duration_us = int(parts[3]) if len(parts) > 3 else 0
            file        = parts[4] if len(parts) > 4 else ""
            lineno      = int(parts[5]) if len(parts) > 5 else 0
            # message may contain colons — rejoin remaining parts
            message     = ":".join(parts[6:]) if len(parts) > 6 else ""
            session.results.append(
                TestResult(name=name, status="FAIL",
                           duration_us=duration_us,
                           file=file, line=lineno,
                           message=message)
            )

        # ── EMBTEST:TIMEOUT:name:timeout_ms ─────────────
        elif kind == "TIMEOUT" and session is not None:
            name = parts[2] if len(parts) > 2 else "unknown"
            session.results.append(
                TestResult(name=name, status="TIMEOUT")
            )

        # ── EMBTEST:FAULT:name:fault_type ───────────────
        elif kind == "FAULT" and session is not None:
            name    = parts[2] if len(parts) > 2 else "unknown"
            message = parts[3] if len(parts) > 3 else "HardFault"
            session.results.append(
                TestResult(name=name, status="FAULT",
                           message=message)
            )

        # ── EMBTEST:END:passed:failed ────────────────────
        elif kind == "END":
            break   # session complete

    return session
