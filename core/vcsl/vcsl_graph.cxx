#include <vcsl/vcsl_graph.h>

#include <vcl/vcl_cassert.h>

#include <vcsl/vcsl_spatial.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_graph::vcsl_graph(void)
{
  _vertices=new vcl_vector<vcsl_spatial_ref>;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_graph::~vcsl_graph()
{
  delete _vertices;
}

//***************************************************************************
// Measurement
//***************************************************************************

//---------------------------------------------------------------------------
// Number of coordinate systems
//---------------------------------------------------------------------------
int vcsl_graph::count(void) const
{
  return _vertices->size();
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Has `this' `cs' as node ?
//---------------------------------------------------------------------------
bool vcsl_graph::has(const vcsl_spatial_ref &cs) const
{
  bool result;

  vcl_vector<vcsl_spatial_ref>::const_iterator i;

  result=false;
  for(i=_vertices->begin();i!=_vertices->end()&&!result;++i)
    result=(*i)==cs;

  return result;
}

//---------------------------------------------------------------------------
// Is `index' valid in the list of the spatial coordinate systems ?
//---------------------------------------------------------------------------
bool vcsl_graph::valid_index(const int index) const
{
  return (index>=0)&&(index<count());
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
// Spatial coordinate system number `index'
// REQUIRE: valid_index(index)
//---------------------------------------------------------------------------
vcsl_spatial_ref vcsl_graph::item(const int index) const
{
  // require
  assert(valid_index(index));

  return (*_vertices)[index];
}

//---------------------------------------------------------------------------
// Add `cs' in `this'
// REQUIRE: !has(cs)
//---------------------------------------------------------------------------
void vcsl_graph::put(const vcsl_spatial_ref &cs)
{
  // require
  assert(!has(cs));

  _vertices->push_back(cs);
}

//---------------------------------------------------------------------------
// Remove `cs' from `this'
// REQUIRE: has(cs)
//---------------------------------------------------------------------------
void vcsl_graph::remove(const vcsl_spatial_ref &cs)
{
  // require
  assert(has(cs));
  
  vcl_vector<vcsl_spatial_ref>::iterator i;

  for(i=_vertices->begin();i!=_vertices->end()&&((*i)!=cs);++i)
    ;
  _vertices->erase(i);
}

//***************************************************************************
// Basic operations
//***************************************************************************

// Set the flag `reached' to false for each spatial coordinate system
// Used by the search path algorithm
void vcsl_graph::init_vertices(void) const
{
  vcl_vector<vcsl_spatial_ref>::iterator i;

  for(i=_vertices->begin();i!=_vertices->end();++i)
    (*i)->set_reached(false);
}
