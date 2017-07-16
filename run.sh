#!/bin/sh

gst-launch-1.0 -v -m --gst-plugin-path=`pwd` videotestsrc ! video/x-raw,width=720,height=480 ! gst-distort ! videoconvert! ximagesink

#gst-launch-1.0 -v --gst-plugin-path=`pwd` filesrc location=~/test.mp4 ! qtdemux ! queue ! avdec_h264 ! videoconvert ! gst-distort ! videoconvert ! xvimagesink
