#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glib.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <pbc.h>

#include "common.h"

void
init_cipher( element_t k,unsigned char* key, unsigned char* iv )
{
  int key_len;
  unsigned char* tmp;

  key_len = element_length_in_bytes(k) < 32 ? 32 : element_length_in_bytes(k);
  tmp = (unsigned char*) malloc(key_len);
  element_to_bytes(tmp, k);
  memcpy(key, tmp, 16);
  memcpy(iv, tmp+16, 16);
}


FILE*
fopen_read_or_die( char* file )
{
	FILE* f;

	if( !(f = fopen(file, "r")) )
		die("can't read file: %s\n", file);

	return f;
}

FILE*
fopen_write_or_die( char* file )
{
	FILE* f;

	if( !(f = fopen(file, "w")) )
		die("can't write file: %s\n", file);

	return f;
}

GByteArray*
suck_file( char* file )
{
	FILE* f;
	GByteArray* a;
	struct stat s;
	a = g_byte_array_new();
	stat(file, &s);
	g_byte_array_set_size(a, s.st_size);

	f = fopen_read_or_die(file);
	fread(a->data, 1, s.st_size, f);
	fclose(f);

	return a;
}

char*
suck_file_str( char* file )
{
	GByteArray* a;
	char* s;
	unsigned char zero;

	a = suck_file(file);
	zero = 0;
	g_byte_array_append(a, &zero, 1);
	s = (char*) a->data;
	g_byte_array_free(a, 0);

	return s;
}

char*
suck_stdin()
{
	GString* s;
	char* r;
	int c;

	s = g_string_new("");
	while( (c = fgetc(stdin)) != EOF )
		g_string_append_c(s, c);

	r = s->str;
	g_string_free(s, 0);

	return r;
}

void
spit_file( char* file, GByteArray* b, int free )
{
	FILE* f;

	f = fopen_write_or_die(file);
	fwrite(b->data, 1, b->len, f);
	fclose(f);

	if( free )
		g_byte_array_free(b, 1);
}

void read_cpabe_file( char* file,    GByteArray** cph_buf, int* file_len )
{
	FILE* f;
	int i;
	int len;

	*cph_buf = g_byte_array_new();

	f = fopen_read_or_die(file);

	/* read real file len as 32-bit big endian int */
	*file_len = 0;
	for( i = 3; i >= 0; i-- )
		*file_len |= fgetc(f)<<(i*8);
	//printf("filelength:%d", *file_len);
	/* read aes buf */
	/*len = 0;
	for( i = 3; i >= 0; i-- )
		len |= fgetc(f)<<(i*8);
	g_byte_array_set_size(*aes_buf, len);
	fread((*aes_buf)->data, 1, len, f);
        */
	/* read cph buf */
	len = 0;
	for( i = 3; i >= 0; i-- )
		len |= fgetc(f)<<(i*8);
	g_byte_array_set_size(*cph_buf, len);
	fread((*cph_buf)->data, 1, len, f);
	
	fclose(f);
}

void
write_cpabe_file( char* file, GByteArray* cph_buf, int file_len )
{
	FILE* f;
	int i;

	f = fopen_write_or_die(file);
	//printf("filelength:%d", file_len);
	/* write real file len as 32-bit big endian int */
	for( i = 3; i >= 0; i-- )
		fputc((file_len & 0xff<<(i*8))>>(i*8), f);

	/* write aes_buf */
	/*for( i = 3; i >= 0; i-- )
		fputc((aes_buf->len & 0xff<<(i*8))>>(i*8), f);
	fwrite(aes_buf->data, 1, aes_buf->len, f);
        */
	/* write cph_buf */
	for( i = 3; i >= 0; i-- )
		fputc((cph_buf->len & 0xff<<(i*8))>>(i*8), f);
	fwrite(cph_buf->data, 1, cph_buf->len, f);

	fclose(f);
}

void
die(char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(1);
}
