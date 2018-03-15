#! /bin/bash

# A simple script to move froms to and from the de1 soc
# The need arises since de1 soc is the only one that can config/build the kernel and modules
# while development on the local computer is much more time efficient
# push - push changes from local computer to de1 soc
# pull - pull changes from de1 soc to local computer

DE1USER=ubuntu
DE1ADDR=de1_soc

DESTFOLDER=/home/ubuntu/Desktop
SRCFOLDER=/home/guoj/Documents/Year-3/CPEN-391/repo/software

OPTS="--delete -cuvzPtlxr"

case "$1" in
    push)
        rsync $OPTS "$SRCFOLDER/kernel" "$DE1USER@$DE1ADDR:$DESTFOLDER"
        ;;
    pull)
        rsync $OPTS "$DE1USER@$DE1ADDR:$DESTFOLDER/kernel" "$SRCFOLDER"
        ;;
    *)
        echo "Usage: $0 push|pull"
        exit 1
        ;;
esac
