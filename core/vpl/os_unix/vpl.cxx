// Include system headers for UNIX-like operating system :
#define _XOPEN_SOURCE 1 // necessary on alpha and on SGI since otherwise
#define _XOPEN_SOURCE_EXTENDED 1 // usleep is not declared
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vxl_config.h> // for VXL_UNISTD_*
#include <vcl_cstdlib.h>
#include <vcl_cstring.h>

char *
vpl_getcwd( char *buf, vcl_size_t buf_size )
{
  return getcwd( buf, buf_size );
}

int
vpl_mkdir( const char *dir, unsigned short mode )
{
  return mkdir( dir, (mode_t)mode );
}

int
vpl_rmdir( const char *dir )
{
  return rmdir( dir );
}

int
vpl_chdir( const char *dir )
{
  return chdir( dir );
}

int
vpl_unlink( const char *file )
{
  return unlink( file );
}

unsigned int
vpl_sleep( unsigned int t )
{
  return sleep( t );
}

int
vpl_usleep( unsigned int t )
{
  // some implementations require argument to usleep < 1000000 :
  if (t > 1000000) sleep( t/1000000 ); t %= 1000000;
#if VXL_UNISTD_HAS_USLEEP
 #if VXL_UNISTD_USLEEP_IS_VOID
  usleep( t ); return 0;
 #else
  return usleep( t );
 #endif
#else
  return 0;
#endif
}

int vpl_putenv ( const char * envvar )
{
  char * storage_space = vcl_strdup(envvar); // This causes a memory leak
                                             // but this can't be helped
  return vcl_putenv(storage_space);
}
