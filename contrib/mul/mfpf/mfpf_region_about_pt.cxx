#include <iostream>
#include <sstream>
#include "mfpf_region_about_pt.h"
//:
// \file
// \brief Region centred on a single point, pose defined by other pts
// \author Tim Cootes

#include <vsl/vsl_binary_loader.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>

#include <vul/vul_string.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_region_about_pt::mfpf_region_about_pt()
  : i0_(0),i1_(0),i2_(1),rel_wi_(1.0),rel_wj_(1.0),form_("box")
{
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_region_about_pt::~mfpf_region_about_pt() = default;

//: Returns true as the region is centred on an input point
bool mfpf_region_about_pt::is_centred_on_pt() const
{
  return true;
}

//: Returns index of reference point on which the region is centred
unsigned mfpf_region_about_pt::ref_point_index() const
{
  return i0_;
}

//: Returns original index of reference point on which the region is centred
unsigned mfpf_region_about_pt::orig_ref_point_index() const
{
  return i0_orig_;
}

//: Replace each point index i with new_index[i]
//  Allows for re-numbering of the points used.
//  Returns true if successful.
bool mfpf_region_about_pt::replace_index(
                        const std::vector<unsigned>& new_index)
{
  if (i0_>=new_index.size()) return false;
  if (new_index[i0_]==mfpf_invalid_index) return false;
  i0_=new_index[i0_];
  if (i1_>=new_index.size()) return false;
  if (new_index[i1_]==mfpf_invalid_index) return false;
  i1_=new_index[i1_];
  if (i2_>=new_index.size()) return false;
  if (new_index[i2_]==mfpf_invalid_index) return false;
  i2_=new_index[i2_];
  return true;
}

//: Returns reference point for region, pts[i0()]
vgl_point_2d<double> mfpf_region_about_pt::get_ref_point(
            const std::vector<vgl_point_2d<double> >& pts) const
{
  assert(i0_<pts.size());
  return pts[i0_];
}

//: Defines a region centred on a point
mfpf_region_form mfpf_region_about_pt::set_up(
            const std::vector<vgl_point_2d<double> >& pts)
{
  assert(i0_<pts.size());
  assert(i1_<pts.size());
  assert(i2_<pts.size());

  vgl_vector_2d<double> u=pts[i2_]-pts[i1_];
  double L=u.length();
  wi_=rel_wi_*L;
  wj_=rel_wj_*L;

  mfpf_pose pose(pts[i0_],u/L);
  return mfpf_region_form(pose,form_,wi_,wj_);
}

//: Defines a region centred on a point
//  The aspect ratio of the region will be the same as that
//  from the last call to set_up.
mfpf_region_form mfpf_region_about_pt::get_region(
              const std::vector<vgl_point_2d<double> >& pts) const
{
  assert(i0_<pts.size());
  assert(i1_<pts.size());
  assert(i2_<pts.size());

  vgl_vector_2d<double> u=pts[i2_]-pts[i1_];
  double L=u.length();

  // Compute scale of this box relative to the last one called by set_up
  double rel_scale = (rel_wi_*L)/wi_;

  mfpf_pose pose(pts[i0_],(rel_scale/L)*u);
  return mfpf_region_form(pose,form_,wi_,wj_);
}

//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_region_about_pt::set_from_stream(std::istream &is)
{
  // Cycle through stream and produce a map of properties
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  // Extract the properties
  i0_=vul_string_atoi(props.get_required_property("i0"));
  i1_=vul_string_atoi(props.get_required_property("i1"));
  i2_=vul_string_atoi(props.get_required_property("i2"));
  rel_wi_=vul_string_atof(props.get_required_property("rel_wi"));
  rel_wj_=vul_string_atof(props.get_required_property("rel_wj"));
  form_=props.get_optional_property("form","box");

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_region_about_pt::set_from_stream", props, mbl_read_props_type());

  // Store original index of i0 since this may be renumbered later
  i0_orig_=i0_;

  return true;
}

//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_region_about_pt::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_region_about_pt::is_a() const
{
  return std::string("mfpf_region_about_pt");
}

//: Create a copy on the heap and return base class pointer
mfpf_region_definer* mfpf_region_about_pt::clone() const
{
  return new mfpf_region_about_pt(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_region_about_pt::print_summary(std::ostream& os) const
{
  os<<"{ i0: "<<i0_<<" i1: "<<i1_<<" i2: "<<i2_
    <<" rel_wi: "<<rel_wi_<<" rel_wj: "<<rel_wj_
    <<" form: "<<form_<<" } ";
}

//=======================================================================
// Method: save
//=======================================================================

void mfpf_region_about_pt::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,i0_);
  vsl_b_write(bfs,i1_);
  vsl_b_write(bfs,i2_);
  vsl_b_write(bfs,rel_wi_);
  vsl_b_write(bfs,rel_wj_);
  vsl_b_write(bfs,wi_);
  vsl_b_write(bfs,wj_);
  vsl_b_write(bfs,form_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_region_about_pt::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,i0_);
      vsl_b_read(bfs,i1_);
      vsl_b_read(bfs,i2_);
      vsl_b_read(bfs,rel_wi_);
      vsl_b_read(bfs,rel_wj_);
      vsl_b_read(bfs,wi_);
      vsl_b_read(bfs,wj_);
      vsl_b_read(bfs,form_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
