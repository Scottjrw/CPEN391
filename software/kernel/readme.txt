This for the development of our kernel module(s)

We keep a version of the linux kernel for building the modules.

This version is linux 4.5 since the ubuntu distro provided by Terasic is linux 4.5. But this kernel is not the same kernel as the one running in the distro, so don't try to compile and actually boot from it, we just build modules using the sources.

There is no way to get the real kernel sources to build our module. This is because unfortunately someone (an idiot) deleted the branch for the actual kernel in the altera socfpga linux repository (https://github.com/altera-opensource/linux-socfpga). The kernels should be close enough that simply praying should be enough to get our module working on the de1 soc...

file breakdown:
linux-4.5 - folder containing linux 4.5, gotten straight from kernel.org
utsrelease.h - special auto-generated file, linked to from linux-4.5/include/generated/utsrelease.h. The version in this file is very important as it must match the de1 soc kernel exactly, we force it to since we are actually using a different kernel version.
Makefile - fancy "kernel style" makefile to compile our module
transfer.sh - small script used to copy files back and forth to de1 soc over network via rsync (Need ssh to be set up)
Other files - module code and auto generated files
