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

OPTS="--delete -cuvzPtlxr"

FILES=(
transfer.sh
utsrelease.h
vgabuffer.c
Makefile
.ycm_extra_conf.py
.color_coded
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

commasepfiles() {
    local IFS=,

    echo "${FILES[*]}"
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
    *)
        echo "Usage: $0 pushfiles|pushall|pullall"
        exit 1
        ;;
esac
