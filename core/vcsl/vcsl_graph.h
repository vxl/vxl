#ifndef VCSL_GRAPH_H
#define VCSL_GRAPH_H
//*****************************************************************************
//
// .NAME vcsl_graph - Spatial coordinate system graph
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_graph.h
// .FILE    vcsl/vcsl_graph.cxx
//
// .SECTION Description
//
// Graph where nodes are spatial coordinate systems and arrows are
// transformations. Only the nodes are in the graph class. The transformations
// are in the spatial coordinates systems
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/08/01 François BERTEL Creation.
//*****************************************************************************

#include <vcsl/vcsl_graph_ref.h>

#include <vbl/vbl_ref_count.h>
#include <vcl/vcl_vector.h>
#include <vcsl/vcsl_spatial_ref.h>

class vcsl_graph
  :public vbl_ref_count
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_graph(void);

  //: Destructor
  virtual ~vcsl_graph();
  
  //***************************************************************************
  // Measurement
  //***************************************************************************
  
  //: Number of coordinate systems
  virtual int count(void) const;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Has `this' `cs' as node ?
  virtual bool has(const vcsl_spatial_ref &cs) const;

  //: Is `index' valid in the list of the spatial coordinate systems ?
  virtual bool valid_index(const int index) const;

  //***************************************************************************
  // Access
  //***************************************************************************

  //: Spatial coordinate system number `index'
  //: REQUIRE: valid_index(index)
  virtual vcsl_spatial_ref item(const int index) const;

  //: Add `cs' in `this'
  //: REQUIRE: !has(cs)
  virtual void put(const vcsl_spatial_ref &cs);

  //: Remove `cs' from `this'
  //: REQUIRE: has(cs)
  virtual void remove(const vcsl_spatial_ref &cs);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Set the flag `reached' to false for each spatial coordinate system.
  //: Used by the search path algorithm
  virtual void init_vertices(void) const;

protected:

  //: Vertices of the graph: all the spatial coordinate systems
  vcl_vector<vcsl_spatial_ref> *_vertices;
};

#endif // #ifndef VCSL_GRAPH_H
