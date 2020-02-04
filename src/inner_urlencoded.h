#ifndef MAGI_INCLUDED_INNER_URLENCODED
#define MAGI_INCLUDED_INNER_URLENCODED

#include "request.h"


/* Fills request->url_params via parsing encoded data. */
void magi_urlencoded(magi_param_list **list,
                     magi_request     *request,
                     const char       *encoded);


#endif
