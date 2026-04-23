# host/reporter.py
# Generates JUnit XML and console output from a TestSession

import xml.etree.ElementTree as ET
from .parser import TestSession


def write_junit_xml(session: TestSession, path: str) -> None:
    """
    Write JUnit XML report — readable by GitHub Actions,
    Jenkins, GitLab CI, and any standard CI system.
    """
    suite = ET.Element(
        "testsuite",
        name=session.suite,
        tests=str(session.total),
        failures=str(session.failed),
        errors="0",
    )

    for r in session.results:
        tc = ET.SubElement(
            suite,
            "testcase",
            name=r.name,
            classname=session.suite,
            time=str(r.duration_us / 1_000_000),
        )

        if r.status == "FAIL":
            fail = ET.SubElement(
                tc, "failure",
                message=r.message or "assertion failed",
                type="AssertionError",
            )
            fail.text = f"{r.file}:{r.line}: {r.message}"

        elif r.status == "TIMEOUT":
            ET.SubElement(
                tc, "error",
                message="test timed out",
                type="Timeout",
            )

        elif r.status == "FAULT":
            ET.SubElement(
                tc, "error",
                message=r.message or "HardFault",
                type="HardFault",
            )

    tree = ET.ElementTree(suite)
    ET.indent(tree, space="  ")
    tree.write(path, xml_declaration=True,
               encoding="utf-8")
    print(f"[embtest] report written: {path}")


def print_summary(session: TestSession) -> None:
    """
    Print coloured console summary.
    """
    print()
    for r in session.results:
        if r.status == "PASS":
            print(f"  PASS  {r.name:<40} ({r.duration_us} us)")
        elif r.status == "FAIL":
            print(f"  FAIL  {r.name}")
            print(f"        {r.file}:{r.line}")
            print(f"        {r.message}")
        elif r.status == "TIMEOUT":
            print(f"  TIMEOUT  {r.name}")
        elif r.status == "FAULT":
            print(f"  FAULT  {r.name}  ({r.message})")

    print()
    print(f"=== done: {session.passed} passed  "
          f"{session.failed} failed ===")
