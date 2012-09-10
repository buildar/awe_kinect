#include <sstream>

#include "position.h"

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

Position::Position(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Position::Position() {
	this->x = 0.0;
	this->y = 0.0;
	this->z = 0.0;
}

void Position::WriteJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) {
	std::stringstream ss;
	writer.StartObject();
	writer.String("x");
	ss << this->x;
	writer.String(ss.str().c_str());
	ss << this->y;
	writer.String("y");
	writer.String(ss.str().c_str());
	ss << this->z;
	writer.String("z");
	writer.String(ss.str().c_str());
	writer.EndObject();
}

Position Position::diff(Position& p) {
	return Position(p.x - this->x, p.y - this->y, p.z - this->z);
}
		
