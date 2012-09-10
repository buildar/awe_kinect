#ifndef H_AK_SJR_ACTION
#define H_AK_SJR_ACTION

#include <string>
#include <ctime>

#include <XnCppWrapper.h>

/**
 * Represents a SOAPjr action.
 */
typedef struct {
	time_t timestamp;
	const char *type;
	XnUserID uid;
} sjr_action;

#endif
	
	
