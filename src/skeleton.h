#ifndef H_AK_SKELETON
#define H_AK_SKELETON

#include <map>
#include <string>
#include <ctime>
#include <queue>
#include <utility>

#include <XnCppWrapper.h>

#include "sjr_action.h"
#include "typedefs.h"
#include "serializable.h"
#include "position.h"

class SkeletonFrame : public JsonSerializable {
	XnUserID id;
	JointMap joints;
	JointMap joints_last;
	JointMap deltas;
	std::map<std::string,XnSkeletonJoint> joint_types;
public:
	SkeletonFrame(XnUserID id);
	void UpdateFromDevice(xn::UserGenerator &user_generator);

	void WriteJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
protected:
	int UpdateJoint(std::string label, double x, double y, double z);
};

class SkeletonTracker {
	UserSkeletonMap skeletons;
	std::queue<sjr_action> action_q;

public:
	SkeletonTracker();
	std::string RenderUpdate();
	std::string RenderAction();
	int ActionsWaiting();
	bool UserExists(XnUserID uid);
	SkeletonFrame* GetUserSkeleton(XnUserID uid);
	SkeletonFrame* NewSkeleton(XnUserID uid);
	int PruneSkeletons(int nusers, XnUserID users[]);
	bool RemoveSkeleton(XnUserID uid);

protected:
	void QueueAction(const char *type, XnUserID uid);
	void WriteHeader(const char *action, time_t timestamp, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
};

#endif

