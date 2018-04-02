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
Makefile
main.cpp
testmain.cpp
testsg.cpp
libs
../shared_libs
arm_system.h
nios.dat
)

tryaddresses() {
    if ping -W 5 -c 1 "$DE1ADDR"; then
        return 0
    fi

    DE1ADDR="$DE1ALTADDR"

    if ping -W 5 -c 1 "$DE1ADDR"; then
        fixtime
        return 0
    fi

    return 1
}

fixtime() {
    ssh -t "$DE1USER@$DE1ADDR" sudo date +%Y%m%d -s "$(date +%Y%m%d)" '&&' sudo date +%T -s "$(date +%T)"
}

if tryaddresses; then
    rsync $OPTS "${FILES[@]}" "$DE1USER@$DE1ADDR:$DESTFOLDER/software_linux"
else
    echo "Cannot reach de1"
fi
