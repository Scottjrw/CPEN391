# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
		obj-m := vgabuffer.o
		ccflags-y := -std=gnu99 -Werror -Wno-declaration-after-statement
else
		KERNELDIR ?= linux-4.5
		PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

# Hack the module version so that it works on the de1's kernel
fixmoduleversion:
	rm -f linux-4.5/include/generated/utsrelease.h
	ln -s ../../../utsrelease.h linux-4.5/include/generated/

insmod:
	sudo insmod vgabuffer.ko

rmmod:
	sudo rmmod vgabuffer.ko

clean:
	rm -f .*.cmd *.o *.ko -r .tmp*
