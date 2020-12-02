/* ------------------------------------------- * 
 * zhttp.h
 * ---------
 * A C-based HTTP parser, request and response builder 
 *
 * Usage
 * -----
 * Soon to come...
 *
 * LICENSE
 * -------
 * Copyright 2020 Tubular Modular Inc. dba Collins Design
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 *
 * CHANGELOG 
 * ---------
 * 12-01-20 - Fixed a seek bug in memstr, adding tests
 *          - Added memblk functions. 
 * 12-02-20 - Fixed an off-by-one bug in memblk* functions.
 * 
 * ------------------------------------------- */
#include "../vendor/zwalker.h"
#include "../vendor/zhasher.h"
#include "util.h"
#ifndef HTTP_H

#define HTTP_H

#define http_free_request( entity ) \
	http_free_body( entity )

#define http_free_response( entity ) \
	http_free_body( entity )

#define http_set_status(ENTITY,VAL) \
	http_set_int( &(ENTITY)->status, VAL )

#define http_set_ctype(ENTITY,VAL) \
	http_set_char( &(ENTITY)->ctype, VAL )
	
#define http_set_method(ENTITY,VAL) \
	http_set_char( &(ENTITY)->method, VAL )
	
#define http_set_protocol(ENTITY,VAL) \
	http_set_char( &(ENTITY)->protocol, VAL )
	
#define http_set_path(ENTITY,VAL) \
	http_set_char( &(ENTITY)->path, VAL )
	
#define http_set_host(ENTITY,VAL) \
	http_set_char( &(ENTITY)->host, VAL )
	
#define http_set_body(ENTITY,KEY,VAL,VLEN) \
	http_set_record( ENTITY, &(ENTITY)->body, 1, KEY, VAL, VLEN )
	
#define http_set_content(ENTITY,VAL,VLEN) \
	http_set_record( ENTITY, &(ENTITY)->body, 1, ".", VAL, VLEN )

#define http_set_content_text(ENTITY,VAL) \
	http_set_record( ENTITY, &(ENTITY)->body, 1, ".", (uint8_t *)VAL, strlen(VAL) )

#define http_set_textbody(ENTITY,KEY,VAL) \
	http_set_record( ENTITY, &(ENTITY)->body, 1, KEY, (uint8_t *)VAL, strlen((char *)VAL) )
	
#define http_set_header(ENTITY,KEY,VAL) \
	http_set_record( ENTITY, &(ENTITY)->headers, 0, KEY, (uint8_t *)VAL, strlen(VAL) )

extern const char http_200_fixed[];

typedef enum {
	HTTP_100 = 100,
	HTTP_101 = 101,
	HTTP_200 = 200,
	HTTP_201 = 201,
	HTTP_202 = 202,
	HTTP_204 = 204,
	HTTP_206 = 206,
	HTTP_300 = 300,
	HTTP_301 = 301,
	HTTP_302 = 302,
	HTTP_303 = 303,
	HTTP_304 = 304,
	HTTP_305 = 305,
	HTTP_307 = 307,
	HTTP_400 = 400,
	HTTP_401 = 401,
	HTTP_403 = 403,
	HTTP_404 = 404,
	HTTP_405 = 405,
	HTTP_406 = 406,
	HTTP_407 = 407,
	HTTP_408 = 408,
	HTTP_409 = 409,
	HTTP_410 = 410,
	HTTP_411 = 411,
	HTTP_412 = 412,
	HTTP_413 = 413,
	HTTP_414 = 414,
	HTTP_415 = 415,
	HTTP_416 = 416,
	HTTP_417 = 417,
	HTTP_418 = 418,
	HTTP_500 = 500,
	HTTP_501 = 501,
	HTTP_502 = 502,
	HTTP_503 = 503,
	HTTP_504 = 504
} HTTP_Status;


typedef enum {
	ZHTTP_NONFATAL = 0x0000
, ZHTTP_FATAL = 0xff00
} HTTP_ErrorType;


typedef enum {
	ZHTTP_NONE = 0
,	ZHTTP_INCOMPLETE_METHOD
, ZHTTP_INCOMPLETE_PATH
, ZHTTP_INCOMPLETE_PROTOCOL
, ZHTTP_INCOMPLETE_HEADER
, ZHTTP_UNSUPPORTED_METHOD
, ZHTTP_UNSUPPORTED_PROTOCOL
, ZHTTP_MALFORMED_FIRSTLINE
, ZHTTP_INCOMPLETE_FIRSTLINE
, ZHTTP_OUT_OF_MEMORY
} HTTP_Error;


struct HTTPRecord {
	const char *field; 
	const char *metadata; 
	uint8_t *value; 
	int size; 
};

struct HTTPBody {
	char *path;
	char *ctype; //content type ptr
	char *host;
#if 1
	char *method;
	char *protocol;
	char *boundary;
#else
	char method[ 8 ];
	char protocol[ 8 ];
	char boundary[ 64 ];
#endif
 	uint8_t *msg;
	int clen;  //content length
	int mlen;  //message length (length of the entire received message)
	int	hlen;  //header length
	int status; //what was this?
#if 0
	//This may make things more efficient
	struct HTTPRecord **values;
#else
	struct HTTPRecord **headers;
	struct HTTPRecord **url;
	struct HTTPRecord **body;
	int error;
	int rstatus;  //HEADER_PARSED, URL_PARSED, ...
#endif
};

unsigned char *httpvtrim (uint8_t *, int , int *) ;
unsigned char *httptrim (uint8_t *, const char *, int , int *) ;
void print_httprecords ( struct HTTPRecord ** ) ;
void print_httpbody ( struct HTTPBody * ) ;
void http_free_body( struct HTTPBody * );
struct HTTPBody * http_finalize_response (struct HTTPBody *, char *, int );
struct HTTPBody * http_finalize_request (struct HTTPBody *, char *, int );
struct HTTPBody * http_parse_request (struct HTTPBody *, char *, int );
struct HTTPBody * http_parse_response (struct HTTPBody *, char *, int );
int http_set_int( int *, int );
char *http_set_char( char **, const char * );
void *http_set_record( struct HTTPBody *, struct HTTPRecord ***, int, const char *, uint8_t *, int );
int http_set_error ( struct HTTPBody *entity, int status, char *message );

#endif
