/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_stream_FILE_ptr.h"

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cstdio.h>     // tmpnam()
#include <vcl/vcl_cassert.h>

#include <vpl/vpl_unistd.h>     // vpl_unlink()

vil_stream_FILE_ptr::vil_stream_FILE_ptr(vil_stream *vs) : fp(0) {
  assert(vs);

  // get name of temp file :
  filename = tmpnam(0);

  // open temp file :  
  fp = fopen(filename.c_str(), "w+b");
  assert(fp);

  unsigned n_read = 0;
  unsigned n_writ = 0;

  const unsigned N=65536;
  char buffer[N];
  vs->seek(0);
  while (true) {
    int r = vs->read(buffer, N);
    if (r<0) {
      cerr << __FILE__ ":" << __LINE__ << " : urgh!" << endl;
      break;
    }
    if (r==0)
      break; // assume eof
    n_read += r;
    
    int w = fwrite(buffer, 1, r, fp); 
    //perror(0);
    if (w<0) {
      cerr << __FILE__ ":" << __LINE__ << " : urgh!" << endl;
      break;
    }
    n_writ += w;
    
    if (w != r) 
      cerr << __FILE__ ":" << __LINE__ << " : urgh!  r=" << r << " w=" << w << endl;

    if (r < N)
      break;
  }
  // we don't need the vil_stream any more :
  //delete vs;

  // debug
  //cerr << __FILE__ " : read  " << n_read << " bytes" << endl;
  //cerr << __FILE__ " : wrote " << n_writ << " bytes" << endl;

  // rewind(fp)
  fseek(fp, 0, SEEK_SET);
}

vil_stream_FILE_ptr::~vil_stream_FILE_ptr() {
  // close temp file :
  if (fp)
    fclose(fp);
  fp = 0;

  // unlink temp file :
  vpl_unlink(filename.c_str());
} 

