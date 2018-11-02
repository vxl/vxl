#include <iostream>
#include <sstream>
#include "mfpf_region_about_lineseg.h"
//:
// \file
// \brief Region centred on line between two points
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

mfpf_region_about_lineseg::mfpf_region_about_lineseg()
  : i0_(0),i1_(1),rel_wi_(1.2),rel_wj_(0.5),form_("box")
{
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_region_about_lineseg::~mfpf_region_about_lineseg() = default;

//: Returns false as the region is not centred on an input point
bool mfpf_region_about_lineseg::is_centred_on_pt() const
{
  return false;
}

//: Returns zero
unsigned mfpf_region_about_lineseg::ref_point_index() const
{
  return 0;
}

//: Returns zero also
unsigned mfpf_region_about_lineseg::orig_ref_point_index() const
{
  return 0;
}

//: Replace each point index i with new_index[i]
//  Allows for re-numbering of the points used.
//  Returns true if successful.
bool mfpf_region_about_lineseg::replace_index(
                        const std::vector<unsigned>& new_index)
{
  if (i0_>=new_index.size()) return false;
  if (new_index[i0_]==mfpf_invalid_index) return false;
  i0_=new_index[i0_];
  if (i1_>=new_index.size()) return false;
  if (new_index[i1_]==mfpf_invalid_index) return false;
  i1_=new_index[i1_];
  return true;
}

//: Returns reference point for region, mid_point(pts[i0],pts[i1])
vgl_point_2d<double> mfpf_region_about_lineseg::get_ref_point(
             const std::vector<vgl_point_2d<double> >& pts) const
{
  assert(i0_<pts.size());
  assert(i1_<pts.size());
  return midpoint(pts[i0_],pts[i1_]);
}

//: Defines a region centred on a point
mfpf_region_form mfpf_region_about_lineseg::set_up(
            const std::vector<vgl_point_2d<double> >& pts)
{
  assert(i0_<pts.size());
  assert(i1_<pts.size());

  vgl_vector_2d<double> u=pts[i1_]-pts[i0_];
  double L=u.length();
  wi_=rel_wi_*L;
  wj_=rel_wj_*L;

  vgl_point_2d<double> c=midpoint(pts[i0_],pts[i1_]);

  mfpf_pose pose(c,u/L);
  return mfpf_region_form(pose,form_,wi_,wj_);
}

//: Defines a region centred on a point
//  The aspect ratio of the region will be the same as that
//  from the last call to set_up.
mfpf_region_form mfpf_region_about_lineseg::get_region(
              const std::vector<vgl_point_2d<double> >& pts) const
{
  assert(i0_<pts.size());
  assert(i1_<pts.size());

  vgl_vector_2d<double> u=pts[i1_]-pts[i0_];
  double L=u.length();

  // Compute scale of this box relative to the last one called by set_up
  double rel_scale = (rel_wi_*L)/wi_;

  vgl_point_2d<double> c=midpoint(pts[i0_],pts[i1_]);

  mfpf_pose pose(c,(rel_scale/L)*u);
  return mfpf_region_form(pose,form_,wi_,wj_);
}

//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_region_about_lineseg::set_from_stream(std::istream &is)
{
  // Cycle through stream and produce a map of properties
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  // Extract the properties
  i0_=vul_string_atoi(props.get_required_property("i0"));
  i1_=vul_string_atoi(props.get_required_property("i1"));
  rel_wi_=vul_string_atof(props.get_required_property("rel_wi"));
  rel_wj_=vul_string_atof(props.get_required_property("rel_wj"));
  form_=props.get_optional_property("form","box");

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_region_about_lineseg::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_region_about_lineseg::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_region_about_lineseg::is_a() const
{
  return std::string("mfpf_region_about_lineseg");
}

//: Create a copy on the heap and return base class pointer
mfpf_region_definer* mfpf_region_about_lineseg::clone() const
{
  return new mfpf_region_about_lineseg(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_region_about_lineseg::print_summary(std::ostream& os) const
{
  os<<"{ i0: "<<i0_<<" i1: "<<i1_
    <<" rel_wi: "<<rel_wi_<<" rel_wj: "<<rel_wj_
    <<" form: "<<form_<<" } ";
}

//=======================================================================
// Method: save
//=======================================================================

void mfpf_region_about_lineseg::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,i0_);
  vsl_b_write(bfs,i1_);
  vsl_b_write(bfs,rel_wi_);
  vsl_b_write(bfs,rel_wj_);
  vsl_b_write(bfs,wi_);
  vsl_b_write(bfs,wj_);
  vsl_b_write(bfs,form_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_region_about_lineseg::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,i0_);
      vsl_b_read(bfs,i1_);
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
