#ifndef MAGI_INCLUDED_URLENCODED
#define MAGI_INCLUDED_URLENCODED

#include "error.h"
#include "param.h"


/* Add decoded params from 'encoded' to 'list'. */
magi_error magi_urlencoded(magi_param_list **list, const char *encoded);


#endif
