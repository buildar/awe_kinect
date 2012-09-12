#ifndef H_AK_DUMP_WRITER
#define H_AK_DUMP_WRITER

#include <string>
#include <fstream>

const int kMaxBufferSize = 50;

class DumpWriter {
	std::ofstream stream;
	std::string path;
	int writes_since_sync;

public:
	DumpWriter(std::string fname);
	void WriteData(std::string data);
	void Flush();
	bool IsOpen();
};

#endif
