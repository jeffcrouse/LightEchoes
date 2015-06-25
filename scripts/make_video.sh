#!/bin/bash

cd "${HOME}/Desktop"
echo "============= " `date` >> make_video.log

# See if the number of frames has changed since the last time we've generated a video 
COUNTFILE="framecount.txt" # Save it outside of the dropbox in case another machine runs this script
NEW_FRAMECOUNT=`ls -1 ${HOME}/Dropbox/LE\ Shared/_PhotosSmall/*.jpg | wc -l`
OLD_FRAMECOUNT=0
if [ -f "$COUNTFILE" ]; then
    OLD_FRAMECOUNT=$( < $COUNTFILE  )
    echo "FOUND $OLD_FRAMECOUNT FRAMES" >> make_video.log
fi



# If there are a different number of files, generate a video
if [ $NEW_FRAMECOUNT -ne $OLD_FRAMECOUNT ]; then
	echo "lock" >> video.lock
	/usr/local/bin/ffmpeg -y -f image2 -pattern_type glob -i "${HOME}/Dropbox/LE Shared/_PhotosSmall/*.jpg" -s 1920x1280 -c:v libx264 -an -q:v 2 -r 12 -pix_fmt yuv420p LightEchoes.mp4 &> make_video.log
	rm -rf video.lock
else
    echo "NO NEW FRAMES" >> make_video.log
fi

if [ $NEW_FRAMECOUNT -ne 0 ]; then
    echo "$NEW_FRAMECOUNT" > "$COUNTFILE"
fi