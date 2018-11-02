// This is gel/vsol/vsol_spatial_object_2d.cxx
#include <iostream>
#include "vsol_spatial_object_2d.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsol/vsol_box_2d.h>
#include <vsl/vsl_binary_loader.h>

const float vsol_spatial_object_2d::eps=1.0e-3f;
//int vsol_spatial_object_2d::tagcount_=0;

const char * vsol_spatial_object_2d::SpatialTypes[] =
{
  "NO_TYPE             ",
  "TOPOLOGYOBJECT      ",
  "POINT               ",
  "CURVE               ",
  "REGION              ",
  "SPATIALGROUP        ",
  "NUM_SPATIALOBJECT_TYPES"
};

vsol_spatial_object_2d::vsol_spatial_object_2d()
  : bounding_box_(nullptr)
{
  this->tag_ = 0;
  this->id_ = 0;
  set_tag_id(++tagcount_);
}

vsol_spatial_object_2d::vsol_spatial_object_2d(vsol_spatial_object_2d const& s)
  : vsol_spatial_object(s), bounding_box_(nullptr)
{
  this->tag_ = 0;
  this->id_ = s.get_id();
  set_tag_id(++tagcount_);
}

const char * vsol_spatial_object_2d::get_name() const
{
  vsol_spatial_object_2d_type type =spatial_type();
  if (type > 0 && type < vsol_spatial_object_2d::NUM_SPATIALOBJECT_TYPES)
    return SpatialTypes[type];
  else
    return SpatialTypes[0];
}

vsol_spatial_object_2d::~vsol_spatial_object_2d() = default;

//: Return IO version number;
short vsol_spatial_object_2d::version() const
{
  return 1;
}

void vsol_spatial_object_2d::compute_bounding_box() const
{
  if (!bounding_box_) bounding_box_=new vsol_box_2d; bounding_box_->touch();
}

void vsol_spatial_object_2d::empty_bounding_box() const
{
  bounding_box_=new vsol_box_2d;
}

void vsol_spatial_object_2d::set_bounding_box(double x, double y) const
{
  bounding_box_=new vsol_box_2d; bounding_box_->add_point(x,y);
}

void vsol_spatial_object_2d::set_bounding_box(vsol_box_2d_sptr const& box) const
{
  bounding_box_=new vsol_box_2d(*box);
}

void vsol_spatial_object_2d::add_to_bounding_box(double x, double y) const
{
  if (!bounding_box_) bounding_box_=new vsol_box_2d; bounding_box_->add_point(x,y);
}

void vsol_spatial_object_2d::add_to_bounding_box(vsol_box_2d_sptr const& comp_box) const
{
  if (!bounding_box_)
    bounding_box_=new vsol_box_2d;
  bounding_box_->grow_minmax_bounds(comp_box);
}

//: Bounds Accessors:
// min_ and max_ are provided as methods on vsol_spatial_object_2d
// to be consistent with the previous interface
// Additional bounds accessors are available directly
// on vsol_box_2d.  - JLM

void vsol_spatial_object_2d::check_update_bounding_box() const
{
  if (!bounding_box_)
  {
    bounding_box_ = new vsol_box_2d;
    this->compute_bounding_box();
    bounding_box_->touch();
    return;
  }
  if (bounding_box_->older(this))
  { // NOTE: first touch then compute, to avoid infinite loop!! - PVr
    bounding_box_->touch();
    this->compute_bounding_box();
  }
}

double vsol_spatial_object_2d::get_min_x() const
{
  check_update_bounding_box(); return bounding_box_->get_min_x();
}

double vsol_spatial_object_2d::get_max_x() const
{
  check_update_bounding_box(); return bounding_box_->get_max_x();
}

double vsol_spatial_object_2d::get_min_y() const
{
  check_update_bounding_box(); return bounding_box_->get_min_y();
}

double vsol_spatial_object_2d::get_max_y() const
{
  check_update_bounding_box(); return bounding_box_->get_max_y();
}

//: Binary save self to stream.
void
vsol_spatial_object_2d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, this->version());
  vsl_b_write(os, this->tag_);
  vsl_b_write(os, this->id_);
}

//: Binary load self from stream.
void
vsol_spatial_object_2d::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsl_b_read(is, this->tag_);
    vsl_b_read(is, this->id_);
    break;

   default:
    std::cerr << "I/O ERROR: vsol_spatial_object_2d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//==============================================
//: Allows derived class to be loaded by base-class pointer.
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(os,base_ptr);".  This loads derived class
//  objects from the stream, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(vsol_spatial_object_2d const& b)
{
  vsl_binary_loader<vsol_spatial_object_2d>::instance().add(b);
}
