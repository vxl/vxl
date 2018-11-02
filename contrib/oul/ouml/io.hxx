#ifndef ouml_io_hxx_
#define ouml_io_hxx_
// miscellaneous io operators

#include <iostream>
#include "io.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
std::ostream &operator <<(std::ostream &os, const vnl_vector<T> &vec)
{
  typename vnl_vector<T>::const_iterator iter, end_iter;

  end_iter = vec.end();
  std::cout << "<Vector: ";
  for (iter=vec.begin(); iter!=end_iter; iter++)
  {
    std::cout << ' ' << *iter;
  }
  std::cout << "> ";

  return os;
}

template <class T>
std::ostream &operator <<(std::ostream &os, const std::vector<T> &vec)
{
  typename std::vector<T>::const_iterator iter, end_iter;

  end_iter = vec.end();
  std::cout << "<Vector: ";
  for (iter=vec.begin(); iter!=end_iter; iter++)
  {
    std::cout << ' ' << *iter;
  }
  std::cout << "> ";

  return os;
}

template <class T, class S>
std::ostream &operator<<(std::ostream &os, const std::map<T, S> &omap)
{
  typename std::map<T, S>::const_iterator iter, end_iter;

  end_iter = omap.end();
  os << "<Map: ";
  for (iter=omap.begin(); iter!=end_iter; iter++)
  {
    os << " <" << (*iter).first << ", " << (*iter).second << '>';
  }
  os << "> ";

  return os;
}

template <class T>
std::ostream &operator <<(std::ostream &os, const std::set<T> &vec)
{
  typename std::set<T>::const_iterator iter, end_iter;

  end_iter = vec.end();
  os << "<Set: ";
  for (iter=vec.begin(); iter!=end_iter; iter++)
  {
    os << ' ' << *iter;
  }
  os << "> ";

  return os;
}


std::ostream &operator<<(std::ostream &os, const vil1_rgb<unsigned char> &colour)
{
  os << "<RGB = " << (int)colour.R() << ", " << (int)colour.G() << ", "
     << (int)colour.B() << ">\n";
  return os;
}

#endif // ouml_io_hxx_
