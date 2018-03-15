# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
		obj-m := vgabuffer.o
else
		KERNELDIR ?= linux-4.5
		PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	$(RM) .*.cmd *.o *.ko -r .tmp*
