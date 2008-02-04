//:
// \file
// \brief Builder for mfpf_norm_corr2d objects.
// \author Tim Cootes


#include <mfpf/mfpf_norm_corr2d_builder.h>
#include <mfpf/mfpf_norm_corr2d.h>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>

#include <vil/vil_resample_bilin.h>
#include <vil/vil_math.h>
#include <vil/io/vil_io_image_view.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_norm_corr2d_builder::mfpf_norm_corr2d_builder()
{
  set_defaults();
}

//: Define default values
void mfpf_norm_corr2d_builder::set_defaults()
{
  step_size_=1.0;
  ni_=7; nj_=7;
  ref_x_=0.5*(ni_-1);
  ref_y_=0.5*(nj_-1);
  search_ni_=5;
  search_nj_=5;
  nA_=0;
  dA_=0.0;
}

void mfpf_norm_corr2d_builder::set_step_size(double s)
{
  step_size_=s;
}


//=======================================================================
// Destructor
//=======================================================================

mfpf_norm_corr2d_builder::~mfpf_norm_corr2d_builder()
{
}

//: Create new mfpf_norm_corr2d on heap
mfpf_point_finder* mfpf_norm_corr2d_builder::new_finder() const
{
  return new mfpf_norm_corr2d();
}

void mfpf_norm_corr2d_builder::set_kernel_size(unsigned ni, unsigned nj,
                                               double ref_x, double ref_y)
{
  ni_=ni; nj_=nj;
  ref_x_=ref_x;
  ref_y_=ref_y;
}

void mfpf_norm_corr2d_builder::set_kernel_size(unsigned ni, unsigned nj)
{
  ni_=ni; nj_=nj;
  ref_x_=0.5*(ni_-1);
  ref_y_=0.5*(nj_-1);
}


//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_norm_corr2d_builder::clear(unsigned n_egs)
{
  n_added_=0;
}

static void normalize(vil_image_view<double>& im)
{
  unsigned ni=im.ni(),nj=im.nj();
  double sum=0.0,ss=0.0;
  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
    {
      sum+=im(i,j); ss+=im(i,j)*im(i,j);
    }

  // Normalise so that im has zero mean and unit sum of squares.
  double mean=sum/(ni*nj);
  ss-=(mean*mean*ni*nj);
  double s=1.0;
  if (ss>0) s = vcl_sqrt(1.0/ss);
  vil_math_scale_and_offset_values(im,s,-s*mean);
}

//: Add one example to the model
void mfpf_norm_corr2d_builder::add_one_example(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u)
{
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());

  vil_image_view<double> sample;

  const vgl_point_2d<double> p0 = p-ref_x_*u1-ref_y_*v1;

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> im_p0 = s_w2i(p0);
  vgl_vector_2d<double> im_u = s_w2i.delta(p0, u1);
  vgl_vector_2d<double> im_v = s_w2i.delta(p0, v1);

  vil_resample_bilin(image.image(),sample,
                      im_p0.x(),im_p0.y(),  im_u.x(),im_u.y(),
                      im_v.x(),im_v.y(),ni_,nj_);

  normalize(sample);
  if (n_added_==0) sum_.deep_copy(sample);
  else             vil_math_add_image_fraction(sum_,1.0,sample,1.0);
  n_added_++;
}

//: Add one example to the model
void mfpf_norm_corr2d_builder::add_example(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u)
{
  if (nA_==0)
  {
    add_example(image,p,u);
    return;
  }

  vgl_vector_2d<double> v(-u.y(),u.x());
  for (int iA=-int(nA_);iA<=nA_;++iA)
  {
    double A = iA*dA_;
    vgl_vector_2d<double> uA = u*vcl_cos(A)+v*vcl_sin(A);
    add_example(image,p,uA);
  }
}

//: Build this object from the data supplied in add_example()
void mfpf_norm_corr2d_builder::build(mfpf_point_finder& pf)
{
  assert(pf.is_a()=="mfpf_norm_corr2d");
  mfpf_norm_corr2d& nc = static_cast<mfpf_norm_corr2d&>(pf);
  nc.set_search_area(search_ni_,search_nj_);
  vil_image_view<double> mean;
  mean.deep_copy(sum_);
  vil_math_scale_values(mean,1.0/n_added_);
  normalize(mean);
  nc.set(mean,ref_x_,ref_y_);
}

//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_norm_corr2d_builder::set_from_stream(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  set_defaults();

  // Extract the properties
  if (props.find("ni")!=props.end())
  {
    ni_=vul_string_atoi(props["ni"]);
    props.erase("ni");
  }
  if (props.find("nj")!=props.end())
  {
    nj_=vul_string_atoi(props["nj"]);
    props.erase("nj");
  }

  if (props.find("ref_x")!=props.end())
  {
    ref_x_=vul_string_atof(props["ref_x"]);
    props.erase("ref_x");
  }
  else ref_x_=0.5*(ni_-1.0);

  if (props.find("ref_y")!=props.end())
  {
    ref_y_=vul_string_atof(props["ref_y"]);
    props.erase("ref_y");
  }
  else ref_y_=0.5*(nj_-1.0);

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

  if (props.find("nA")!=props.end())
  {
    nA_=vul_string_atoi(props["nA"]);
    props.erase("nA");
  }

  if (props.find("dA")!=props.end())
  {
    dA_=vul_string_atof(props["dA"]);
    props.erase("dA");
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_norm_corr2d_builder::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_norm_corr2d_builder::is_a() const
{
  return vcl_string("mfpf_norm_corr2d_builder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder_builder* mfpf_norm_corr2d_builder::clone() const
{
  return new mfpf_norm_corr2d_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_norm_corr2d_builder::print_summary(vcl_ostream& os) const
{
  os << "{ step_size: " << step_size_
     << " size: " << ni_ << 'x' << nj_
     << " search_ni: " << search_ni_
     << " search_nj: " << search_nj_
     << " nA: " << nA_ << " dA: " << dA_
     << '}';
}

void mfpf_norm_corr2d_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,step_size_);
  vsl_b_write(bfs,ni_);
  vsl_b_write(bfs,nj_);
  vsl_b_write(bfs,ref_x_);
  vsl_b_write(bfs,ref_y_);
  vsl_b_write(bfs,sum_);
  vsl_b_write(bfs,n_added_);
  vsl_b_write(bfs,search_ni_);
  vsl_b_write(bfs,search_nj_);
  vsl_b_write(bfs,nA_);
  vsl_b_write(bfs,dA_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_norm_corr2d_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,step_size_);
      vsl_b_read(bfs,ni_);
      vsl_b_read(bfs,nj_);
      vsl_b_read(bfs,ref_x_);
      vsl_b_read(bfs,ref_y_);
      vsl_b_read(bfs,sum_);
      vsl_b_read(bfs,n_added_);
      vsl_b_read(bfs,search_ni_);
      vsl_b_read(bfs,search_nj_);
      vsl_b_read(bfs,nA_);
      vsl_b_read(bfs,dA_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

