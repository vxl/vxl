/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_block_cache_image.h"
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_cstring.h>  // strcmp()

vil_block_cache_image::vil_block_cache_image(vil_image i_, unsigned bx, unsigned by)
  : base(i_)
  , block_size_x(bx)
  , block_size_y(by)
{
}

vil_block_cache_image::~vil_block_cache_image() {
}

vil_image vil_block_cache_image::get_plane(int ) const {
  return 0;
}

bool vil_block_cache_image::get_section(void *buf, int x0, int y0, int w, int h) const {
  // implement this, please.
  return base.get_section(buf, x0, y0, w, h);
}

bool vil_block_cache_image::put_section(void const *buf, int x0, int y0, int w, int h) {
  return base.put_section(buf, x0, y0, w, h);
}

//--------------------------------------------------------------------------------
  
bool vil_block_cache_image::get_property(char const *tag, 
					 void *out VCL_DEFAULT_VALUE(0)) const 
{
  if (strcmp(tag, "is_blocked") == 0)
    return true;
  
  if (strcmp(tag, "block_size_x") == 0) {
    *(unsigned *)out = block_size_x;
    return true;
  }
  
  if (strcmp(tag, "block_size_y") == 0) {
    *(unsigned *)out = block_size_y;
    return true;
  }
  
  return false;
}
