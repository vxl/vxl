// This is core/vpl/os_win32/vpl.cxx
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <io.h>
#include <process.h>
#include <windows.h>

char *
vpl_getcwd( char *buf, std::size_t buf_size )
{
  return _getcwd( buf, (int)buf_size );
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
  return _rmdir( dir );
}

int
vpl_chdir( const char *dir )
{
  return _chdir( dir );
}

int
vpl_unlink( const char *file )
{
  return _unlink( file );
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

unsigned
vpl_getpid( )
{
  return _getpid();
}

int vpl_putenv ( const char * envvar )
{
  return _putenv(envvar);
}


int vpl_gethostname(char *name, std::size_t len)
{
#if defined(_MSC_VER)
  static bool wsa_initialised = false;

  if (!wsa_initialised)
  {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
  }
#endif
  return gethostname(name, len);
}
