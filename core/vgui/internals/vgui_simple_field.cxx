// This is core/vgui/internals/vgui_simple_field.cxx
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   23 Oct 99
// \brief  See vgui_simple_field.h for a description of this file.

#include <sstream>
#include "vgui_simple_field.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


template <class T>
vgui_simple_field<T>::vgui_simple_field(const char *text,T &variable_to_modify)
  : vgui_dialog_field(text)
  , var(variable_to_modify)
{
}

template <class T>
vgui_simple_field<T>::~vgui_simple_field()
{
}

template <class T>
std::string vgui_simple_field<T>::current_value() const
{
  std::stringstream ss;
  ss << T(var) << '\0';
  return ss.str();
}

template <class T>
bool vgui_simple_field<T>::update_value(const std::string &s)
{
  std::stringstream ss(s);
  ss >> var;
  return ss.eof();
}


template class vgui_simple_field<bool>;
template class vgui_simple_field<int>;
template class vgui_simple_field<long>;
template class vgui_simple_field<float>;
template class vgui_simple_field<double>;
