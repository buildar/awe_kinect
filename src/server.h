#ifndef H_AK_SERVER
#define H_AK_SERVER

#include <libwebsockets.h>

// We support two protocols, one full-duplex websocket for pushing data
// updates and one plain HTTP that serves a simple Javascript client.
enum ws_protocols {
	PROTOCOL_HTTP,
	PROTOCOL_KINECT
};

// Initialize websocket server listening on port.
libwebsocket_context* init_ws_context(int port);
// Handle requests.
void ws_serve(libwebsocket_context *context);
void ws_skeleton_broadcast(std::string json_data, unsigned char *buf);

#endif
