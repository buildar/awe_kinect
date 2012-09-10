#include <iostream>
#include <sstream>

#include <XnCppWrapper.h>
#include <libwebsockets.h>
#include <glog/logging.h>

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "ezOptionParser.hpp"
#include "constants.h"
#include "options.h"
#include "skeleton.h"
#include "position.h"
#include "util.h"
#include "server.h"

xn::UserGenerator g_user_generator;
SkeletonTracker g_skel_tracker;
XnBool g_need_pose = false;

XnBool file_exists(const char *path) {
	XnBool exists;
	xnOSDoesFileExist(path, &exists);
	return exists;
}

void XN_CALLBACK_TYPE new_user(xn::UserGenerator& user_generator,
			      XnUserID uid, void* pcookie)
{
	XnChar str_pose[20];
	LOG(INFO) << "New user: " << uid;
	if (g_need_pose) {
		user_generator.GetPoseDetectionCap().StartPoseDetection(str_pose, uid);
	} else {
		user_generator.GetSkeletonCap().RequestCalibration(uid, true);
	}
}

void XN_CALLBACK_TYPE lost_user(xn::UserGenerator& user_generator,
			      XnUserID uid, void* pcookie)
{
	g_skel_tracker.RemoveSkeleton(uid);
	LOG(INFO) << "Lost user" << uid;
}

void XN_CALLBACK_TYPE pose_detected(xn::PoseDetectionCapability& capability,
				   const XnChar* pose,
				   XnUserID uid,
				   void *pcookie)
{
	g_user_generator.GetPoseDetectionCap().StopPoseDetection(uid);
	g_user_generator.GetSkeletonCap().RequestCalibration(uid, true);
	LOG(INFO) << "Pose detected";

}

void XN_CALLBACK_TYPE calibration_start(xn::SkeletonCapability& capability,
				       XnUserID uid,
				       void *pcookie) {
	LOG(INFO) << "Starting calibration for user ID " << uid;

}

void XN_CALLBACK_TYPE calibration_end(xn::SkeletonCapability& capability,
				       XnUserID uid,
				       XnCalibrationStatus status,
				       void *pcookie) {
	XnChar str_pose[20];
	if (status == XN_CALIBRATION_STATUS_OK) {
		std::cout << "Calibration successful." << std::endl;
		g_user_generator.GetSkeletonCap().StartTracking(uid);
		g_skel_tracker.NewSkeleton(uid);
	} else {
		std::cout << "Failed to calibrate user " << uid << std::endl;
		if (g_need_pose) {
			g_user_generator.GetPoseDetectionCap().StartPoseDetection(str_pose, uid);
		}
		else {
			g_user_generator.GetSkeletonCap().RequestCalibration(uid, true);
		}
	}
}

int main(int argc, const char **argv) {
	ez::ezOptionParser op;
	std::stringstream msg;
	int ws_port;
	int update_freq;
	XnStatus ret = XN_STATUS_OK;
	XnCallbackHandle hn0, hn1, hn2, hn3, hn4;
	xn::DepthGenerator depth_generator;
	xn::ScriptNode script_node;
	xn::Context context;
	xn::EnumerationErrors errors;
	XnUserID users[kMaxUsers];
	XnUInt16 nusers = kMaxUsers;
	SkeletonFrame *skel;
	XnChar str_pose[20];
	struct libwebsocket_context* ws_context;

	XnSkeletonJointTransformation joint;

	unsigned char sjr_buf[LWS_SEND_BUFFER_PRE_PADDING + kMaxUpdateSize + LWS_SEND_BUFFER_POST_PADDING];
	std::string sjr_json;

	google::InitGoogleLogging(argv[0]);
	
	init_options(op);
	op.parse(argc, argv);
	if (op.isSet("-h")) {
		print_usage(op);
		return 0;
	}

	op.get("-p")->getInt(ws_port);
	op.get("-f")->getInt(update_freq);
	ws_context = init_ws_context(ws_port);
	ret = context.InitFromXmlFile("config.xml", script_node, &errors);
	if (ret != XN_STATUS_OK) {
		std::cout << "Open failed: " << xnGetStatusString(ret) << std::endl;
		return ret;
	}
	ret = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depth_generator);
	ret = context.FindExistingNode(XN_NODE_TYPE_USER, g_user_generator);
	if (ret != XN_STATUS_OK) {
		ret = g_user_generator.Create(context);
	}
	g_user_generator.RegisterUserCallbacks(new_user,
					     lost_user,
					     NULL,
					     hn0);
	g_user_generator.GetSkeletonCap().RegisterToCalibrationStart(calibration_start, NULL, hn1);
	g_user_generator.GetSkeletonCap().RegisterToCalibrationComplete(calibration_end, NULL, hn4);
	if (g_user_generator.GetSkeletonCap().NeedPoseForCalibration()) {
		g_need_pose = true;
		ret = g_user_generator.GetPoseDetectionCap().RegisterToPoseDetected(pose_detected, NULL, hn2);
		g_user_generator.GetSkeletonCap().GetCalibrationPose(str_pose);
	}
	g_user_generator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	context.StartGeneratingAll();
	if (g_need_pose) {
		std::cout << "Assume calibration pose." << std::endl;
	}
	for (;;) {
		nusers = kMaxUsers;
		context.WaitOneUpdateAll(g_user_generator);
		g_user_generator.GetUsers(users, nusers);
		for (int i = 0; i < nusers; ++i) {
			if (g_user_generator.GetSkeletonCap().IsTracking(users[i])) {
				if (g_skel_tracker.UserExists(users[i])) {
					skel = g_skel_tracker.GetUserSkeleton(users[i]);
					skel->UpdateFromDevice(g_user_generator);
				}
				else {
					LOG(ERROR) << "Received data for user ID " << users[i] << ", but no such skeleton is being tracked.";
				}
				
			}

		}
		
		// If there are actions waiting, render and broadcaset them
		while (g_skel_tracker.ActionsWaiting()) {
			sjr_json = g_skel_tracker.RenderAction();
			ws_skeleton_broadcast(sjr_json, sjr_buf);
			
		}
		// Broadcast skeleton update
		sjr_json = g_skel_tracker.RenderUpdate();
		ws_skeleton_broadcast(sjr_json, sjr_buf);
		ws_serve(ws_context);
		sleep_hz(update_freq);

	}

	script_node.Release();
	depth_generator.Release();
	g_user_generator.Release();
	context.Release();

	return 0;
}


