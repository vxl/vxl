/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_skip_image.h"
#include <vcl/vcl_climits.h> // for CHAR_BIT
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>
#include <assert.h>

vil_skip_image::vil_skip_image(vil_image const &underlying, unsigned sx, unsigned sy)
  : base(underlying)
  , skipx(sx)
  , skipy(sy)
{
  assert(base);
  assert(skipx>0);
  assert(skipy>0);
}

vil_skip_image::~vil_skip_image() {
}

//--------------------------------------------------------------------------------

vil_image vil_skip_image::get_plane(int p) const {
  vil_image_impl *i = new vil_skip_image(base.get_plane(p), skipx, skipy);
  return i; //
}

bool vil_skip_image::get_section(void * buf, int x0, int y0, int w, int h) const {
  if (base.bits_per_component() % CHAR_BIT) {
    cerr << __FILE__ " : urgh!" << endl;
    return false; // FIXME
  }

  // make a buffer for (skipx*w) x 1 sections of base :
  unsigned cell_size = base.planes() * base.components() * base.bits_per_component();
  unsigned buffer_size = (skipx*w * cell_size);
  buffer_size /= CHAR_BIT;
  vcl_vector<unsigned char> buffer(buffer_size);

  // destination, as a unsigned char*
  unsigned char *dst = static_cast<unsigned char*>(buf);

  // for each raster
  for (unsigned j=0; j<h; ++j) {
    // get from underlying :
    bool v = base.get_section(buffer.begin(), skipx*x0, skipy*(y0+j), skipx*w, 1);
    if (!v)
      return false; // failed
    
    // 
    for (unsigned i=0; i<w; ++i)
      // copy the cell :
      for (unsigned k=0; k<cell_size; ++k)
	dst[cell_size*(j*w + i) + k] = buffer[cell_size*skipx*i + k];
  }

  return true;
}

bool vil_skip_image::put_section(void const * , int, int, int, int) {
  return false;
}

bool vil_skip_image::get_property(char const *, void * VCL_DEFAULT_VALUE(0)) const {
  return false;
}

//--------------------------------------------------------------------------------
