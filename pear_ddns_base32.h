#ifndef _PEAR_DDNS_BASE32_H_
#define _PEAR_DDNS_BASE32_H_

/*
IS: in bit-by-bit manner; e.g. 4B -- 7B
NOT: traditional in 5 bytes -- out 8 bytes with padding always.
THUS saves space.
*/

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @function pear_ddns_base32_enc
 * Encodes s_len bytes from src to base32 and stores it in dest.
 *
 * @param char * dest  pointer to destination where encoding will be stored
 * @param const void * src   pointer to data to be encoded
 * @param size_t s_len length of data to be encoded
 * @return size of encoded string
 */

size_t pear_ddns_base32_enc(char *dest, const void *src, size_t src_len);

/**
 * @function pear_ddns_base32_dec
 *
 * Decodes {@link src} bytes into dest
 *
 *
 * This function decodes an arbitrary length source buffer into
 * destination buffer, until dest is filled.
 *
 *
 * @param dest      where decoded content will end up
 * @param dest_len  size of destination (data gets truncated)
 * @param src       encoded string
 * @return  0 on success, -1 on failure
 *
 * @todo Should return offset into src up to where decoding occured
 *
 */
size_t pear_ddns_base32_dec(void *dest, size_t dest_len, const char *src);

/*
 * @function pear_ddns_base32_encsize
 *
 * Calculate size of encoded text for a buffer of count size
 *
 * @param count
 *
 * @return size of encoded buffer
 */

size_t pear_ddns_base32_encsize(size_t count);

/* size of decoded text */
size_t pear_ddns_base32_decsize(size_t count);

#endif
