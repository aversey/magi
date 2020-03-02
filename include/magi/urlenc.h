#ifndef MAGI_INCLUDED_URLENC
#define MAGI_INCLUDED_URLENC
/** @file urlenc.h
 * @brief Realisation of URL-encoding.
 *
 * Can be helpful in forming urls in response.  Use #magi_urlenc for
 * encoding itself and #magi_urlenc_size to find what the size of code
 * will be.
 *
 * RFC 3986 describes URL-encoding.  Briefly it is changing every space into
 * plus sign and every not alpha-numerical and not @c "~-._" character into
 * percent sign followed by hexadecimal representation of given character.
 *
 * @note This module is optional.
 */


/** Count URL-code size for @p plain.
 * @param[in] plain is a text to count code size for.
 * @return size of URL-code of @p plain. */
int magi_urlenc_size(const char *plain);

/** Encode @p plain to url-code @p code.
 * @warning @p code must be at least size of #magi_urlenc_size(@p plain).
 * @param[in] plain is a text to be encoded.
 * @param[out] code will be filled with URL-code of @p plain. */
void magi_urlenc(const char *plain, char *code);


#endif
