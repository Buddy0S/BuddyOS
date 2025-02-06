#!/bin/bash


sudo losetup --partscan /dev/loop0 ./buddy.img
sudo mount /dev/loop0p1 /mnt/beagle
sudo rm -rf /mnt/beagle/MLO
sudo cp ./MLO /mnt/beagle
sudo umount /mnt/beagle
sudo losetup -D /dev/loop0
