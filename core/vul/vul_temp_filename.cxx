#ifdef __GNUC__
#pragma interface
#endif

#include <vul/vul_temp_filename.h>
#include <vcl_string.h>
#include <vcl_ctime.h>
#include <vcl_cstdlib.h> // for rand/srand

#ifndef _MSC_VER
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
      if( fd == -1 ) {
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
      vcl_srand( time( 0 ) );
      return 0;
    }
    static int random_seed_trigger = init_randomizer();

    char random_char()
    {
      // Make sure the random character is not a "no-no" character.
      char c;
      do {
        c = char( vcl_rand() % (126-33) + 33 );
      } while( c=='/' || c=='\\' );
      return c;
    }
  }
#else
  #include <Windows.h>
  #include <stdio.h>
#endif

vcl_string
vul_temp_filename( )
{
#ifdef _MSC_VER
  char path[ _MAX_PATH ];
  char* file;
  if( GetTempPath( _MAX_PATH, path ) == 0 )
	return "";
  // Can't use GetTempFileName, because the function actually creates the
  // temporary file! This would mean that every call to this function creates
  // yet another file that will lie around if the caller doesn't use the generated
  // filename. And I don't trust the implementation enough to just unlink the file
  // before returning.
  file = _tempnam( path, "" );
  if( file == 0 )
	return "";
  return file;
#else
  // Don't use tmpnam, since it causes linker warnings (and sometimes
  // linker errors). Instead reimplement. Sigh.
  const unsigned int num_char_in_filename = 8;
  vcl_string filename;
  vcl_string tempdir;
  unsigned int count = 0;
  bool okay = false;

  if( vcl_getenv( "TMP" ) ) {
    tempdir = vcl_getenv( "TMP" );
  } else {
    tempdir = P_tmpdir; // defined in stdio.h
  }
  char lastchar = ( tempdir.size() > 0 ) ? tempdir[tempdir.size()-1] : ' ';
  if (lastchar != '/' && lastchar != '\\')
    tempdir += "/";

  while( !okay && count < 10 ) {
    char buf[ num_char_in_filename+1 ];
    for( unsigned int i=0; i < num_char_in_filename; ++i )
      buf[i] = random_char();
    buf[num_char_in_filename] = '\0';
    filename = tempdir + buf;
    ++count;
    okay = is_okay( filename );
  };

  if( okay )
    return filename;
  else
    return "";
#endif
}
