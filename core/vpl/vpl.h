// This is core/vpl/vpl.h
#ifndef vpl_h_
#define vpl_h_
//:
// \file
// \brief Access to certain POSIX functions.

#include <vcl_cstddef.h>

//: Get the pathname of the current working directory.
//
//  If \a buf is not NULL, then \a size must be at least 1 greater
//  than the length of the pathname to be returned and the pathname
//  will be stored in \a buf.  If \a buf is NULL then the pathname
//  will be returned and the returned pointer may be passed to free()
//  by the caller.
//
//  See also: getcwd(3) manpage.

char           *vpl_getcwd( char *buf, vcl_size_t buf_size );

//: Create a new directory \a dir with permissions \a mode.
int             vpl_mkdir( const char *dir, unsigned short mode );

//: Remove the directory \a dir.
int             vpl_rmdir( const char *dir );

//: Change the working directory to \a dir.
int             vpl_chdir( const char *dir );

//: Remove the file \a file.
int             vpl_unlink( const char *file );

//: Sleep for \a t seconds.
unsigned int    vpl_sleep( unsigned int t );

//: Sleep for \a t microseconds.
int             vpl_usleep( unsigned int t );

//: Set environment variable
// \param envvar should contain the environment variable name and value
// separated by an equals sign, e.g. "VARNAME=VALUE"
int             vpl_putenv ( const char * envvar );

#endif // vpl_h_
