// This is oxl/vgui/internals/vgui_simple_field.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   23 Oct 99
// \brief  See vgui_simple_field.h for a description of this file.

#include "vgui_simple_field.h"

#include <vcl_cstring.h>
#include <vcl_sstream.h>


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
vcl_string vgui_simple_field<T>::current_value() const
{
  vcl_stringstream ss;
  ss << T(var) << '\0';
  return ss.str();
}

template <class T>
bool vgui_simple_field<T>::update_value(const vcl_string &s)
{
  vcl_stringstream ss(s);
  ss >> var;
  return ss.eof();
}


template class vgui_simple_field<bool>;
template class vgui_simple_field<int>;
template class vgui_simple_field<long>;
template class vgui_simple_field<float>;
template class vgui_simple_field<double>;
