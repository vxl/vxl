/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vnl_block.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>

void vnl_block_raise_exception(char const *FILE, int LINE, char const *why)
{
  vcl_cerr << FILE << ":" << LINE << ": " << why << vcl_endl;
  assert(false);
  // throw;
}
