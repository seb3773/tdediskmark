#!/bin/bash
echo "=== tdeDiskMark Cleanup Script ==="

if [ -d "build" ]; then
    echo "Removing build directory..."
    rm -rf build
    echo "Done."
else
    echo "Build directory does not exist. Nothing to clean."
fi

# Also clean common IDE and temp files if necessary
rm -f *~
rm -f src/*~
rm -f meter-gauge/*~

echo "Workspace is clean."
