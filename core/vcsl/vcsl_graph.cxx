// This is core/vcsl/vcsl_graph.cxx
#include "vcsl_graph.h"
#include <vcl_cassert.h>
#include <vcsl/vcsl_spatial.h>

//---------------------------------------------------------------------------
// Has `this' `cs' as node ?
//---------------------------------------------------------------------------
bool vcsl_graph::has(const vcsl_spatial_sptr &cs) const
{
  bool result;

  vcl_vector<vcsl_spatial_sptr>::const_iterator i;

  result=false;
  for (i=vertices_.begin();i!=vertices_.end()&&!result;++i)
    result=(*i)==cs;

  return result;
}

//---------------------------------------------------------------------------
// Spatial coordinate system number `index'
// REQUIRE: valid_index(index)
//---------------------------------------------------------------------------
vcsl_spatial_sptr vcsl_graph::item(unsigned int index) const
{
  // require
  assert(valid_index(index));

  return vertices_[index];
}

//---------------------------------------------------------------------------
// Add `cs' in `this'
// REQUIRE: !has(cs)
//---------------------------------------------------------------------------
void vcsl_graph::put(const vcsl_spatial_sptr &cs)
{
  // require
  assert(!has(cs));

  vertices_.push_back(cs);
}

//---------------------------------------------------------------------------
// Remove `cs' from `this'
// REQUIRE: has(cs)
//---------------------------------------------------------------------------
void vcsl_graph::remove(const vcsl_spatial_sptr &cs)
{
  // require
  assert(has(cs));

  vcl_vector<vcsl_spatial_sptr>::iterator i;

  for (i=vertices_.begin(); i!=vertices_.end()&&((*i)!=cs); ++i)
    ;
  vertices_.erase(i);
}

// Set the flag `reached' to false for each spatial coordinate system
// Used by the search path algorithm
void vcsl_graph::init_vertices() const
{
  vcl_vector<vcsl_spatial_sptr>::const_iterator i;

  for (i=vertices_.begin();i!=vertices_.end();++i)
    (*i)->set_reached(false);
}
