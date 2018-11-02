#ifndef MISC_IO__INCLUDED
#define MISC_IO__INCLUDED

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vil1/vil1_rgb.h>

template <class T>
std::ostream &operator<<(std::ostream &os, const vnl_vector<T> &vector);

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vector);

template <class T, class S>
std::ostream &operator<<(std::ostream &os, const std::map<T, S> &omap);

template <class T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &vector);

std::ostream &operator<<(std::ostream &os, const vil1_rgb<unsigned char> &colour);

#endif // MISC_IO__INCLUDED
