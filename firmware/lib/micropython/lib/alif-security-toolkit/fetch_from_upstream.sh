#!/bin/bash
#
# Fetch new code for the Alif Security Toolkit, and clean it up a little.

if [ $# -ne 1 ]; then
    echo "usage: $0 <src>"
    echo ""
    echo "eg: $0 path/to/app-release"
    exit 1
fi

alifsrc=$1
alifdest=./toolkit/

if [ ! -f $alifsrc/maintenance.py ]; then
    echo "ERROR: $alifsrc does not contain an Alif Security Toolkit"
    exit 1
fi

# Remove any old files and copy across the new ones.
echo "Fetching Alif Security Toolkit"
rm -rf $alifdest
mkdir -p $alifdest
cp -r $alifsrc/* $alifdest

# Remove __pycache__.
rm -rf `find $alifdest -name __pycache__`

# Convert newlines to Unix style, and ensure there's a newline at the end of the file.
for file in `find $alifdest -regex '.+\.\(py\|txt\|json\)'`; do
    cat $file | sed 's/$//' | sed '$a\' > tmp$$
    /bin/mv tmp$$ $file
done

# Make sure permission flags are sensible.
for file in `find $alifdest -type f`; do
    if grep -q argparse $file; then
        chmod 755 $file
    else
        chmod 644 $file
    fi
done

# Format Python code.
ruff format `find $alifdest -name \*.py`
