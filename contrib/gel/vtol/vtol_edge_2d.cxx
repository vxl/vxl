// This is gel/vtol/vtol_edge_2d.cxx
#include "vtol_edge_2d.h"
//:
// \file

#include <vtol/vtol_zero_chain.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vcl_cassert.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from the two endpoints `new_v1', `new_v2' and from a curve `new_curve'.
//  If `new_curve' is 0, a line is created from `new_v1' and `new_v2'.
//---------------------------------------------------------------------------
vtol_edge_2d::vtol_edge_2d(vtol_vertex_2d_sptr const& new_v1,
                           vtol_vertex_2d_sptr const& new_v2,
                           vsol_curve_2d_sptr const& new_curve)
{
  assert(new_v1!=0); v1_=new_v1->cast_to_vertex();
  assert(new_v2!=0); v2_=new_v2->cast_to_vertex();
  if (!new_curve)
    curve_=new vsol_line_2d(new_v1->point(),new_v2->point());
  else
    curve_=new_curve;

  link_inferior(new vtol_zero_chain(v1_,v2_));
}

vtol_edge_2d::vtol_edge_2d(vtol_vertex_sptr const& new_v1,
                           vtol_vertex_sptr const& new_v2,
                           vsol_curve_2d_sptr const& new_curve)
{
  assert(new_v1->cast_to_vertex_2d()); v1_=new_v1;
  assert(new_v2->cast_to_vertex_2d()); v2_=new_v2;
  if (!new_curve)
    curve_=new vsol_line_2d(v1_->cast_to_vertex_2d()->point(),
                            v2_->cast_to_vertex_2d()->point());
  else
    curve_=new_curve;

  link_inferior(new vtol_zero_chain(v1_,v2_));
}

//---------------------------------------------------------------------------
//: Pseudo copy constructor. Deep copy.
//---------------------------------------------------------------------------
vtol_edge_2d::vtol_edge_2d(vtol_edge_2d_sptr const& other)
  : curve_(0)
{
  topology_list::const_iterator i;
  for (i=other->inferiors()->begin();i!=other->inferiors()->end();++i)
  {
    link_inferior((*i)->clone()->cast_to_topology_object()->cast_to_zero_chain());
  }
  set_vertices_from_zero_chains();

  if (other->curve())
  {
    curve_ = other->curve()->clone()->cast_to_curve();
    // make sure the geometry and Topology are in sync
    if (v1_)
    {
      if (v1_->cast_to_vertex_2d())
      {
        curve_->set_p0(v1_->cast_to_vertex_2d()->point());
        curve_->touch();
      }
    }
    if (v2_)
    {
      if (v1_->cast_to_vertex_2d())
      {
        curve_->set_p1(v2_->cast_to_vertex_2d()->point());
        curve_->touch();
      }
    }
  }
  touch();
}

//---------------------------------------------------------------------------
//: Constructor from a zero-chain.
//---------------------------------------------------------------------------
//
// Constructor for an vtol_edge_2d. If the vtol_zero_chain has two vertices , then the
// first and last vertices of the vtol_zero_chain are used for endpoints and
// an ImplicitLine is assumed to be the curve.  Otherwise, the all data
// (v1_, v2_, curve_) are set to NULL.  The vtol_zero_chain, newchain, becomes
// the Inferior of the vtol_edge_2d.

vtol_edge_2d::vtol_edge_2d(vtol_zero_chain_sptr const& new_zero_chain)
{
  link_inferior(new_zero_chain);
  set_vertices_from_zero_chains();
  if (new_zero_chain->numinf()==2 && v1_->cast_to_vertex_2d() && v2_->cast_to_vertex_2d())
    // Safe to assume that it is a vsol_line_2d.
    curve_=new vsol_line_2d(v1_->cast_to_vertex_2d()->point(),
                            v2_->cast_to_vertex_2d()->point());
  else
    // User must set the type of curve needed.
    // Since guessing could get confusing.
    // So NULL indicates an edge of unknown type.
    curve_=0;
  touch();
}

//: Constructor for a vtol_edge_2d from a list of zero-chains.
// The list of zero-chains, newchains, is
// assumed to be ordered along an edge. This method assigns the first
// vertex in the chain list to v1_, and assigns the last vertex in the
// chain list to v2_. No assumptions are made as to the curve type. The
// data member, curve_ is left to be NULL.

vtol_edge_2d::vtol_edge_2d(zero_chain_list const& newchains)
{
  // 1) Link the inferiors.

  for (zero_chain_list::const_iterator i=newchains.begin(); i!=newchains.end(); ++i)
    link_inferior(*i);

  // 2) Set v1_ and v2_;

  set_vertices_from_zero_chains();
  curve_=0;
}

//: Constructor for a linear vtol_edge_2d.
// The coordinates, (x1, y1, z1), determine vtol_vertex_2d, v1_.
// The coordinates, (x2, y2, z2), determine v2_.
// If curve is NULL, a vsol_line_2d is generated for the vtol_edge_2d.

vtol_edge_2d::vtol_edge_2d(double x1, double y1,
                           double x2, double y2,
                           vsol_curve_2d_sptr curve)
{
  v1_=new vtol_vertex_2d(x1,y1);
  v2_=new vtol_vertex_2d(x2,y2);
  if (!curve)
    if (v1_->cast_to_vertex_2d() && v2_->cast_to_vertex_2d())
      curve_=new vsol_line_2d(v1_->cast_to_vertex_2d()->point(),
                              v2_->cast_to_vertex_2d()->point());
  else
    curve_=curve->clone()->cast_to_curve();

  link_inferior(new vtol_zero_chain(v1_,v2_));
}

//: Constructor for an vtol_edge_2d from a vsol_curve_2d.
// If edgecurve is of vsol_line_2d
// type, vertex locations for endpoints, v1_ and v2_, are computed from
// the vsol_line_2d parameters.  If edgecurve is of any other type, v1_
// and v2_ are retrieved from the end points of the curve.

vtol_edge_2d::vtol_edge_2d(vsol_curve_2d &edgecurve)
{
  v1_ = new vtol_vertex_2d(*(edgecurve.p0()));
  v2_ = new vtol_vertex_2d(*(edgecurve.p1()));
  link_inferior(new vtol_zero_chain(v1_,v2_));
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vtol_edge_2d::clone() const
{
  return new vtol_edge_2d(vtol_edge_2d_sptr(const_cast<vtol_edge_2d*>(this)));
}

//---------------------------------------------------------------------------
//: Set the curve with `new_curve'
//---------------------------------------------------------------------------
void vtol_edge_2d::set_curve(vsol_curve_2d &new_curve)
{
  curve_=&new_curve;
  touch(); //Update timestamp
}

// ******************************************************
//
//    Operators
//

bool vtol_edge_2d::operator==(const vtol_edge_2d &other) const
{
  if (this==&other) return true;

  if ( (curve() && !other.curve()) ||
       (!curve() && other.curve()) )
    return false;

  if (curve() && (*curve())!=(*other.curve()))
    return false;

  if (!(*v1_==*(other.v1_)) || !(*v2_==*(other.v2_)))
    return false;

  vtol_zero_chain_sptr zc1=zero_chain();
  vtol_zero_chain_sptr zc2=other.zero_chain();
  if (!zc1||!zc2)
    return false;
  return *zc1==*zc2;
}

//: edge equality
bool vtol_edge_2d::operator==(const vtol_edge &other) const
{
  return other.cast_to_edge_2d() && *this == *other.cast_to_edge_2d();
}

//: spatial object equality
bool vtol_edge_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.cast_to_topology_object() &&
   obj.cast_to_topology_object()->cast_to_edge() &&
   *this == *obj.cast_to_topology_object()->cast_to_edge();
}

// ******************************************************
//
//    Inferior/Superior Accessor Functions
//
// ******************************************************
//
//    I/O methods
//

//:
// This method outputs all edge information to the vcl_ostream, strm.  It
// indents various levels of output by the number given in blanking.
void vtol_edge_2d::describe(vcl_ostream &strm,
                            int blanking) const
{
  for (int i1=0; i1<blanking; ++i1) strm << ' ';
  print(strm);
  for (int i2=0; i2<blanking; ++i2) strm << ' ';
  if (v1_) {
    v1_->print(strm);
  } else {
    strm << "Null vertex 1\n";
  }
  for (int i3=0; i3<blanking; ++i3) strm << ' ';
  if (v2_) {
    v2_->print(strm);
  } else {
    strm << "Null vertex 2\n";
  }
}

//:
// This method outputs a brief vtol_edge_2d info with vtol_edge_2d object address.
void vtol_edge_2d::print(vcl_ostream &strm) const
{
   strm<<"<vtol_edge_2d "<<(void const *)this <<"> with id "<<get_id()<<'\n';
}

//: copy the geometry

void vtol_edge_2d::copy_geometry(const vtol_edge &other)
{
  if (other.cast_to_edge_2d())
    curve_ = other.cast_to_edge_2d()->curve();
}

bool vtol_edge_2d::compare_geometry(const vtol_edge &other) const
{
  // we want to compare geometry

  if (other.cast_to_edge_2d())
    return (*curve()) == *(other.cast_to_edge_2d()->curve());
  else
    return false;
}

void vtol_edge_2d::compute_bounding_box() const
{
  this->empty_bounding_box();
  vsol_curve_2d_sptr c = this->curve();
  if (c && c->cast_to_vdgl_digital_curve())
    this->set_bounding_box(c->cast_to_vdgl_digital_curve()->get_bounding_box());
  else // the geometry is either a line segment or unknown
    vtol_topology_object::compute_bounding_box();
}
