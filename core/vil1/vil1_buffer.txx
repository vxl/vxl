#ifndef vil_buffer_txx_
#define vil_buffer_txx_
/*
  fsm@robots.ox.ac.uk
*/
#include "vil_buffer.h"

#undef VIL_BUFFER_INSTANTIATE
#define VIL_BUFFER_INSTANTIATE(T) template struct vil_buffer<T >;

#endif
