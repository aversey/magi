#ifndef MAGI_INCLUDED_SESSION
#define MAGI_INCLUDED_SESSION


typedef struct magi_session {
    int socket;
} magi_session;


void magi_session_init(magi_session *s);
void magi_session_free(magi_session *s);

int magi_session_inet(magi_session *s, const char *address, int port);
int magi_session_unix(magi_session *s, const char *path);


#endif
