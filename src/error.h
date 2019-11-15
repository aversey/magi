#ifndef MAGI_INCLUDED_ERROR
#define MAGI_INCLUDED_ERROR


struct magi_error {
    struct magi_error * prev;
    char *              message; /* enum magi_error_type { ... } type; */
};


void magi_error_add(struct magi_error ** error, char * format, ...);
void magi_error_destroy(struct magi_error * error);


#endif
