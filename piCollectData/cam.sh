#!/bin/sh

# Save pictures once a second.

while true; do
/usr/bin/fswebcam --no-banner -r 1280x720 /home/pi/camera/$(date +%s).jpg
sleep 1
done
