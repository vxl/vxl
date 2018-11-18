#include <iostream>
#include <sstream>
#include "mfpf_edge_finder_builder.h"
//:
// \file
// \brief Builder for mfpf_edge_finder objects.
// \author Tim Cootes

#include <mfpf/mfpf_edge_finder.h>
#include <vsl/vsl_binary_loader.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_edge_finder_builder::mfpf_edge_finder_builder() = default;

//=======================================================================
// Destructor
//=======================================================================

mfpf_edge_finder_builder::~mfpf_edge_finder_builder() = default;

//: Define region size in world co-ordinates
//  Sets up ROI to cover given box (with samples at step_size()),
//  with ref point at centre.
void mfpf_edge_finder_builder::set_region_size(double, double)
{
  // NYI
}

//: Create new mfpf_edge_finder on heap
mfpf_point_finder* mfpf_edge_finder_builder::new_finder() const
{
  return new mfpf_edge_finder();
}

//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_edge_finder_builder::clear(unsigned /*n_egs*/)
{
  // NYI
}

//: Add one example to the model
void mfpf_edge_finder_builder::add_example(const vimt_image_2d_of<float>& /*image*/,
                                           const vgl_point_2d<double>& /*p*/,
                                           const vgl_vector_2d<double>& /*u*/)
{
  // NYI
}

//: Build this object from the data supplied in add_example()
void mfpf_edge_finder_builder::build(mfpf_point_finder& pf)
{
  assert(pf.is_a()=="mfpf_edge_finder");
  auto& ef = static_cast<mfpf_edge_finder&>(pf);
  set_base_parameters(ef);
}

//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_edge_finder_builder::set_from_stream(std::istream &is)
{
  // Cycle through string and produce a map of properties
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  search_ni_=5;
  // Extract the properties
  parse_base_props(props);

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_edge_finder_builder::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_edge_finder_builder::is_a() const
{
  return std::string("mfpf_edge_finder_builder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder_builder* mfpf_edge_finder_builder::clone() const
{
  return new mfpf_edge_finder_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_edge_finder_builder::print_summary(std::ostream& os) const
{
  os << "{ ";
  mfpf_point_finder_builder::print_summary(os);
  os << " }";
}

//: Version number for I/O
short mfpf_edge_finder_builder::version_no() const
{
  return 1;
}

void mfpf_edge_finder_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  mfpf_point_finder_builder::b_write(bfs);  // Save base class
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_edge_finder_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      mfpf_point_finder_builder::b_read(bfs);  // Load base class
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
