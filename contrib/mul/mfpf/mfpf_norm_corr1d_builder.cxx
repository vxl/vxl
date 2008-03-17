//:
// \file
// \brief Builder for mfpf_norm_corr1d objects.
// \author Tim Cootes


#include <mfpf/mfpf_norm_corr1d_builder.h>
#include <mfpf/mfpf_norm_corr1d.h>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>

#include <vimt/vimt_bilin_interp.h>
#include <vimt/vimt_sample_profile_bilin.h>
#include <vnl/io/vnl_io_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_norm_corr1d_builder::mfpf_norm_corr1d_builder()
{
  set_defaults();
}

//: Define default values
void mfpf_norm_corr1d_builder::set_defaults()
{
  step_size_=1.0;
  ilo_=-4; ihi_=4;
  search_ni_=5;
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_norm_corr1d_builder::~mfpf_norm_corr1d_builder()
{
}

//: Create new mfpf_norm_corr1d on heap
mfpf_point_finder* mfpf_norm_corr1d_builder::new_finder() const
{
  return new mfpf_norm_corr1d();
}

void mfpf_norm_corr1d_builder::set_kernel_size(int ilo, int ihi)
{
  ilo_=ilo;
  ihi_=ihi;
}

//: Define region size in world co-ordinates
//  Sets up ROI to cover given box (with samples at step_size()), 
//  with ref point at centre.
void mfpf_norm_corr1d_builder::set_region_size(double wi, double)
{
  wi/=step_size();
  int ni = vcl_max(1,int(0.99+wi));
  set_kernel_size(-ni,ni);
}


//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_norm_corr1d_builder::clear(unsigned n_egs)
{
  n_added_=0;
}

//: Add one example to the model
void mfpf_norm_corr1d_builder::add_example(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u)
{
  int n=1+ihi_-ilo_;
  vnl_vector<double> v(n);
  vgl_vector_2d<double> u1=step_size_*u;
  const vgl_point_2d<double> p0 = p+ilo_*u1;
  vimt_sample_profile_bilin(v,image,p0,u1,n);
  v-=v.mean();
  v.normalize();
  if (n_added_==0) sum_=v;
  else             sum_+=v;
  n_added_++;
}

//: Build this object from the data supplied in add_example()
void mfpf_norm_corr1d_builder::build(mfpf_point_finder& pf)
{
  assert(pf.is_a()=="mfpf_norm_corr1d");
  mfpf_norm_corr1d& nc = static_cast<mfpf_norm_corr1d&>(pf);
  vnl_vector<double> mean=sum_/n_added_;
  mean.normalize();
  nc.set(ilo_,ihi_,mean);
  set_base_parameters(nc);
}

//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_norm_corr1d_builder::set_from_stream(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  set_defaults();

  // Extract the properties
  parse_base_props(props);

  if (props.find("ilo")!=props.end())
  {
    ilo_=vul_string_atoi(props["ilo"]);
    props.erase("ilo");
  }
  if (props.find("ihi")!=props.end())
  {
    ihi_=vul_string_atoi(props["ihi"]);
    props.erase("ihi");
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_norm_corr1d_builder::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_norm_corr1d_builder::is_a() const
{
  return vcl_string("mfpf_norm_corr1d_builder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder_builder* mfpf_norm_corr1d_builder::clone() const
{
  return new mfpf_norm_corr1d_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_norm_corr1d_builder::print_summary(vcl_ostream& os) const
{
  os << "{ size: [" << ilo_ << ',' << ihi_ << ']' <<vcl_endl;
  mfpf_point_finder_builder::print_summary(os);
  os << " }";
}

//: Version number for I/O
short mfpf_norm_corr1d_builder::version_no() const
{
  return 1;
}


void mfpf_norm_corr1d_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  mfpf_point_finder_builder::b_write(bfs);  // Save base class
  vsl_b_write(bfs,ilo_);
  vsl_b_write(bfs,ihi_);
  vsl_b_write(bfs,sum_);
  vsl_b_write(bfs,n_added_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_norm_corr1d_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      mfpf_point_finder_builder::b_read(bfs);  // Load base class
      vsl_b_read(bfs,ilo_);
      vsl_b_read(bfs,ihi_);
      vsl_b_read(bfs,sum_);
      vsl_b_read(bfs,n_added_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

