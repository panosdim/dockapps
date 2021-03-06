///////////////////////////////////////////////////////////////////////////////
// common.h
// common defines and typedefs, part of wmail
//
// Copyright 2000~2002, Sven Geisenhainer <sveng@informatik.uni-jena.de>.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions, and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions, and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef _COMMON_H_fdda99de15ce3f21ce7faf607a5b4985_
#define _COMMON_H_fdda99de15ce3f21ce7faf607a5b4985_


///////////////////////////////////////////////////////////////////////////////
// defines

// X display to open
#define WMAIL_DISPLAY        ""

// ticker scroll frame rate per seconds
#define WMAIL_FPS            60

// default mail check interval in seconds
#define WMAIL_CHECK_INTERVAL 1

// default client-button command
#define WMAIL_CLIENT_CMD     "xterm -e mail"

// filename of the checksum-file
#define WMAIL_CHECKSUM_FILE  ".wmail-cksums"

// filename of the config-file
#define WMAIL_RC_FILE        ".wmailrc"

#define WMAIL_NAME           "wmail"
#define WMAIL_VERSION        "wmail v2.0 (C) 2000~2002 Sven Geisenhainer <sveng@informatik.uni-jena.de>"

#define WMAIL_READSTATUS     "O"

///////////////////////////////////////////////////////////////////////////////
// typedefs

#if !defined(__cplusplus) && !defined(c_plusplus)
typedef enum {
    false = 0,
    true = 1
} bool;
#endif


///////////////////////////////////////////////////////////////////////////////
// prototypes

void ABORT( const char *fmt, ... );
void WARNING( const char *fmt, ... );
char *MakePathName( const char *dir, const char *file );

#if defined(DEBUG) || defined(DEBUG2) || defined(_DEBUG)

void TRACE( const char *fmt, ... );
#define ASSERT( EXPR ) ((void)(EXPR ? 0 : ABORT( "%s(%i): Assertion failed: \"%s\"\n", __FILE__, __LINE__, #EXPR )))
#define VERIFY( EXPR ) ASSERT( EXPR )

#else

#define TRACE 0 && // hopefully rejected by the optimizing compiler (as gcc does...)
#define ASSERT(EXPR) ((void)0)
#define VERIFY(EXPR) (EXPR)

#endif

#endif
