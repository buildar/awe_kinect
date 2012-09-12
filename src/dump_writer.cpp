#include "dump_writer.h"

#include <string>
#include <fstream>
#include <iostream>

#include <glog/logging.h>



DumpWriter::DumpWriter(std::string fname) {
	this->writes_since_sync = 0;
	this->path = fname;
	this->stream.open(this->path.c_str());
	if (!this->IsOpen()) {
		LOG(ERROR) << "Failed to open file " << this->path << " for writing.";
	}
}

void DumpWriter::WriteData(std::string data) {
	if (!this->IsOpen()) {
		LOG(ERROR) << "Can't write to " << this->path << "; stream isn't open.";
		return;
	}
	this->stream << std::endl;
	this->stream << data;
	this->stream << std::endl;
	if (this->writes_since_sync > kMaxBufferSize) {
		this->Flush();
		this->writes_since_sync++;
	}
}

void DumpWriter::Flush() {
	this->stream.flush();
	this->writes_since_sync = 0;
}

bool DumpWriter::IsOpen() {
	return !this->stream.fail();
}
