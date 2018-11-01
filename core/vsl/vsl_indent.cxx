// This is core/vsl/vsl_indent.cxx
//:
// \file

#include <iostream>
#include <map>
#include <utility>
#include "vsl_indent.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

constexpr int default_tab = 2;

typedef std::pair<int,int> indent_data_type;

// Get pointer to tab and indent data for os
indent_data_type* indent_data(std::ostream& os)
{
  typedef std::map<void*, indent_data_type, std::less<void*> > maps2i_type;
  // Global record of tab information for streams.
  // Allows data to persist beyond the lifetime of the indent object itself,
  // which may be mercifully brief
  static maps2i_type indent_data_map;

  auto entry = indent_data_map.find(&os);
  if (entry==indent_data_map.end())
  {
    // Create a new entry
    indent_data_map[&os]=indent_data_type(0,default_tab);
  entry = indent_data_map.find(&os);
  }

  return &((*entry).second);
}

//: Increments current indent for given stream
void vsl_indent_inc(std::ostream& os)
{
  indent_data(os)->first++;
}

//: Decrements current indent for given stream
void vsl_indent_dec(std::ostream& os)
{
  indent_data(os)->first--;
}

//: Set number of spaces per increment step
void vsl_indent_set_tab(std::ostream& os, int t)
{
  indent_data(os)->second = t;
}

//: Number of spaces per increment step
int vsl_indent_tab(std::ostream& os)
{
  return indent_data(os)->second;
}

//: Set indentation to zero
void vsl_indent_clear(std::ostream& os)
{
  indent_data(os)->first =0;
}

std::ostream& operator<<(std::ostream& os, const vsl_indent& /*indent*/)
{
  indent_data_type* data = indent_data(os);

  int n = data->first * data->second;
  for (int i=0;i<n;i++) os<<' ';
  return os;
}

//: Tidy up the internal indent map to remove potential memory leaks
//  The details of indents for each stream are stored in a static
//  map.  When testing for memory leaks, this is flagged, creating
//  lots of noise in the output of memory leak checkers.
//  This call empties the map, removing the potential leak.
//  Pragmatically it is called in the vsl_delete_all_loaders()
//
//  This should no longer be needed, since that static map was made a static
//  inside the function indent_data() instead of a global one. - PVr.
void vsl_indent_clear_all_data()
{
}

// removed explicit instantiation of map<void*, pair<int, int> > -- fsm.
