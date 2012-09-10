#include <iostream>
#include <string>

#include "options.h"

void init_options(ez::ezOptionParser& op) {
	op.overview = "awe_kinect is a skeleton data publishing daemon for the OpenNI framework.";
	op.syntax = "awe_kinect [options]";
	op.example = "awe_kinect -p 8080\n\n";
	op.footer = "awe_kinect copyright 2012 Rob Manson and Sharif Olorin.\n";

	op.add("",
	       0,
	       0,
	       0,
	       "Display usage instructions.",
	       "-h",
	       "--help"
		);
	
	op.add("80",
	       0,
	       1,
	       0,
	       "Port number to serve (defaults to 80).",
	       "-p",
	       "--port"
		);

	op.add("30",
	       0,
	       1,
	       0,
	       "Update frequency, in hertz.",
	       "-f",
	       "--frequency"
		);
		
}

void print_usage(ez::ezOptionParser& op) {
	std::string usage;
	op.getUsage(usage);
	std::cout << usage;
}
