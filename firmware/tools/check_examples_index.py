#!/usr/bin/env python3
# This script checks that all example scripts under scripts/examples/
# are covered by an entry in scripts/examples/index.json, and that
# all index entries match at least one existing file.
import json
import sys
from fnmatch import fnmatch
from pathlib import Path

REPO_ROOT = Path(__file__).parent.parent
EXAMPLES_DIR = REPO_ROOT / "scripts" / "examples"
INDEX_FILE = EXAMPLES_DIR / "index.json"


def resolve_entry(path):
    """Resolve an index entry path to the set of .py files it covers."""
    full_path = EXAMPLES_DIR / path

    # Direct .py file reference
    if path.endswith(".py"):
        if full_path.exists():
            return {full_path.resolve()}
        return set()

    # Wildcard path like "50-OpenMV-Boards/50-STM32-Boards/*"
    if path.endswith("/*"):
        base = EXAMPLES_DIR / path[:-2]
        files = set()
        if base.is_dir():
            for child in base.iterdir():
                if child.is_dir():
                    files.update(p.resolve() for p in child.rglob("*.py"))
        return files

    # Directory path - covers all .py files recursively
    if full_path.is_dir():
        return {p.resolve() for p in full_path.rglob("*.py")}

    return set()


def main():
    with open(INDEX_FILE) as f:
        index = json.load(f)

    all_py_files = {p.resolve() for p in EXAMPLES_DIR.rglob("*.py")}
    covered = set()
    empty_entries = []

    for entry in index["entries"]:
        files = resolve_entry(entry["path"])
        if not files:
            empty_entries.append(entry["path"])
        covered.update(files)

    uncovered = sorted(all_py_files - covered)
    errors = False

    if uncovered:
        errors = True
        print("Example scripts not covered by index.json:")
        for f in uncovered:
            print(f"  {Path(f).relative_to(EXAMPLES_DIR)}")

    if empty_entries:
        errors = True
        print("index.json entries that don't match any files:")
        for e in sorted(empty_entries):
            print(f"  {e}")

    if errors:
        sys.exit(1)

    print("All example scripts are covered by index.json.")


if __name__ == "__main__":
    main()
