Alif Security Toolkit
=====================

This repository contains a copy of the Alif Security Toolkit, for Alif
Semiconductor microcontrollers.  Minor changes are applied to the Toolkit to
make it easier to use within a wider project.

The "vendor" branch contains the original Toolkit source and is updated from
time-to-time when new versions become available.  The commits in the "vendor"
branch are tagged as appropriate with a tag that describes the Toolkit release
at that commit, for example v1.104.0.

There are then working branches that branch at a given vendor tag and apply
minor patches to the vendor code, for example work-v1.104.0.  The commits that
form a given working branch are reapplied (with conflict resolution) to newer
vendor tags to create the next working branch.

Original sources
================

The sources are obtained from Alif Semiconductor, and placed in the `toolkit`
directory.

The provided `fetch_from_upstream.sh` script can be used to copy and process a
new Toolkit release into this repository.  It does some cleaning and formatting
of the Toolkit source files to make them more uniform.
