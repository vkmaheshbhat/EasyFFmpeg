# EasyFFmpeg

This is a wrapper to the famous FFmpeg library to enable "easy and simple" frame-by-frame en-/de-coding of video/audio data. This is inspired by the CvCapture interface of OpenCV library and is there to avoid having to include the whole OpenCV code libs to gain this relatively straightforward codec requirement.

Hence, this library is intended to be header only and provides only basic direct codec functionality. For example, it has no internal multi-threading, can't en-/de-code audio and video simultaneously, etc. The library is inteded for the use of multimedia tool developers who wish to, say, analyze different media for statistical purposes, extract features out of them etc.
