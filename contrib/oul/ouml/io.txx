#ifndef ouml_io_txx_
#define ouml_io_txx_
// miscellaneous io operators

#include "io.h"
#include <vcl_iostream.h>

template <class T>
vcl_ostream &operator <<(vcl_ostream &os, const vnl_vector<T> &vec)
{
  typename vnl_vector<T>::const_iterator iter, end_iter;

  end_iter = vec.end();
  vcl_cout << "<Vector: ";
  for (iter=vec.begin(); iter!=end_iter; iter++)
  {
    vcl_cout << ' ' << *iter;
  }
  vcl_cout << "> ";

  return os;
}

template <class T>
vcl_ostream &operator <<(vcl_ostream &os, const vcl_vector<T> &vec)
{
  typename vcl_vector<T>::const_iterator iter, end_iter;

  end_iter = vec.end();
  vcl_cout << "<Vector: ";
  for (iter=vec.begin(); iter!=end_iter; iter++)
  {
    vcl_cout << ' ' << *iter;
  }
  vcl_cout << "> ";

  return os;
}

template <class T, class S>
vcl_ostream &operator<<(vcl_ostream &os, const vcl_map<T, S> &omap)
{
  typename vcl_map<T, S>::const_iterator iter, end_iter;

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
vcl_ostream &operator <<(vcl_ostream &os, const vcl_set<T> &vec)
{
  typename vcl_set<T>::const_iterator iter, end_iter;

  end_iter = vec.end();
  os << "<Set: ";
  for (iter=vec.begin(); iter!=end_iter; iter++)
  {
    os << ' ' << *iter;
  }
  os << "> ";

  return os;
}


vcl_ostream &operator<<(vcl_ostream &os, const vil1_rgb<unsigned char> &colour)
{
  os << "<RGB = " << (int)colour.R() << ", " << (int)colour.G() << ", "
     << (int)colour.B() << ">\n";
  return os;
}

#endif // ouml_io_txx_
