#!/bin/zsh

# Builds and runs the project.

make -j12
qemu-system-i386 -drive format=raw,file=bin/target/i386/os.img,media=disk,index=0
