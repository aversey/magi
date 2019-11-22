#ifndef MAGI_INCLUDED_FASTCGI
#define MAGI_INCLUDED_FASTCGI

#include "error.h"
#include "file.h"
#include "request.h"


struct magi_session {
    struct magi_socket_list * sockets;
    enum magi_error           error;
};

int magi_session_fcgi(struct magi_session * session, int socket);


/* Returns null only in case of error. */
int magi_request_fcgi(struct magi_request * request,
                      struct magi_session * session);


#endif
