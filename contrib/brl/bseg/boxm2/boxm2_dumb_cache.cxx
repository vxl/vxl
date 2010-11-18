#include "boxm2_dumb_cache.h"
//:
// \file

//: realization of abstract "get_block(block_id)"
boxm2_block* boxm2_dumb_cache::get_block(boxm2_block_id id)
{
  vcl_cout<<"Dumb Cache Get Block"<<vcl_endl;

  return NULL;
}

//: realization of abstract "get_block(block_id)"
void boxm2_dumb_cache::update_cache(boxm2_block_id id)
{
  vcl_cout<<"Dumb Cache Update"<<vcl_endl;
}
