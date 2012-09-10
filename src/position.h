#ifndef H_AK_POSITION
#define H_AK_POSITION

#include "serializable.h"

// Three-dimensional vector class
class Position : public JsonSerializable {
	double x;
	double y;
	double z;

public:
	Position(double x, double y, double z);
	Position();
	// Write JSON representation of position, e.g.,
	// "{'x' : 0.2, 'y' : -2.4, 'z' : -0.5}" 
	void WriteJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	// Subtract this from p and return the resulting Position
	Position diff(Position& p);
};

#endif 
