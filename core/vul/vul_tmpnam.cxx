#ifdef __GNUC__
#pragma interface
#endif

#include <vul/vul_tmpnam.h>

#include <vcl_string.h>

#include <vcl_ctime.h>
#include <vcl_cstdlib.h>

#include <fcntl.h>
#include <unistd.h>

// Helper functions
namespace {

  // The filename is okay if it doesn't exist and can be opened for
  // writing.
  bool is_okay( const vcl_string& name )
  {
    int fd = open( name.c_str(), O_CREAT|O_EXCL );
    if( fd == -1 ) {
      return false;
    } else {
      unlink( name.c_str() );
      close( fd );
      return true;
    }
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
