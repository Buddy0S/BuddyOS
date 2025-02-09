#!/bin/bash

../../qemu-system-arm -M beagle -cpu cortex-a8 -m 512M -sd ../../sdcard.img -clock unix -serial stdio -usb -device usb-kbd -k /usr/share/qemu/keymaps/en-us
