#include <iostream>
#include <cstring>
#include <libwebsockets.h>

#include <glog/logging.h>

#include "server.h"
#include "constants.h"
#include "skeleton.h"

extern SkeletonFrame g_skel_frame;

struct per_session_data_skeleton {
	
};

static int callback_http(struct libwebsocket_context *context,
			 struct libwebsocket *wsi,
			 enum libwebsocket_callback_reasons reason,
			 void *user,
			 void *in,
			 size_t len) {
	int ret;

	std::cout << "HTTP callback\n";
	switch (reason) {
	case LWS_CALLBACK_HTTP:
		ret = libwebsockets_serve_http_file(wsi, "index.html", "text/html");
		if (ret)
			std::cout << "Failed to send file via HTTP." << std::endl;
		break;
	}
	return 0;
}

static int callback_skeleton_data(struct libwebsocket_context *context,
			 struct libwebsocket *wsi,
			 enum libwebsocket_callback_reasons reason,
			 void *user,
			 void *in,
			 size_t len) {
	unsigned char *buf_pos;
	
	switch (reason) {
	case LWS_CALLBACK_ESTABLISHED:
		LOG(INFO) << "Client connected.";
		break;
	case LWS_CALLBACK_BROADCAST:
		LOG(INFO) << "Broadcast callback: writing data";
		libwebsocket_write(wsi, (unsigned char *) in, len, LWS_WRITE_TEXT);
	default:
		break;
	}
	return 0;
}

static struct libwebsocket_protocols protocols[] = {
	{
		"http-only",
		callback_http,
		0
	},
	{
		"skeleton-data",
		callback_skeleton_data,
		sizeof(struct per_session_data_skeleton)
	},
	{
		NULL,
		NULL,
		0
	}
};
	
libwebsocket_context* init_ws_context(int port) {
	libwebsocket_context *context;
	LOG(INFO) << "Starting server on port " << port;
	context = libwebsocket_create_context(port,
					      NULL,
					      protocols,
					      NULL,
					      NULL,
					      NULL,
					      -1,
					      -1,
					      0);
	if (context == NULL) {
		LOG(FATAL) << "libwebsocket init failed.";
	}
	
}

void ws_serve(libwebsocket_context *context) {
	libwebsocket_service(context, 50);
}

/**
 * Writes the characters in data to buf, leaving LWS_SEND_BUFFER_PRE_PADDING
 * bytes at the start and and LWS_SEND_BUFFER_POST_PADDING bytes at the end.
 * Returns the number of characters written.
 */
int write_update_buf(std::string data, unsigned char *buf) {
	int i, nchars;
	unsigned char *buf_pos;
	buf_pos = &buf[LWS_SEND_BUFFER_PRE_PADDING];
	nchars = 0;
	for (i = 0; i < data.size(); ++i) {
		*(buf_pos++) = (unsigned char) data[i];
		++nchars;
	}
	return nchars;
}

void ws_skeleton_broadcast(std::string json_data, unsigned char *buf) {
	int nchars;
	nchars = write_update_buf(json_data, buf);
	libwebsockets_broadcast(&protocols[1], buf, nchars);
}
