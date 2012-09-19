/* MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */
#include "global.h"

/* MD5 context. */
typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void MD5Final(unsigned char [16], MD5_CTX *);

#include <string>
#pragma warning(disable:4996)
inline std::string   MDString   (const char* string)
{
	MD5_CTX context;
	unsigned char digest[16];
	char output1[33] = {0};
	char output[33]  ={0};   
	unsigned   int   len = strlen(string);   
	int   i;   
	MD5Init(&context);   
	MD5Update(&context, (unsigned   char*)string,   len);   
	MD5Final(digest,   &context);   

	for (i=0; i<16; i++)   
	{   
		sprintf(&(output1[2*i]), "%02x",(unsigned   char)digest[i]); 
	}
	for(i=0;i<32;i++)   
		output[i]=output1[i];   
	return   std::string(output);
}