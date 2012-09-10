#include <string>
#include <map>
#include <ctime>
#include <utility>
#include <set>

#include <XnCppWrapper.h>

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "sjr_action.h"
#include "skeleton.h"
#include "position.h"

SkeletonFrame::SkeletonFrame(XnUserID id) {
	std::map<std::string,XnSkeletonJoint>::iterator it;
	this->id = id;
	this->joint_types["head"] = XN_SKEL_HEAD;
	this->joint_types["neck"] = XN_SKEL_NECK;
	this->joint_types["torso"] = XN_SKEL_TORSO;
	this->joint_types["waist"] = XN_SKEL_WAIST;
	this->joint_types["left_collar"] = XN_SKEL_LEFT_COLLAR;
	this->joint_types["right_collar"] = XN_SKEL_RIGHT_COLLAR;
	this->joint_types["left_shoulder"] = XN_SKEL_LEFT_SHOULDER;
	this->joint_types["right_shoulder"] = XN_SKEL_RIGHT_SHOULDER;
	this->joint_types["left_elbow"] = XN_SKEL_LEFT_ELBOW;
	this->joint_types["right_elbow"] = XN_SKEL_RIGHT_ELBOW;
	this->joint_types["left_wrist"] = XN_SKEL_LEFT_WRIST;
	this->joint_types["right_wrist"] = XN_SKEL_RIGHT_WRIST;
	this->joint_types["left_hand"] = XN_SKEL_LEFT_HAND;
	this->joint_types["right_hand"] = XN_SKEL_RIGHT_HAND;
	this->joint_types["left_fingertip"] = XN_SKEL_LEFT_FINGERTIP;
	this->joint_types["right_fingertip"] = XN_SKEL_RIGHT_FINGERTIP;
	this->joint_types["left_hip"] = XN_SKEL_LEFT_HIP;
	this->joint_types["right_hip"] = XN_SKEL_RIGHT_HIP;
	this->joint_types["left_knee"] = XN_SKEL_LEFT_KNEE;
	this->joint_types["right_knee"] = XN_SKEL_RIGHT_KNEE;
	this->joint_types["left_ankle"] = XN_SKEL_LEFT_ANKLE;
	this->joint_types["right_ankle"] = XN_SKEL_RIGHT_ANKLE;
	this->joint_types["left_foot"] = XN_SKEL_LEFT_FOOT;
	this->joint_types["right_foot"] = XN_SKEL_RIGHT_FOOT;
	for (it=this->joint_types.begin(); it != this->joint_types.end(); ++it) {
		this->joints[it->first] = Position(0.0, 0.0, 0.0);
	}
}

void SkeletonFrame::UpdateFromDevice(xn::UserGenerator &user_generator) {
	XnSkeletonJointTransformation joint;
	std::map<std::string,XnSkeletonJoint>::iterator it;
	// iterate through each joint and add it to the frame
	for (it=this->joint_types.begin(); it != this->joint_types.end(); ++it) {
		user_generator.GetSkeletonCap().GetSkeletonJoint(this->id, it->second, joint);
		this->UpdateJoint(it->first,
				  joint.position.position.X,
				  joint.position.position.Y,
				  joint.position.position.Z);
	}
	
}

int SkeletonFrame::UpdateJoint(std::string label, double x, double y, double z) {
	std::map<std::string,Position>::iterator it;
	it = this->joints.find(label);
	if (it == this->joints.end())
		return -1;
	this->joints_last[label] = it->second;
	it->second = Position(x, y, z);
	this->deltas[label] = this->joints_last[label].diff(it->second);
	return 0;
}

void SkeletonFrame::WriteJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) {
	std::map<std::string,Position>::iterator it;
	std::stringstream ss;
	ss << this->id;
	writer.String(ss.str().c_str());
	writer.StartObject();
	

	for (it=this->deltas.begin(); it != this->deltas.end(); ++it) {
		writer.String(it->first.c_str());
		it->second.WriteJson(writer);
	}
	writer.EndObject();
}

SkeletonTracker::SkeletonTracker() {
}

std::string SkeletonTracker::RenderUpdate() {
	std::string json_str;
	rapidjson::StringBuffer writer_buf; 
	JsonWriter writer(writer_buf);
	UserSkeletonMap::iterator it;
	SkeletonFrame *skel;
	writer.StartObject();
	this->WriteHeader("update", time(NULL), writer);
	writer.String("BODY");
	writer.StartObject();
	
	for (it = this->skeletons.begin(); it != this->skeletons.end(); ++it) {
		skel = &it->second;
		skel->WriteJson(writer);
	}
	writer.EndObject();
	writer.EndObject();
        json_str = writer_buf.GetString();
	return json_str;
}

std::string SkeletonTracker::RenderAction() {
	sjr_action action;
	std::string json_str;
	rapidjson::StringBuffer writer_buf; 
	JsonWriter writer(writer_buf);
	SkeletonFrame *skel;
	std::stringstream ss;
	action = this->action_q.front();
	this->action_q.pop();
	writer.StartObject();
	this->WriteHeader(action.type, action.timestamp, writer);
	if (strcmp("delete", action.type) == 0) {
		writer.String("BODY");
		writer.StartObject();
//		writer.StartObject();
		writer.String("id");
		ss << action.uid;
		writer.String(ss.str().c_str());
//		writer.EndObject();
		writer.EndObject();
	}
	else if (strcmp("add", action.type) == 0) {
		skel = this->GetUserSkeleton(action.uid);
		writer.String("BODY");
		writer.StartObject();		
		skel->WriteJson(writer);
		writer.EndObject();
	}
	writer.EndObject();
	json_str = writer_buf.GetString();
	return json_str;
}

int SkeletonTracker::ActionsWaiting() {
	return this->action_q.size();
}

bool SkeletonTracker::UserExists(XnUserID uid) {
	UserSkeletonMap::iterator it;
	it = this->skeletons.find(uid);
	if (it == this->skeletons.end())
		return false;
	return true;
}

SkeletonFrame* SkeletonTracker::GetUserSkeleton(XnUserID uid) {
	UserSkeletonMap::iterator it;
	it = this->skeletons.find(uid);
	if (it == this->skeletons.end()) {
		return NULL;
	}
	return &it->second;
}

SkeletonFrame* SkeletonTracker::NewSkeleton(XnUserID uid) {
	SkeletonFrame skeleton(uid);
	UserSkeletonMap::iterator it;
	// if there's already a skeleton with this UID, bail out
	if (this->UserExists(uid)) {
		return NULL;
	}
	this->QueueAction("add", uid);
	it = this->skeletons.insert(std::pair<XnUserID,SkeletonFrame>(uid, skeleton)).first;
	return &it->second;
}

/**
 * Delete any skeletons that are no longer being tracked.
 *
 * @return int number of skeletons deleted
 */
int SkeletonTracker::PruneSkeletons(int nusers, XnUserID users[]) {
	std::set<XnUserID> uids;
	UserSkeletonMap::iterator it;
	int nremoved, i;
	nremoved = 0;
	for (i=0; i < nusers; ++i) {
		uids.insert(users[i]);
	}
	for (it=this->skeletons.begin(); it != this->skeletons.end(); ++it) {
		if (uids.find(it->first) == uids.end()) {
			this->RemoveSkeleton(it->first);
			++nremoved;
		}
	}
	return nremoved;
}

bool SkeletonTracker::RemoveSkeleton(XnUserID uid) {
	UserSkeletonMap::iterator it;
	it = this->skeletons.find(uid);
	if (it == this->skeletons.end()) {
		return false;
	}
	this->QueueAction("delete", it->first);
	this->skeletons.erase(it);
	return true;
}

void SkeletonTracker::QueueAction(const char *type, XnUserID uid) {
	sjr_action a;
	a.type = type;
	a.uid = uid;
	a.timestamp = time(NULL);
	this->action_q.push(a);
}

void SkeletonTracker::WriteHeader(const char *action, time_t timestamp, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) {
	std::stringstream ss;
	ss << timestamp;
	writer.String("HEAD");
	writer.StartObject();
	writer.String("service");
	writer.String("skeletons");
	writer.String("timestamp");
	writer.String(ss.str().c_str());
	writer.String("action");
	writer.String(action);
	writer.EndObject();
}









