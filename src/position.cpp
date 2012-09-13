#include <sstream>
#include <iostream>

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
	std::stringstream x;
	std::stringstream y;
	std::stringstream z;
	writer.StartObject();
	writer.String("x");
	x << this->x;
	writer.String(x.str().c_str());
	y << this->y;
	writer.String("y");
	writer.String(y.str().c_str());
	z << this->z;
	writer.String("z");
	writer.String(z.str().c_str());
	writer.EndObject();
}

Position Position::diff(Position& p) {
	return Position(p.x - this->x, p.y - this->y, p.z - this->z);
}
		
