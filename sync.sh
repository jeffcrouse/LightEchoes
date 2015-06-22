#!/bin/bash

MOUNTPT=/Volumes/LELaser
SOURCE=/Volumes/LELaser/Desktop/LightEchoesSmall
DEST=~/Desktop
LOG=~/Desktop/sync.log

mkdir -p $MOUNTPT
mkdir -p $DEST

if mount|grep $MOUNTPT; then
	echo "mounted" > $LOG
else 
	echo "mounting..." > $LOG
	mount_afp -s afp://admin:barbican@LELaser._afpovertcp._tcp.local/admin $MOUNTPT > $LOG
fi

rsync -auvz --timeout 10 --exclude ".DS_Store" --omit-dir-times $SOURCE $DEST > $LOG
	