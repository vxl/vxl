#ifndef MISC_IO__INCLUDED
#define MISC_IO__INCLUDED

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_iosfwd.h>
#include <vnl/vnl_vector.h>
#include <vil1/vil1_rgb.h>

template <class T>
vcl_ostream &operator<<(vcl_ostream &os, const vnl_vector<T> &vector);

template <class T>
vcl_ostream &operator<<(vcl_ostream &os, const vcl_vector<T> &vector);

template <class T, class S>
vcl_ostream &operator<<(vcl_ostream &os, const vcl_map<T, S> &omap);

template <class T>
vcl_ostream &operator<<(vcl_ostream &os, const vcl_set<T> &vector);

vcl_ostream &operator<<(vcl_ostream &os, const vil1_rgb<unsigned char> &colour);

#endif // MISC_IO__INCLUDED
