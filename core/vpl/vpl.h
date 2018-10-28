// This is core/vpl/vpl.h
#ifndef vpl_h_
#define vpl_h_
//:
// \file
// \brief Access to certain POSIX functions.

#include <cstddef>
#include "vpl/vpl_export.h"

//: Get the pathname of the current working directory.
//
//  If \a buf is not NULL, then \a size must be at least 1 greater
//  than the length of the pathname to be returned and the pathname
//  will be stored in \a buf.  If \a buf is NULL then the pathname
//  will be returned and the returned pointer may be passed to free()
//  by the caller.
//
//  See also: getcwd(3) manpage.

extern VPL_EXPORT char         *vpl_getcwd( char *buf, std::size_t buf_size );

//: Create a new directory \a dir with permissions \a mode.
extern VPL_EXPORT int          vpl_mkdir( const char *dir, unsigned short mode );

//: Remove the directory \a dir.
extern VPL_EXPORT int          vpl_rmdir( const char *dir );

//: Change the working directory to \a dir.
extern VPL_EXPORT int          vpl_chdir( const char *dir );

//: Remove the file \a file.
extern VPL_EXPORT int          vpl_unlink( const char *file );

//: Sleep for \a t seconds.
extern VPL_EXPORT unsigned int vpl_sleep( unsigned int t );

//: Sleep for \a t microseconds.
extern VPL_EXPORT int          vpl_usleep( unsigned int t );

//: Set environment variable
// \param envvar should contain the environment variable name and value
// separated by an equals sign, e.g. "VARNAME=VALUE"
extern VPL_EXPORT int          vpl_putenv ( const char * envvar );

//: Get the process identifier.
extern VPL_EXPORT unsigned     vpl_getpid( );

//: Get the current machine's hostname.
extern VPL_EXPORT int          vpl_gethostname(char *name, std::size_t len );

#endif // vpl_h_
