#!/bin/bash

#REMOTE=afp://admin:barbican@LELaser._afpovertcp._tcp.local/admin
REMOTE=afp://jeff:123@goldfish._afpovertcp._tcp.local/jeff
MOUNTPT=/Volumes/LELaser
SOURCE=/Volumes/LELaser/Desktop/LightEchoesSmall
DEST=~/Desktop
LOG=~/Desktop/sync.log
LOCKFILE=~/Desktop/sync.lock

mkdir -p $MOUNTPT >> $LOG
mkdir -p $DEST >> $LOG

if mount|grep $MOUNTPT; then
	echo "mounted" >> $LOG
else 
	echo "mounting..." >> $LOG
	mount_afp -s $REMOTE $MOUNTPT >> $LOG
fi

echo "lock" >> $LOCKFILE

rsync -auv --timeout 10 --exclude ".DS_Store" --omit-dir-times $SOURCE $DEST >> $LOG

sleep 3

rm -rf $LOCKFILE >> $LOG