#ifndef vbl_binary_txx_
#define vbl_binary_txx_
/*
  fsm@robots.ox.ac.uk
*/
#include "vbl_binary.h"

#include <vcl_iostream.h>

// The function templates in this source file won't work specializations
// of vcl_vector<>, e.g. vector<bool>. Declare specializations of the
// functions here, to stop people instantiating them explicitly.
#define poison(T) \
VCL_DEFINE_SPECIALIZATION void vbl_binary_save(vcl_ostream &, vcl_vector<T > const &); \
VCL_DEFINE_SPECIALIZATION void vbl_binary_load(vcl_istream &, vcl_vector<T > &)
//heh heh. I actually *want* to do this -- fsm. poison(bool);
#undef poison


template </*typename*/class T>
void vbl_binary_save(vcl_ostream &f, vcl_vector<T> const &v)
{
  unsigned tmp = v.size();
  f.write((char*) &tmp, sizeof(tmp));
  f.write((char*) &v[0], v.size() * sizeof(T));
}

template </*typename*/class T>
void vbl_binary_load(vcl_istream &f, vcl_vector<T> &v)
{
  unsigned tmp;
  f.read((char*) &tmp, sizeof(tmp));
  v = vcl_vector<T>(tmp);
  f.read((char*) &v[0], v.size() * sizeof(T));
}

#define VBL_BINARY_INSTANTIATE(T) \
template void vbl_binary_save(vcl_ostream &, vcl_vector<T > const &); \
template void vbl_binary_load(vcl_istream &, vcl_vector<T > &)

#endif
