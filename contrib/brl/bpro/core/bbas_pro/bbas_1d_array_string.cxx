#include "bbas_1d_array_string.h"
#include <vbl/io/vbl_io_array_1d.h>
// Binary write 1d array string to stream
void vsl_b_write(vsl_b_ostream & os, bbas_1d_array_string const& array){
  vsl_b_write(os, array.data_array);
}

// Binary load 1d array string from stream.
void vsl_b_read(vsl_b_istream& is, bbas_1d_array_string & array){
  vbl_array_1d<std::string> temp;
  vsl_b_read(is, temp);
  array.data_array = temp;
}

// Binary load 1d array string from stream.
void vsl_b_read(vsl_b_istream& is, bbas_1d_array_string* aptr){
  delete aptr;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    aptr = new bbas_1d_array_string(0);
    vsl_b_read(is, *aptr);
  }
  else
    aptr = nullptr;
}

void vsl_b_read(vsl_b_istream& is, bbas_1d_array_string_sptr& aptr){
  bbas_1d_array_string as(0);
  vsl_b_read(is, as);
  aptr = new bbas_1d_array_string(as);
}

// Binary write scene pointer to stream
void vsl_b_write(vsl_b_ostream& os, const bbas_1d_array_string_sptr & aptr){
  if(!aptr)
    return;
  vsl_b_write(os, *(aptr.ptr()));
}
