
#ifndef MISC_IO__INCLUDED
#define MISC_IO__INCLUDED 1

#include <vcl/vcl_vector.h>
#include <vcl/vcl_map.h>
#include <vcl/vcl_set.h>
#include <iostream.h>
#include <vnl/vnl_vector.h>
#include <vil/vil_rgb.h>

template <class T>
ostream &operator<<(ostream &os, const vnl_vector<T> &vector);

template <class T>
ostream &operator<<(ostream &os, const vcl_vector<T> &vector);

template <class T, class S>
ostream &operator<<(ostream &os, const vcl_map<T, S> &omap);

template <class T>
ostream &operator<<(ostream &os, const vcl_set<T> &vector);

ostream &operator<<(ostream &os, const vil_rgb<unsigned char> &colour);

#endif
