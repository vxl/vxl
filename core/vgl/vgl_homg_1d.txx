#ifndef vgl_homg_1d_txx_
#define vgl_homg_1d_txx_

#include "vgl_homg_1d.h"
#include <vcl_iostream.h>

template <class T>
vcl_ostream& operator<<(vcl_ostream& os, vgl_homg_1d<T> const& h)
{ os << "<vgl_homg_1d (" << h.x() << ',' << h.w() << ">)"; return os; }

#endif // vgl_homg_1d_txx_
