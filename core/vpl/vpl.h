// This is vxl/vpl/vpl.h
#ifndef vpl_h_
#define vpl_h_
//:
// \file

#include <vcl_cstddef.h>

//: Get the working directory
//  See the getcwd(3) manpage for details.
char           *vpl_getcwd( char *buf, vcl_size_t buf_size );

//: Create a new directory \a dir with permissions \a mode.
int             vpl_mkdir( const char *dir, unsigned short mode );

//: Remove the directory \a dir.
int             vpl_rmdir( const char *dir );

//: Change the working directory to \a dir.
int             vpl_chdir( const char *dir );

//: Remove the file \a file.
int             vpl_unlink( const char *file );

//: Sleep for t seconds.
unsigned int    vpl_sleep( unsigned int t );

//: Sleep for t microseconds.
int             vpl_usleep( unsigned int t );

//: Set environment variable
// \param envvar should contain the environment variable name and value
// separated by an equals sign, e.g. "VARNAME=VALUE"
int             vpl_putenv ( const char * envvar );

#endif // vpl_h_
