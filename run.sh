#!/bin/bash

echo "=== Building OS ==="
make clean
make

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "=== Starting QEMU ==="
qemu-system-x86_64 \
    -drive format=raw,file=build/os-image.img \
    -m 512M \
    -serial stdio \
    -no-reboot \
    -d guest_errors,cpu_reset

echo "=== QEMU exited ==="