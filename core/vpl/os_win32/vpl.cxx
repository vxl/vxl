// This is core/vpl/os_win32/vpl.cxx
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <io.h>
#include <windows.h>

char *
vpl_getcwd( char *buf, vcl_size_t buf_size )
{
  return _getcwd( buf, buf_size );
}

int
vpl_mkdir( const char *dir, unsigned short /*mode*/ )
{
  _mkdir( dir );
  return 0;
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
#if defined(VCL_BORLAND)
  return unlink( file );
#else
  return _unlink( file );
#endif
}

unsigned int
vpl_sleep( unsigned int t )
{
  Sleep( long(t) * 1000 );
  return 0;
}

int
vpl_usleep( unsigned int t )
{
  Sleep( t / 1000 );
  return 0;
}

int vpl_putenv ( const char * envvar )
{
#if defined(VCL_BORLAND)
  return putenv(envvar);
#else
  return _putenv(envvar);
#endif
}
