#include "mfpf_point_finder_builder.h"
//:
// \file
// \brief Base for classes which build mfpf_point_finder objects.
// \author Tim Cootes

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_cloneables_factory.h>
#include <vcl_cmath.h>

#include <mfpf/mfpf_point_finder.h>
#include <vul/vul_string.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_point_finder_builder::mfpf_point_finder_builder()
  : step_size_(1.0),
    search_ni_(5),search_nj_(0),
    search_nA_(0),search_dA_(0.0),search_ns_(0),search_ds_(1.0)
{
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_point_finder_builder::~mfpf_point_finder_builder()
{
}

//: Size of step between sample points
void mfpf_point_finder_builder::set_step_size(double s)
{
  step_size_=s;
}

//: Define search region size
//  During search, samples at points on grid [-ni,ni]x[-nj,nj],
//  with axes defined by u.
void mfpf_point_finder_builder::set_search_area(unsigned ni, unsigned nj)
{
  search_ni_=ni;
  search_nj_=nj;
}

//: Define angle search parameters
void mfpf_point_finder_builder::set_search_angle_range(unsigned nA, double dA)
{
  search_nA_=nA;
  search_dA_=dA;
}

//: Define scale search parameters
void mfpf_point_finder_builder::set_search_scale_range(unsigned ns, double ds)
{
  search_ns_=ns;
  search_ds_=ds;
}


//: Parse relevant parameters from props list
void mfpf_point_finder_builder::parse_base_props(mbl_read_props_type& props)
{
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
  if (props.find("search_nj")!=props.end())
  {
    search_nj_=vul_string_atoi(props["search_nj"]);
    props.erase("search_nj");
  }
  if (props.find("search_nA")!=props.end())
  {
    search_nA_=vul_string_atoi(props["search_nA"]);
    props.erase("search_nA");
  }
  if (props.find("search_ns")!=props.end())
  {
    search_ns_=vul_string_atoi(props["search_ns"]);
    props.erase("search_ns");
  }
  if (props.find("search_dA")!=props.end())
  {
    search_dA_=vul_string_atof(props["search_dA"]);
    props.erase("search_dA");
  }
  if (props.find("search_ds")!=props.end())
  {
    search_ds_=vul_string_atof(props["search_ds"]);
    props.erase("search_ds");
  }

}


//: Set base-class parameters of point finder
void mfpf_point_finder_builder::set_base_parameters(mfpf_point_finder& pf)
{
  pf.set_step_size(step_size_);
  pf.set_search_area(search_ni_,search_nj_);
  pf.set_angle_range(search_nA_,search_dA_);
  pf.set_scale_range(search_ns_,search_ds_);
}

//: Initialise from a string stream
bool mfpf_point_finder_builder::set_from_stream(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  if (props.size()!=0)
  {
    vcl_cerr<<is_a()<<" does not expect any extra arguments.\n";
    mbl_read_props_look_for_unused_props(
      "mfpf_point_finder_builder::set_from_stream", props, mbl_read_props_type());
  }
  return true;
}

//: Create a concrete object, from a text specification.
vcl_auto_ptr<mfpf_point_finder_builder> mfpf_point_finder_builder::
  create_from_stream(vcl_istream &is)
{
  vcl_string name;
  is >> name;
  vcl_auto_ptr<mfpf_point_finder_builder> opt;
  try {
    opt = mbl_cloneables_factory<mfpf_point_finder_builder>::get_clone(name);
  }
  catch (const mbl_exception_no_name_in_factory & e)
  {
    throw (mbl_exception_parse_error( e.what() ));
  }
  opt->set_from_stream(is);
  return opt;
}


//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_point_finder_builder::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_point_finder_builder::is_a() const
{
  return vcl_string("mfpf_point_finder_builder");
}

//: Return true if base class parameters are the same in pf
bool mfpf_point_finder_builder::base_equality(const mfpf_point_finder_builder& pf) const
{
  if (search_ni_!=pf.search_ni_) return false;
  if (search_nj_!=pf.search_nj_) return false;
  if (search_nA_!=pf.search_nA_) return false;
  if (search_ns_!=pf.search_ns_) return false;
  if (vcl_fabs(search_dA_-pf.search_dA_)>1e-6) return false;
  if (vcl_fabs(search_ds_-pf.search_ds_)>1e-6) return false;
  if (vcl_fabs(step_size_-pf.step_size_)>1e-6) return false;
  return true;
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_point_finder_builder::print_summary(vcl_ostream& os) const
{
  os<<" step_size: "<<step_size_;
  os<<" search: { ni: "<<search_ni_;
  os<<" nj: "<<search_nj_;
  os<<" nA: "<<search_nA_<<" dA: "<<search_dA_;
  os<<" ns: "<<search_ns_<<" ds: "<<search_ds_<<" } ";
}

//=======================================================================
// Binary I/O
//=======================================================================

void mfpf_point_finder_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,step_size_); 
  vsl_b_write(bfs,search_ni_); 
  vsl_b_write(bfs,search_nj_); 
  vsl_b_write(bfs,search_nA_); 
  vsl_b_write(bfs,search_dA_); 
  vsl_b_write(bfs,search_ns_); 
  vsl_b_write(bfs,search_ds_); 
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_point_finder_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,step_size_);
      vsl_b_read(bfs,search_ni_);
      vsl_b_read(bfs,search_nj_);
      vsl_b_read(bfs,search_nA_); 
      vsl_b_read(bfs,search_dA_); 
      vsl_b_read(bfs,search_ns_); 
      vsl_b_read(bfs,search_ds_); 
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&) \n";
      vcl_cerr << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_point_finder_builder& b)
{
  vsl_binary_loader<mfpf_point_finder_builder>::instance().add(b);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mfpf_point_finder_builder& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mfpf_point_finder_builder& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mfpf_point_finder_builder& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mfpf_point_finder_builder* b)
{
  if (b)
    return os << *b;
  else
    return os << "No mfpf_point_finder_builder defined.";
}
