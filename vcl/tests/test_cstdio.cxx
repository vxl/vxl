#include <vcl_cstdio.h>

int close_stdin();

int test_cstdio_main( const char* filename )
{
  vcl_printf( "Hello. %d %f %03x.\n", 1, 2.0f, 3 );

  bool fail = false; // global status of the test suite

  int rc; // return code

  // Close the standard input. All reads from
  // stdin should fail after this.
  rc = close_stdin();
  if( rc != 0 ) {
    vcl_printf( "FAIL: couldn't close standard input\n" );
    fail = true;
  }


  rc = vcl_getchar();
  if( rc != EOF ) {
    vcl_printf( "FAIL: get_char read a value from a closed stream\n" );
    fail = true;
  }

  FILE* fh = vcl_fopen( filename, "rw" );
  if( !fh ) {
    vcl_printf( "FAIL: couldn't open %s\n", filename );
    vcl_printf( "      (skipping file tests)\n" );
    fail = true;
  } else {
    rc = vcl_getc( fh );
    if( rc != 't' ) {
      vcl_printf( "FAIL: first character read was not 't'\n" );
      fail = true;
    }

    rc = vcl_ungetc( 'x', fh );
    if( rc != 'x' ) {
      vcl_printf( "FAIL: ungetc failed\n" );
      fail = true;
    } else {
      rc = vcl_getc( fh );
      if( rc != 'x' ) {
        vcl_printf( "FAIL: getc returned %d, and not %d ('x') as expected\n", rc, 'x' );
        fail = true;
      }
    }

    rc = vcl_fclose( fh );
    if( rc != 0 ) {
      vcl_printf( "FAIL: failed to close file\n" );
      fail = true;
    }
  }

  return fail ? 1 : 0;
}

// Implement this below the main tests so that the includes don't
// accidently provide something that shouldn't be provided.

// Closing the input stream using a file descriptor is not portable,
// since there *may* be some systems that don't use file
// descriptors. However, I think it should work on all the common
// platforms. If someone has a conflicting platform, perhaps they
// could conditionally define the appropriate code for their platform.

// Return 0 on success, non-zero on error.

#ifdef VCL_VC
# include <io.h>
int close_stdin()
{
  return _close( 0 );
}
#else
# include <unistd.h>
int close_stdin()
{
  return close( 0 );
}
#endif

