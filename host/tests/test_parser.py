# host/tests/test_parser.py
# Tests for the parser — runs on host, no hardware needed

import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../.."))

from host.parser import parse

# import with aliases so pytest does not confuse them
# with test classes
from host.parser import TestSession as Session
from host.parser import TestResult  as Result

SAMPLE_OUTPUT = [
    "EMBTEST BOOT",
    "EMBTEST:START:suite:3",
    "EMBTEST:PASS:always_passes:45",
    "EMBTEST:PASS:basic_math:45",
    "EMBTEST:FAIL:always_fails:0:test_blink.c:15:1 != 2",
    "EMBTEST:END:2:1",
]


def test_parse_returns_session():
    session = parse(SAMPLE_OUTPUT)
    assert session is not None


def test_parse_suite_name():
    session = parse(SAMPLE_OUTPUT)
    assert session.suite == "suite"


def test_parse_total_count():
    session = parse(SAMPLE_OUTPUT)
    assert session.total == 3


def test_parse_passed_count():
    session = parse(SAMPLE_OUTPUT)
    assert session.passed == 2


def test_parse_failed_count():
    session = parse(SAMPLE_OUTPUT)
    assert session.failed == 1


def test_parse_pass_result():
    session = parse(SAMPLE_OUTPUT)
    r = session.results[0]
    assert r.name == "always_passes"
    assert r.status == "PASS"
    assert r.duration_us == 45


def test_parse_fail_result():
    session = parse(SAMPLE_OUTPUT)
    r = session.results[2]
    assert r.name == "always_fails"
    assert r.status == "FAIL"
    assert r.file == "test_blink.c"
    assert r.line == 15
    assert r.message == "1 != 2"


def test_ignores_non_protocol_lines():
    lines = ["garbage", "EMBTEST:START:s:1",
             "more garbage", "EMBTEST:PASS:t:10",
             "EMBTEST:END:1:0"]
    session = parse(lines)
    assert session.total == 1
    assert session.passed == 1
