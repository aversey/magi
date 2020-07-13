#ifndef MAGI_INCLUDED_MULTIPART
#define MAGI_INCLUDED_MULTIPART

#include "request.h"


void magi_parse_multipart(struct magi_request *request,
                          char                *boundary,
                          int                (*next)(void *userdata),
                          void                *next_userdata);


#endif
