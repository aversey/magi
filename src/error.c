#include "error.h"


const char * magi_error_message(enum magi_error error)
{
    const char * const messages[] = {
        0,                             /* magi_error_none */
        "No boundary for multipart.",  /* magi_error_nobound */
        "Content-Type is unknown.",    /* magi_error_unknown */
        "Content-Type is not given.",  /* magi_error_notype */
        "Mismatch of content length.", /* magi_error_length */
        "Cannot allocate memory.",     /* magi_error_memory */
        "Cannot read cookies.",        /* magi_error_cookies */
        "Cannot decode URL.",          /* magi_error_urlenc */
        "Cannot read multipart body.", /* magi_error_multipart */
        "Part of request was too big." /* magi_error_limit */
    };
    return messages[error];
}
