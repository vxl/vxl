// This is vxl/vsl/vsl_indent.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include <vsl/vsl_indent.h>
#include <vcl_iostream.h>
#include <vcl_map.txx>
#include <vcl_utility.h>

const int default_tab = 2;

typedef vcl_pair<int,int> indent_data_type;

typedef vcl_map<void*, indent_data_type, vcl_less<void*> > maps2i_type;

//: Global record of tab information for streams
//  Allows data to persist beyond the lifetime of the indent object itself,
//  which may be mercifully brief
static maps2i_type indent_data_map;

// Get pointer to tab and indent data for os
indent_data_type* indent_data(vcl_ostream& os)
{
  maps2i_type::iterator entry = indent_data_map.find(&os);
  if (entry==indent_data_map.end())
  {
    // Create a new entry
    indent_data_map[&os]=indent_data_type(0,default_tab);
  entry = indent_data_map.find(&os);
  }

  return &((*entry).second);
}

//: Increments current indent for given stream
void vsl_indent_inc(vcl_ostream& os)
{
  indent_data(os)->first++;
}

//: Decrements current indent for given stream
void vsl_indent_dec(vcl_ostream& os)
{
  indent_data(os)->first--;
}

//: Set number of spaces per increment step
void vsl_indent_set_tab(vcl_ostream& os, int t)
{
  indent_data(os)->second = t;
}

//: Number of spaces per increment step
int vsl_indent_tab(vcl_ostream& os)
{
  return indent_data(os)->second;
}

//: Set indentation to zero
void vsl_indent_clear(vcl_ostream& os)
{
  indent_data(os)->first =0;
}

vcl_ostream& operator<<(vcl_ostream& os, const vsl_indent& indent)
{
  indent_data_type* data = indent_data(os);

  int n = data->first * data->second;
  for (int i=0;i<n;i++) os<<' ';
  return os;
}

// removed explicit instantiation of map<void*, pair<int, int> > -- fsm.
