This for the development of our kernel module(s)

We keep a version of the linux kernel for building the modules.

This version is linux 4.5 since the ubuntu distro provided by Terasic is linux 4.5. But this kernel is not the same kernel as the one running in the distro, so don't try to compile and actually boot from it, we just build modules using the sources.

There is no way to get the real kernel sources to build our module. This is because unfortunately someone deleted the branch for the actual kernel in the altera socfpga linux repository (https://github.com/altera-opensource/linux-socfpga). The solution is to hack this kernel's version so that we can build modules for the Terasic kernel.

Install:
No need to compile again, just copy Makefile, cpen391_vgabuffer.ko to some folder on DE1, run 'make install' in the folder

File breakdown:
linux-4.5/ - folder containing the Linux 4.5 kernel, downloaded from kernel.org
linux-4.5/.config - config from Terasic kernel
utsrelease.h - special auto-generated file, linked to from linux-4.5/include/generated/utsrelease.h. The version in this file is very important as it must match the de1 soc kernel exactly, we force it to since we are actually using a different kernel version.
Makefile - fancy "kernel style" makefile to compile our module
transfer.sh - small script used to copy files back and forth to de1 soc over network via rsync (Need ssh to be set up)
vgabuffer.c - HPS ram for VGA buffer
cpen391_vgabuffer.ko - Latest compilation of the kernel module, included as a convenience

boot/ - stuff to do with the boot process of the HPS
boot/socfpga_cyclone5.dts - device tree for the HPS, it is modified from the original device tree binary that Terasic provided.
boot/socfpga.dtb - latest compilation binary of the device tree
boot/soc_system.rbf - latest hardware system bitstream, the HPS programs the FPGA using this at boot. This is coverted over from our sof.
