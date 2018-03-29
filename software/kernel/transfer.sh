#! /bin/bash

# A simple script to move froms to and from the de1 soc
# The need arises since de1 soc is the only one that can config/build the kernel and modules
# while development on the local computer is much more time efficient
# push - push changes from local computer to de1 soc
# pull - pull changes from de1 soc to local computer

DE1USER=ubuntu
DE1ADDR=de1_soc
DE1ALTADDR=10.0.0.1

DESTFOLDER=/home/ubuntu/Desktop
SRCFOLDER=/home/guoj/Documents/Year-3/CPEN-391/repo/software

OPTS="--delete -cvzPlxr"

FILES=(
utsrelease.h
vgabuffer.c
Makefile
.ycm_extra_conf.py
.color_coded
testvga.c
testuart.c
testregs.c
)

RESULTFILES=(
altera_uart.ko
cpen391_vgabuffer.ko
)

tryaddresses() {
    if ping -W 5 -c 1 "$DE1ADDR"; then
        return 0
    fi

    DE1ADDR="$DE1ALTADDR"

    if ping -W 5 -c 1 "$DE1ADDR"; then
        return 0
    fi

    return 1
}

fixtime() {
    ssh "$DE1USER@$DE1ADDR" sudo date +%Y%m%d -s "$(date +%Y%m%d)" && sudo date +%T -s "$(date +%T)"
}

commasepfiles() {
    local IFS=,

    echo "${FILES[*]}"
}

commasepresult() {
    local IFS=,

    echo "${RESULTFILES[*]}"
}

case "$1" in
    pushfiles)
        if tryaddresses; then
            rsync $OPTS "${FILES[@]}" "$DE1USER@$DE1ADDR:$DESTFOLDER/kernel"
        else
            echo "Cannot reach de1"
        fi
        ;;

    pullfiles)
        if tryaddresses; then
            rsync $OPTS "$DE1USER@$DE1ADDR:$DESTFOLDER/kernel/{$(commasepfiles)}" "$SRCFOLDER/kernel"
        else
            echo "Cannot reach de1"
        fi
        ;;

    pushall)
        if tryaddresses; then
            rsync $OPTS "$SRCFOLDER/kernel" "$DE1USER@$DE1ADDR:$DESTFOLDER"
        else
            echo "Cannot reach de1"
        fi
        ;;

    pullall)
        if tryaddresses; then
            rsync $OPTS "$DE1USER@$DE1ADDR:$DESTFOLDER/kernel" "$SRCFOLDER"
        else
		    echo "Cannot reach de1"
		fi
		;;

    pullresult)
		if tryaddresses; then
            rsync $OPTS "$DE1USER@$DE1ADDR:$DESTFOLDER/kernel/{$(commasepresult)}" "$SRCFOLDER/kernel"
        else
		    echo "Cannot reach de1"
		fi
        ;;

    fixtime)
		if tryaddresses; then
            fixtime
        else
		    echo "Cannot reach de1"
		fi
        ;;

    boot)
		if tryaddresses; then
		    scp ./boot/soc_system.rbf ./boot/socfpga.dtb "$DE1USER@$DE1ADDR:/tmp/"
            ssh -t "$DE1USER@$DE1ADDR" sudo cp -v /tmp/socfpga.dtb /tmp/soc_system.rbf /boot/uboot/ && echo ok
        else
		    echo "Cannot reach de1"
		fi
		;;
    *)
        echo "Usage: $0 pushfiles|pullfiles|pushall|pullall|pullresult|fixtime|boot"
        exit 1
        ;;
esac
