#include <iostream>

#include <glog/logging.h>

#include "util.h"

#ifdef PLATFORM_POSIX

#include <unistd.h>

void sleep_hz(int freq) {
	usleep((int) 1000.0/(double) freq);
}
#endif

#ifdef PLATFORM_WINDOWS

#include <windows.h>

void sleep_hz(int freq) {
	Sleep((DWORD) 1000.0/((double) freq));
}

#endif
