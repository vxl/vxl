//:
// \file
// \brief Builder for mfpf_edge_finder objects.
// \author Tim Cootes


#include <mfpf/mfpf_edge_finder_builder.h>
#include <mfpf/mfpf_edge_finder.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <vcl_cmath.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_cloneables_factory.h>

#include <vimt/vimt_bilin_interp.h>
#include <vimt/vimt_sample_profile_bilin.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_edge_finder_builder::mfpf_edge_finder_builder()
  : step_size_(1.0),search_ni_(5)
{
}

void mfpf_edge_finder_builder::set_step_size(double s)
{
  step_size_=s;
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_edge_finder_builder::~mfpf_edge_finder_builder()
{
}

//: Create new mfpf_edge_finder on heap
mfpf_point_finder* mfpf_edge_finder_builder::new_finder() const
{
  return new mfpf_edge_finder();
}

//: Initialise building
// Must be called before any calls to add_example(...) 
void mfpf_edge_finder_builder::clear(unsigned n_egs)
{
}

//: Add one example to the model
void mfpf_edge_finder_builder::add_example(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u)
{
}

//: Build this object from the data supplied in add_example()
void mfpf_edge_finder_builder::build(mfpf_point_finder& pf)
{
  assert(pf.is_a()=="mfpf_edge_finder");
  mfpf_edge_finder& ef = static_cast<mfpf_edge_finder&>(pf);
  ef.set_search_area(search_ni_,0);
}

//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_edge_finder_builder::set_from_stream(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  search_ni_=5;
  // Extract the properties
  if (props.find("step_size")!=props.end())
  { 
    step_size_=vul_string_atof(props["step_size"]); 
    props.erase("step_size"); 
  }
  if (props.find("search_ni")!=props.end())
  { 
    search_ni_=vul_string_atoi(props["search_ni"]); 
    props.erase("search_ni"); 
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_edge_finder_builder::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_edge_finder_builder::is_a() const
{
  return vcl_string("mfpf_edge_finder_builder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder_builder* mfpf_edge_finder_builder::clone() const
{
  return new mfpf_edge_finder_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_edge_finder_builder::print_summary(vcl_ostream& os) const
{
  os<<"{ step_size: "<<step_size_;
  os<<" search_ni: "<<search_ni_;
  os<<" }";
}

void mfpf_edge_finder_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,search_ni_); 
  vsl_b_write(bfs,step_size_); 
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
      vsl_b_read(bfs,step_size_);
      vsl_b_read(bfs,search_ni_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&) \n";
      vcl_cerr << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

