#ifndef MAGI_INCLUDED_FASTCGI
#define MAGI_INCLUDED_FASTCGI

#include "field.h"
#include "request.h"


struct magi_session {
    void (*callback)(struct magi_field * field, char * buffer, int len);
    int                       max_post;
    struct magi_socket_list * sockets;
};

int magi_fcgi(struct magi_session * session);

/*
 * Returns null if succeed, otherwise error code.
 */
int magi_accept(struct magi_request * request, struct magi_session * session);


#endif
