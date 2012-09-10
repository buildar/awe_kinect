#include <iostream>
#include <sstream>
#include <map>
#include <XnCppWrapper.h>
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

class Position;
class SkeletonFrame;

typedef std::map<XnUserID,SkeletonFrame> UserSkeletonMap;
typedef std::map<std::string,Position> JointMap;
typedef rapidjson::PrettyWriter<rapidjson::StringBuffer> JsonWriter;
