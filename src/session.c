#include "session.h"

#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>


void magi_session_init(magi_session *s)
{
    s->socket = 0;
}

void magi_session_free(magi_session *s)
{
    if (s->socket) {
        shutdown(s->socket, SHUT_RDWR);
        s->socket = 0;
    }
}


int magi_session_inet(magi_session *s, const char *address, int port)
{
    struct sockaddr_in addr;
    s->socket            = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);
    return !connect(s->socket, (struct sockaddr *)&addr, sizeof(addr));
}

int magi_session_unix(magi_session *s, const char *path)
{
    struct sockaddr_un addr;
    s->socket       = socket(AF_UNIX, SOCK_STREAM, IPPROTO_TCP);
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    return !connect(s->socket, (struct sockaddr *)&addr, sizeof(addr));
}
