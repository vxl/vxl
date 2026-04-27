#!/usr/bin/env python3
"""Extract and print build warnings/errors from CTest Build.xml.

ctest_build() writes compiler diagnostics to Build.xml for CDash but does
not print them to stdout. This script reads Build.xml from the most recent
test run and prints the <Text> content of every <Warning> and <Error>
element so diagnostics appear directly in CI logs without inflating the
log with full per-target compile output.

Usage:
    python report_build_diagnostics.py <build-directory>

Adapted from ITK's Testing/ContinuousIntegration/report_build_diagnostics.py.
Uses defusedxml to harden against XXE / billion-laughs attacks even though
the input is CTest-generated and trusted; the workflow installs defusedxml
before running this script.
"""

import os
import sys

# Use defusedxml for XXE-safe parsing. The GitHub Actions workflow installs
# this package before invoking the script.
from defusedxml import ElementTree as ET


def main() -> int:
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <build-directory>", file=sys.stderr)
        return 1

    build_dir = sys.argv[1]

    tag_file = os.path.join(build_dir, "Testing", "TAG")
    if not os.path.isfile(tag_file):
        print(f"No TAG file found at {tag_file} - skipping.", file=sys.stderr)
        return 0

    with open(tag_file) as f:
        tag_dir = f.readline().strip()

    build_xml = os.path.join(build_dir, "Testing", tag_dir, "Build.xml")
    if not os.path.isfile(build_xml):
        print(f"No Build.xml found at {build_xml} - skipping.", file=sys.stderr)
        return 0

    tree = ET.parse(build_xml)
    root = tree.getroot()

    warnings = []
    errors = []
    for build_elem in root.iter("Build"):
        for warning in build_elem.findall("Warning"):
            text = warning.findtext("Text", "").strip()
            src = warning.findtext("SourceFile", "").strip()
            line = warning.findtext("SourceLineNumber", "").strip()
            if text:
                warnings.append((src, line, text))
        for error in build_elem.findall("Error"):
            text = error.findtext("Text", "").strip()
            src = error.findtext("SourceFile", "").strip()
            line = error.findtext("SourceLineNumber", "").strip()
            if text:
                errors.append((src, line, text))

    if not warnings and not errors:
        print("No build warnings or errors found.")
        return 0

    if errors:
        print(f"========== BUILD ERRORS ({len(errors)}) ==========")
        for src, line, text in errors:
            loc = f"{src}:{line}" if src and line else (src or "")
            prefix = f"  [{loc}] " if loc else "  "
            print(f"{prefix}{text}")
        print()

    if warnings:
        print(f"========== BUILD WARNINGS ({len(warnings)}) ==========")
        for src, line, text in warnings:
            loc = f"{src}:{line}" if src and line else (src or "")
            prefix = f"  [{loc}] " if loc else "  "
            print(f"{prefix}{text}")
        print()

    print("====================================================")

    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
