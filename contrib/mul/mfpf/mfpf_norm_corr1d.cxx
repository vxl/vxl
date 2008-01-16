//:
// \file
// \brief Searches along a profile using normalised correllation
// \author Tim Cootes


#include <mfpf/mfpf_norm_corr1d.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_cloneables_factory.h>

#include <vimt/vimt_bilin_interp.h>
#include <vimt/vimt_sample_profile_bilin.h>
#include <vnl/io/vnl_io_vector.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_norm_corr1d::mfpf_norm_corr1d()
{
  set_defaults();
}

//: Define default values
void mfpf_norm_corr1d::set_defaults()
{
  step_size_=1.0;
  ilo_=1; ihi_=0;
  search_ni_=5;
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_norm_corr1d::~mfpf_norm_corr1d()
{
}

void mfpf_norm_corr1d::set_step_size(double s)
{
  step_size_=s;
}

//: Define filter kernel to search with
void mfpf_norm_corr1d::set(int ilo, int ihi, const vnl_vector<double>& k)
{
  assert(1+ihi-ilo==k.size());
  ilo_=ilo;
  ihi_=ihi;
  kernel_=k;
  // Normalise the vector
  kernel_-=kernel_.mean();
  kernel_.normalize();
}

void mfpf_norm_corr1d::set_search_area(unsigned ni, unsigned)
{
  search_ni_=ni;
}

// Assumes v2[i] has zero mean and unit length as a vector
inline double norm_corr(const double* v1, const double* v2, unsigned n)
{
  double sum1=0.0,sum2=0.0,sum_sq=0.0;
  for (unsigned i=0;i<n;++i)
  {
    sum1+=v1[i]*v2[i];
    sum2+=v1[i];
    sum_sq+=v1[i]*v1[i];
  }
  double mean = sum2/n;
  double ss = vcl_max(1e-6,sum_sq-n*mean*mean);
  double s = vcl_sqrt(ss);
  return sum1/s;
}

//: Evaluate match at p, using u to define scale and orientation 
// Returns -1*edge strength at p along direction u
double mfpf_norm_corr1d::evaluate(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u)
{
  int n=1+ihi_-ilo_;
  vnl_vector<double> v(n);
  vgl_vector_2d<double> u1=step_size_*u;

  const vgl_point_2d<double> p0 = p+ilo_*u1;
  vimt_sample_profile_bilin(v,image,p0,u1,n);

  return -1*norm_corr(v.data_block(),kernel_.data_block(),n);
}

   //: Evaluate match at in a region around p 
   // Returns a qualtity of fit at a set of positions.
   // response image (whose size and transform is set inside the
   // function), indicates the points at which the function was
   // evaluated.  response(i,j) is the fit at the point
// response.world2im().inverse()(i,j).  The world2im() transformation
// may be affine.
void mfpf_norm_corr1d::evaluate_region(
                        const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vimt_image_2d_of<double>& response)
{
  int n=1+2*search_ni_;
  int ns = 2*search_ni_ + 1+ ihi_-ilo_;
  vnl_vector<double> v(ns);
  vgl_vector_2d<double> u1=step_size_*u;
  const vgl_point_2d<double> p0 = p+(ilo_-search_ni_)*u1;
  vimt_sample_profile_bilin(v,image,p0,u1,ns);
  response.image().set_size(n,1);
  double* r = response.image().top_left_ptr();
  for (int i=0;i<n;++i,++r)
  {
    *r = -1*norm_corr(&v[i],kernel_.data_block(),kernel_.size());
  }

  // Set up transformation parameters

  // Point (i,j) in dest corresponds to p1+i.u+j.v,
  // an affine transformation for image to world
  const vgl_point_2d<double> p1 = p-search_ni_*u1;

  vimt_transform_2d i2w;
  i2w.set_similarity(vgl_point_2d<double>(u1.x(),u1.y()),p1);
  response.set_world2im(i2w.inverse());
}

   //: Search given image around p, using u to define scale and orientation 
   //  On exit, new_p and new_u define position, scale and orientation of 
   //  the best nearby match.  Returns a qualtity of fit measure at that
   //  point (the smaller the better).
double mfpf_norm_corr1d::search(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p,
                        vgl_vector_2d<double>& new_u)
{
  int n=1+2*search_ni_;
  int ns = 2*search_ni_ + 1+ ihi_-ilo_;
  vnl_vector<double> v(ns);
  vgl_vector_2d<double> u1=step_size_*u;
  const vgl_point_2d<double> p0 = p+(ilo_-search_ni_)*u1;
  vimt_sample_profile_bilin(v,image,p0,u1,ns);
  int best_i=0;
  double best_r = norm_corr(&v[0],kernel_.data_block(),kernel_.size());
  for (int i=1;i<n;++i)
  {
    double r = norm_corr(&v[i],kernel_.data_block(),kernel_.size());
    if (r>best_r) { best_r=r; best_i=i; }
  }
  new_p = p+(best_i-search_ni_)*u1;
  new_u = u;
  return -1.0 * best_r;
}

//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_norm_corr1d::set_from_stream(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  set_defaults();

  // Extract the properties
  if (props.find("step_size")!=props.end())
  { 
    step_size_=vul_string_atof(props["step_size"]); 
    props.erase("step_size"); 
  }
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
  if (props.find("search_ni")!=props.end())
  { 
    search_ni_=vul_string_atoi(props["search_ni"]); 
    props.erase("search_ni"); 
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_norm_corr1d::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_norm_corr1d::is_a() const
{
  return vcl_string("mfpf_norm_corr1d");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder* mfpf_norm_corr1d::clone() const
{
  return new mfpf_norm_corr1d(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_norm_corr1d::print_summary(vcl_ostream& os) const
{
  os<<"{ step_size: "<<step_size_;
  os<<" size: ["<<ilo_<<","<<ihi_<<"]";
  os<<" search_ni: "<<search_ni_;
  os<<" Kernel: "<<kernel_<<vcl_endl;
  os<<" }";
}

void mfpf_norm_corr1d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,step_size_); 
  vsl_b_write(bfs,ilo_); 
  vsl_b_write(bfs,ihi_); 
  vsl_b_write(bfs,kernel_); 
  vsl_b_write(bfs,search_ni_); 
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_norm_corr1d::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,step_size_);
      vsl_b_read(bfs,ilo_);
      vsl_b_read(bfs,ihi_);
      vsl_b_read(bfs,kernel_);
      vsl_b_read(bfs,search_ni_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&) \n";
      vcl_cerr << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Test equality
bool mfpf_norm_corr1d::operator==(const mfpf_norm_corr1d& nc) const
{
  if (ilo_!=nc.ilo_) return false;
  if (ihi_!=nc.ihi_) return false;
  if (search_ni_!=nc.search_ni_) return false;
  if (vcl_fabs(step_size_-nc.step_size_)>1e-6) return false;
  if (kernel_.size()!=nc.kernel_.size()) return false;
  return (vnl_vector_ssd(kernel_,nc.kernel_)<1e-4);
}


