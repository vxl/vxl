//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// Class: vgui_simple_field
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 23 Oct 99
//
//-----------------------------------------------------------------------------

#include "vgui_simple_field.h"

#include <vcl_strstream.h>


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
  vcl_ostrstream ss;
  ss << T(var) << '\0';
  return vcl_string(ss.str());
}

template <class T>
bool vgui_simple_field<T>::update_value(const vcl_string &s)
{
  char buf[1024];
  strcpy(buf, s.c_str());
  vcl_istrstream ss(buf);
  ss >> var;
  return (ss && ss.eof());
}


template class vgui_simple_field<bool>;
template class vgui_simple_field<int>;
template class vgui_simple_field<long>;
template class vgui_simple_field<float>;
template class vgui_simple_field<double>;
