// This is gel/vtol/vtol_edge_2d.h
#ifndef vtol_edge_2d_h_
#define vtol_edge_2d_h_
//:
// \file
// \brief Represents the basic 1D topological entity with 2d geometry (curve)
//
//  The vtol_edge_2d class is used to represent a topological edge.  A vtol_edge_2d
//  maintains a data pointer to the specific mathematical curve geometry
//  which describes the point set that makes up the edge.  For convenience
//  in working with linear edges, pointers to the two endpoint vertices
//  are maintained. The direction of an edge is the vector from v1_ to v2_.
//  A 1-chain is the superior of the edge in the topological
//  hierarchy, and a 0-chain is the inferior of the edge in the
//  topological hierarchy.  In rare cases, an edge will be used to represent
//  a ray.  In this case, only v1_ will be valid and v2_ will be NULL.
//
// \verbatim
//  Modifications:
//   JLM December 1995, Added timeStamp (Touch) to
//       operations which affect bounds.
//   JLM December 1995 Added method for ComputeBoundingBox
//       (Need to decide proper policy for curved edges
//       and possibly inconsistent linear edge geometry)
//
//   Samer Abdallah - 21/06/1996
//     Robotics Research Group, Oxford
//     Changed the constructor vtol_edge_2d(vtol_edge_2d &) to vtol_edge_2d(const vtol_edge_2d &)
//
//   JLM September 1996 - Added default curve argument to two vertex
//     constructors.  This addition is necessary because it is not
//     always the case that one wants to construct an ImplicitLine from
//     two vertices.  The curve might be a DigitalCurve, for example.
//     On the other hand in grouping or similar applications, the
//     curve endpoints can be different from the topological connections.
//     So, it is necessary to pass in the vertices as well as the curve.
//
//   02-26-97 - Peter Vanroose - Added implementation for virtual Transform()
//   may 2000 - PTU - ported to vxl
//   November 30, 2002 - added local implementation for compute_bounding_box
//   Dec. 2002, Peter Vanroose -interface change: vtol objects -> smart pointers
//   9 Jan. 2003, Peter Vanroose - added "copy_geometry()"
// \endverbatim

#include <vcl_iosfwd.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vtol/vtol_edge.h>

//: topological edge

class vtol_edge_2d : public vtol_edge
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  vsol_curve_2d_sptr curve_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor. Empty edge. Not a valid edge.
  //---------------------------------------------------------------------------
  vtol_edge_2d(void) : vtol_edge(), curve_(0) {}

  //---------------------------------------------------------------------------
  //: Constructor from the two endpoints `new_v1', `new_v2' and from a curve `new_curve'.
  //  If `new_curve' is 0, a line is created from `new_v1' and `new_v2'.
  //---------------------------------------------------------------------------
  vtol_edge_2d(vtol_vertex_2d_sptr const& new_v1,
               vtol_vertex_2d_sptr const& new_v2,
               const vsol_curve_2d_sptr &new_curve=0);

  vtol_edge_2d(vtol_vertex_sptr const& new_v1,
               vtol_vertex_sptr const& new_v2,
               const vsol_curve_2d_sptr &new_curve=0);
 private:
  // deprecated interface:
  vtol_edge_2d(vtol_vertex_2d &new_v1,
               vtol_vertex_2d &new_v2,
               const vsol_curve_2d_sptr &new_curve=0);

  //---------------------------------------------------------------------------
  //: Copy constructor. Deep copy.  Deprecated.
  //---------------------------------------------------------------------------
  vtol_edge_2d(const vtol_edge_2d &other);
 public:
  //---------------------------------------------------------------------------
  //: Pseudo copy constructor. Deep copy.
  //---------------------------------------------------------------------------
  vtol_edge_2d(vtol_edge_2d_sptr const& other);

  //---------------------------------------------------------------------------
  //: Constructor from a zero-chain.
  //---------------------------------------------------------------------------
  explicit vtol_edge_2d(vtol_zero_chain_sptr const& new_zero_chain);
 private:
  // Deprecated:
  explicit vtol_edge_2d(vtol_zero_chain &new_zero_chain);
 public:
  //---------------------------------------------------------------------------
  //: Constructor from an array of zero-chains.
  //---------------------------------------------------------------------------
  explicit vtol_edge_2d(zero_chain_list const& new_zero_chains);

  explicit vtol_edge_2d(vsol_curve_2d &);

  //: Constructor from two vertices (alternate interface)
  vtol_edge_2d(double, double, double, double, vsol_curve_2d_sptr c=0);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_edge_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d* clone(void) const;
  
  //: Return a platform independent string identifying the class
  vcl_string is_a() const;

  //---------------------------------------------------------------------------
  //: Return the curve associated to `this'
  //---------------------------------------------------------------------------
  vsol_curve_2d_sptr curve(void) const { return curve_; }

  //---------------------------------------------------------------------------
  //: Set the curve with `new_curve'
  //---------------------------------------------------------------------------
  virtual void set_curve(vsol_curve_2d &new_curve);

  //---------------------------------------------------------------------------
  //: Equality operators
  //---------------------------------------------------------------------------
  virtual bool operator==(const vtol_edge_2d &other) const;
  inline bool operator!=(const vtol_edge_2d &other)const{return !operator==(other);}
  bool operator==(const vtol_edge &other) const; // virtual of vtol_edge
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_edge_2d *cast_to_edge_2d(void) const { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_edge_2d *cast_to_edge_2d(void) { return this; }

  virtual void compute_bounding_box(void) const; //A local implementation

  virtual void print(vcl_ostream &strm=vcl_cout) const;
  virtual void describe(vcl_ostream &strm=vcl_cout,
                        int blanking=0) const;

  //:  copy the geometry
  virtual void copy_geometry(const vtol_edge &other);

  //: comparison of geometry
  virtual bool compare_geometry(const vtol_edge &other) const;
};

#endif // vtol_edge_2d_h_
