// Include system headers for UNIX-like operating system :
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

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
  #if VXL_UNISTD_USLEEP_IS_VOID
    usleep( t );
    return 0;
  #else
    return usleep( t );
  #endif
}
