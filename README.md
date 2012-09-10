awe_kinect
==========

A utility which reads skeleton orientation data from a device (such as a 
Kinect) via OpenNI and publishes it in JSON via WebSocket. 

Currently only tested on Linux, but a Windows build is coming.

Requirements
============

- OpenNI (http://www.openni.org/)
- Primesense NITE (http://www.openni.org/Downloads/OpenNIModules.aspx)
- libwebsockets (http://git.warmcat.com/cgi-bin/cgit/libwebsockets/)
- glog (http://code.google.com/p/google-glog/)
- scons (http://www.scons.org/)

To build, just run _scons_ from the root of the repository.