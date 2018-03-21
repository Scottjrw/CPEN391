# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
		obj-m := cpen391_vgabuffer.o
		cpen391_vgabuffer-y := vgabuffer.o
		ccflags-y := -std=gnu99 -Werror -Wno-declaration-after-statement
else
		KERNELDIR ?= linux-4.5
		PWD := $(shell pwd)
		ALTERA_UART_DIR ?= drivers/tty/serial

# Build vgabuffer driver
default: fixmoduleversion
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

# Build the altera_uart driver, needed to support the UART (RS-232) Qsys Module in Linux
altera_uart: fixmoduleversion altera_uart_clean altera_uart_link
	$(MAKE) -C $(KERNELDIR) M=$(ALTERA_UART_DIR) modules
	cp $(KERNELDIR)/$(ALTERA_UART_DIR)/altera_uart.ko $(PWD)

altera_uart_link:
	rm -f $(KERNELDIR)/$(ALTERA_UART_DIR)/altera_uart.c
	ln -s ../../../../altera_uart.c $(KERNELDIR)/$(ALTERA_UART_DIR)/altera_uart.c

altera_uart_clean:
	cd $(KERNELDIR)/$(ALTERA_UART_DIR) && rm -f altera_uart.ko altera_uart.o altera_uart.mod.o altera_uart.mod.c

altera_uart_install: altera_uart
	-sudo modprobe -r altera_uart
	sudo cp altera_uart.ko /lib/modules/4.5.0-00183-g4647b69-dirty/
	sudo depmod -a
	sudo modprobe altera_uart

# Hack the module version so that it works on the de1's kernel
fixmoduleversion:
	rm -f linux-4.5/include/generated/utsrelease.h
	ln -s ../../../utsrelease.h linux-4.5/include/generated/

install:
	-sudo modprobe -r cpen391_vgabuffer
	sudo cp cpen391_vgabuffer.ko /lib/modules/4.5.0-00183-g4647b69-dirty/
	sudo depmod -a
	sudo modprobe cpen391_vgabuffer

clean:
	rm -f .*.cmd *.o *.ko -r .tmp*

testvga: testvga.o
	$(CC) -o testvga testvga.o -std=c99

testuart: testuart.o
	$(CC) -o testuart testuart.o -std=c99

testregs: testregs.o
	$(CC) -o testregs testregs.o -std=c99

endif

