#ifndef boxm_block_txx_
#define boxm_block_txx_

#include "boxm_block.h"

template <class T>
void boxm_block<T>::b_read(vsl_b_istream &s)
{
}
  
  
template <class T>
void boxm_block<T>::b_write(vsl_b_ostream &s)
{
}

#define BOXM_BLOCK_INSTANTIATE(T) \
template boxm_block<T>; 

#endif