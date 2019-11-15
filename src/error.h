#ifndef MAGI_INCLUDED_ERROR
#define MAGI_INCLUDED_ERROR


struct magi_error {
    char * message;
};


struct magi_error * magi_error_get();
void                magi_error_set(char * format, ...);
void                magi_error_rid();


#endif
