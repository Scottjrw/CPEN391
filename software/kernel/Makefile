# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
		obj-m := vgabuffer.o
		ccflags-y := -std=gnu99 -Werror -Wno-declaration-after-statement
else
		KERNELDIR ?= linux-4.5
		PWD := $(shell pwd)
		ALTERA_UART_DIR ?= drivers/tty/serial
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

altera_uart: fixmoduleversion altera_uart_clean altera_uart_link
	$(MAKE) -C $(KERNELDIR) M=$(ALTERA_UART_DIR) modules
	cp $(KERNELDIR)/$(ALTERA_UART_DIR)/altera_uart.ko $(PWD)

altera_uart_link:
	rm -f $(KERNELDIR)/$(ALTERA_UART_DIR)/altera_uart.c
	ln -s ../../../../altera_uart.c $(KERNELDIR)/$(ALTERA_UART_DIR)/altera_uart.c

altera_uart_clean:
	cd $(KERNELDIR)/$(ALTERA_UART_DIR) && rm -f altera_uart.ko altera_uart.o altera_uart.mod.o altera_uart.mod.c

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

altera_uart_install:
	sudo modprobe -r altera_uart
	sudo cp altera_uart.ko /lib/modules/4.5.0-00183-g4647b69-dirty/
	sudo depmod -a
	sudo modprobe altera_uart

testvga: testvga.o
	$(CC) -o testvga testvga.o -std=c99

testuart: testuart.o
	$(CC) -o testuart testuart.o -std=c99


