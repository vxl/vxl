// This is core/vul/vul_temp_filename.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vul_temp_filename.h"
#include <vcl_string.h>
#include <vcl_ctime.h>
#include <vcl_cstdlib.h> // for rand/srand

#ifdef _MSC_VER
  #include <Windows.h>
#else
#if defined(unix) || defined(__unix)
  // Helper functions for Unix

  #include <stdio.h>  // for P_tmpdir
  #include <unistd.h> // for unlink
  #include <fcntl.h>  // for O_CREATE,...

  namespace {
    // The filename is okay if it doesn't exist and can be opened for
    // writing.
    bool is_okay( const vcl_string& name )
    {
      bool okay = true;
      int fd = open( name.c_str(), O_CREAT|O_EXCL );
      if ( fd == -1 ) {
        okay = false;
      } else {
        unlink( name.c_str() );
        close( fd );
      }
      return okay;
    }


    // Initialise the random number seed with the time.  Maybe need to
    // include things like (Unix) process id, but I don't think the
    // randomness is that crucial.
    int init_randomizer()
    {
      vcl_srand( vcl_time( 0 ) );
      return 0;
    }
    static int random_seed_trigger = init_randomizer();

    char random_letter()
    {
      // Make sure the random character is a letter.
      int r = vcl_rand() % (26+26); // 26 uppercase and 26 lowercase letters
      return (r<26) ? char('A'+r) : char('a'+r-26);
    }

    char random_char()
    {
      // Make sure the random character is a letter or number.
      int r = vcl_rand() % (26+26+10); // 2x26 letters, 10 digits
      return (r<26) ? char('A'+r) : (r<52) ? char('a'+r-26) : char('0'+r-52);
    }
  }
#else
# warning "This is neither unix nor MS-windows - please add specifics to " __FILE__
#endif
#endif

vcl_string
vul_temp_filename( )
{
#ifdef _MSC_VER
  char path[ _MAX_PATH ];
  char* file;
  if ( GetTempPath( _MAX_PATH, path ) == 0 )
    return "";
  // Can't use GetTempFileName, because the function actually creates the
  // temporary file! This would mean that every call to this function creates
  // yet another file that will lie around if the caller doesn't use the generated
  // filename. And I don't trust the implementation enough to just unlink the file
  // before returning.
  file = _tempnam( path, "" );
  if ( file == 0 )
    return "";
  return file;
#else
#if defined(unix) || defined(__unix)
  // Don't use tmpnam, since it causes linker warnings (and sometimes
  // linker errors). Instead reimplement. Sigh.
  const unsigned int num_char_in_filename = 7+1; // should always be at least 1
  vcl_string filename;
  vcl_string tempdir;
  unsigned int count = 0;
  bool okay = false;

  if ( vcl_getenv( "TMP" ) ) {
    tempdir = vcl_getenv( "TMP" );
  } else {
    tempdir = P_tmpdir; // defined in stdio.h
  }
  char lastchar = ( tempdir.size() > 0 ) ? tempdir[tempdir.size()-1] : ' ';
  if (lastchar != '/' && lastchar != '\\')
    tempdir += "/";

  while ( !okay && count < 10 ) {
    char buf[ num_char_in_filename+1 ];
    buf[0] = random_letter(); // make sure first char is a letter
    for ( unsigned int i=1; i < num_char_in_filename; ++i )
      buf[i] = random_char();
    buf[num_char_in_filename] = '\0';
    filename = tempdir + buf;
    ++count;
    okay = is_okay( filename );
  };

  if ( okay )
    return filename;
  else
    return "";
#else
# warning "This is neither unix nor MS-windows - please add specifics to " __FILE__
#endif
#endif
}
