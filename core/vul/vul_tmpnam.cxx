#ifdef __GNUC__
#pragma interface
#endif

#include <vul/vul_tmpnam.h>

#include <vcl_string.h>

#include <vcl_ctime.h>
#include <vcl_cstdlib.h> // for rand/srand

#ifndef _MSC_VER
  #include <unistd.h> // for unlink
#else
  #include <io.h> // for _open and _close
  #include <sys/stat.h> // for _S_IREAD and _S_IWRITE
#endif
#include <fcntl.h> // for O_CREATE,...

// Helper functions
namespace {

  // The filename is okay if it doesn't exist and can be opened for
  // writing.
  bool is_okay( const vcl_string& name )
  {
    bool okay = true;
#ifndef _MSC_VER
    int fd = open( name.c_str(), O_CREAT|O_EXCL );
#else
    int fd = _open( name.c_str(), _O_CREAT|_O_EXCL|_O_TEMPORARY, _S_IREAD | _S_IWRITE );
#endif
    if( fd == -1 ) {
      okay = false;
    } else {
#ifndef _MSC_VER
      unlink( name.c_str() );
      close( fd );
#else
      if( _close( fd ) == -1 ) {
	perror("vul_tmpnam: closing temporary file");
	okay = false;
      }
      // no need to unlink under Windows, since _O_TEMPORARY will ensure the file is
      // deleted when the descriptor is closed.
#endif
    }
    return okay;
  }


  // Initialise the random number seed with the time.  Maybe need to
  // include things like (Unix) process id, but I don't think the
  // randomness is that crucial.
  int init_randomizer()
  {
    srand( time( 0 ) );
    return 0;
  }
  static int random_seed_trigger = init_randomizer();

  char random_char()
  {
    // Make sure the random character is not a "no-no" character.
    char c;
    do {
      c = char( rand() % (126-33) + 33 );
    } while( c=='/' || c=='\\' );
    return c;
  }
}


bool
tmpnam( vcl_string& filename )
{
  vcl_string temp;
  unsigned int count = 0;
  bool okay = false;

  while( !okay && count < 10 ) {
    temp = filename;
    vcl_string::size_type p = temp.length()-1;
    while( p >= 0 && temp[p] == 'X' ) {
      temp[p] = random_char();
      --p;
    }
    ++count;
    okay = is_okay( temp );
  };

  if( okay )
    filename = temp;

  return okay;
}
