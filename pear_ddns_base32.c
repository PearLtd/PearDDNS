/*
 * Copyright (C) 2016 Pear Limited
 */

#include <string.h>
#include <stdint.h>
#include "pear_ddns_base32.h"

static const unsigned char pear_ddns_base32_enc_table[] =
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', /* 10 */
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',           /* 18 */
    'j', 'k',                                         /* 20 */
    'm', 'n',                                         /* 22 */
    'p', 'q', 'r', 's', 't',                          /* 27 */
    'v', 'w', 'x', 'y', 'z'                           /* 32 */
};

static const unsigned char pear_ddns_base32_dec_table[] = {
/*                                            Special Shit                                          */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*	SP !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
/*  @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  */
	0,10,11,12,13,14,15,16,17, 0,18,19, 0,20,21, 0, 22,23,24,25,26,0,27,28,29,30,31, 0, 0, 0, 0, 0,
/*  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~     */
	0,10,11,12,13,14,15,16,17, 0,18,19, 0,20,21, 0, 22,23,24,25,26,0,27,28,29,30,31, 0, 0, 0, 0, 0,
/*                                                  ‘  ’                                            */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                      Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                      Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                      Un-American Gibberish                                       */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
/* Encoding/Decoding Tables */

/* Pear uses a special padding character 'i'.
  Because '-' is not allowed in the beginning of a (sub)domain name,
  and consecutive '-'s may cause some problems.
 */
static const char pear_ddns_base32_padding = 'i';

/*
 *    11111111 10101010 10110011 10111100 10010100
 *->  11111 11110 10101 01011 00111 01111 00100 10100
 */

static int pear_ddns_base32_encode_block(char *dest, const void *_src)
{
    const unsigned char *src = _src;      /* cast it to a unsigned char */
    dest[0] = pear_ddns_base32_enc_table[(src[0] >> 3) & 0x1F];                          /* first 5 bits */
    dest[1] = pear_ddns_base32_enc_table[(src[0] << 2) & 0x1C | ((src[1] >> 6) & 0x3)];  /* last 3 bits + next 2 bits */
    dest[2] = pear_ddns_base32_enc_table[(src[1] >> 1) & 0x1F];                          /* next 5 bits (tail has 1 bit) */
    dest[3] = pear_ddns_base32_enc_table[(src[1] << 4) & 0x10 | ((src[2] >> 4) & 0xF)];  /* last bit + next four bits */
    dest[4] = pear_ddns_base32_enc_table[(src[2] << 1) & 0x1E | ((src[3] >> 7) & 0x1)];
    dest[5] = pear_ddns_base32_enc_table[(src[3] >> 2) & 0x1F];
    dest[6] = pear_ddns_base32_enc_table[(src[3] << 3) & 0x18 | ((src[4] >> 5) & 0x7)];
    dest[7] = pear_ddns_base32_enc_table[(src[4] & 0x1F)];
    return 0;
}

static int pear_ddns_base32_encode_tail(char *dest, const void *_src, size_t len)
{
    int k;
    const unsigned char *src = _src;

    if(len < 1 || len > 5)
        return -1;

    for(k = 0 ; k < 8; k++ )
        dest[k] = pear_ddns_base32_padding;

    switch(len)
    {
    case 5:
        dest[7] = pear_ddns_base32_enc_table[(src[4]) & 0x1F];
    case 4:
        dest[6] = pear_ddns_base32_enc_table[(src[3] << 3) & 0x18 | (dest[7] != pear_ddns_base32_padding ? (src[4] >> 5) & 0x7 : 0)];
        dest[5] = pear_ddns_base32_enc_table[(src[3] >> 2) & 0x1F];
    case 3:
        dest[4] = pear_ddns_base32_enc_table[(src[2] << 1) & 0x1E | (dest[5] != pear_ddns_base32_padding ? (src[3] >> 7) & 0x1 : 0)];
    case 2:
        dest[3] = pear_ddns_base32_enc_table[(src[1] << 4) & 0x10 | (dest[4] != pear_ddns_base32_padding ? (src[2] >> 4) & 0xF : 0)];
        dest[2] = pear_ddns_base32_enc_table[(src[1] >> 1) & 0x1F];
    case 1:
        dest[1] = pear_ddns_base32_enc_table[(src[0] << 2) & 0x1C | (dest[2] != pear_ddns_base32_padding ? (src[1] >> 6 & 0x3) : 0)];
        dest[0] = pear_ddns_base32_enc_table[(src[0] >> 3) & 0x1F];
        break;
    default:
        printf("Internal error, length passed to pear_ddns_base32_encode_tail MUST BE between 1 and 5\n");
        break;
    }

    return 0;
}

size_t pear_ddns_base32_enc(char *dest, const void *_src, size_t ssize)
{
    int dk = 0, sk = 0, k;
    const unsigned char *src = _src;


    if ( ssize > 0 && ssize <= 5 )
    {
        pear_ddns_base32_encode_tail(dest, src, ssize);
        dk = 8;
        goto out;
    }

    ssize -= 5;

    for(dk = 0, sk = 0; sk <= ssize; sk += 5, dk += 8)
    {
        pear_ddns_base32_encode_block(&dest[dk], &src[sk]);
    }
    ssize += 5;

    if (ssize > sk)
    {
        pear_ddns_base32_encode_tail(&dest[dk], &src[sk], ssize - sk);
        dk += 8;
    }

out:
    dest[dk] = 0;

    while(dest[dk - 1] == 0 )
        dk--;

    while(dest[dk - 1] == pear_ddns_base32_padding)
    {
        dest[dk - 1] = 0;
        dk--;
    }
    return dk;
}

static int pear_ddns_base32_decode_block(unsigned char *dest, const void *_src)
{
    uint8_t idx[8] = {};
    const unsigned char * src = _src;
    int k;
    int end = 0;
    for (k = 0; k < 8; k++)
    {
        if (src[k] == 0  || src[k] == pear_ddns_base32_padding)
        {
            end = 1;
            break;
        }
        idx[k] = pear_ddns_base32_dec_table[src[k]];
        //printf("indexof: %u; decoding table: %u\n", index_of(src[k]), pear_ddns_base32_dec_table[src[k]]);
    }

    dest[0] = (uint8_t)(((idx[0] << 3) & 0xF8) | ((idx[1] >> 2) & 0x7));
    dest[1] = (uint8_t)(((idx[1] << 6) & 0xC0) | ((idx[2] << 1) & 0x3E) | ((idx[3] >> 4) & 0x1));
    dest[2] = (uint8_t)(((idx[3] << 4) & 0xF0) | ((idx[4] >> 1) & 0xF));
    dest[3] = (uint8_t)(((idx[4] << 7) & 0x80) | ((idx[5] << 2) & 0x7C) | ((idx[6] >> 3) & 0x3));
    dest[4] = (uint8_t)(((idx[6] << 5) & 0xE0) | ((idx[7] & 0x1F)));

    if (end)
    {
        switch(k)
        {
        case 0:
            end = ((idx[0] << 3) & 0xF8) ? 1 : 0;
            break;
        case 1:
            end = ((idx[1] << 6) & 0xC0) ? 2 : 1;
            break;
        case 2:
            end = 2;
            break;
        case 3:
            end = ((idx[3] << 4) & 0xF0 ) ? 3 : 2;
            break;
        case 4:
            end = ((idx[4] << 7) & 0x80 ) ? 4 : 3;
            break;
        case 5:
            end = 4;
            break;
        case 6:
            end = ((idx[6] << 5) & 0xE0 ) ? 5 : 4;
            break;
        case 7:
            end = 5;
            break;
        }
    }
    return end;
}


size_t pear_ddns_base32_dec(void *dest, size_t buf_size, const char * src)
{
    int sk, dk;
    size_t src_len;
    src_len = strlen(src);

    if(buf_size <= 5)
    {
        char small_buf[5]; /* small buffer for 1-5 bytes stuff */
        pear_ddns_base32_decode_block(small_buf, src);
        memcpy(dest, small_buf, buf_size);
        return 0;
    }

    for(sk = 0, dk = 0; sk < src_len; sk += 8, dk += 5)
    {
        int end = pear_ddns_base32_decode_block((unsigned char *)dest + dk, &src[sk]);
        if ( end )
        {
            dk += end - 1;
            break;
        }
    }

    return dk;
}

size_t pear_ddns_base32_encsize(size_t count)
{
    size_t size = -1;

    size = count << 3; /* multiply by 8 */

    if ( size % 5 == 0 )
        return size / 5;
    else
        return 1 + size / 5;
}

size_t pear_ddns_base32_decsize(size_t count)
{
    size_t size = -1;
    size = count * 5;
    return (size >> 3) + (size % 8) ? 1 : 0;
}
