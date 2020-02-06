#ifndef MAGI_INCLUDED_INNER_MULTIPART
#define MAGI_INCLUDED_INNER_MULTIPART

#include "request.h"


void magi_multipart(magi_request *request,
                    char         *boundary,
                    int         (*next)(void *userdata),
                    void         *next_userdata);


#endif
