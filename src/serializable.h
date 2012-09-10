#ifndef H_AK_SERIALIZABLE
#define H_AK_SERIALIZABLE

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

// Abstract interface for objects that can be represented in JSON
class JsonSerializable {
public:
	virtual void WriteJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)=0;
};

#endif
