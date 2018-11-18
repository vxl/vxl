#include <cmath>
#include <iostream>
#include <algorithm>
#include "mfpf_pose_predictor_builder.h"
//:
// \file
// \brief Trains regressor in an mfpf_pose_predictor
// \author Tim Cootes

#include <vsl/vsl_binary_loader.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/algo/vnl_svd.h>

#include <vsl/vsl_indent.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_pose_predictor_builder::mfpf_pose_predictor_builder()
{
  set_defaults();
}

//: Define default values
void mfpf_pose_predictor_builder::set_defaults()
{
  n_per_eg_=25;
  rand_.reseed(57392);
}

//: Define number of samples per training image
void mfpf_pose_predictor_builder::set_n_per_eg(unsigned n)
{
  n_per_eg_=n;
}


//=======================================================================
// Destructor
//=======================================================================

mfpf_pose_predictor_builder::~mfpf_pose_predictor_builder() = default;

//: Define sampling region and method
//  Supplied predictor is partially initialised
void mfpf_pose_predictor_builder::set_sampling(const mfpf_pose_predictor& pp)
{
  sampler_=pp;
}

//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_pose_predictor_builder::clear(unsigned n_egs)
{
  // Assume one plane image at first
  samples_.set_size(n_egs*n_per_eg_,sampler_.n_pixels()+1);

  unsigned nd=2;
  switch (sampler_.pose_type())
  {
    case translation: nd=2; break;
    case rigid:       nd=3; break;
    case zoom:        nd=3; break;
    case similarity:  nd=4; break;
    default: assert(!"Unknown pose_type"); break;
  }

  poses_.set_size(n_egs*n_per_eg_,nd);

  ci_ = 0;
}

//: Add one example to the model
void mfpf_pose_predictor_builder::add_example(
                          const vimt_image_2d_of<float>& image,
                          const mfpf_pose& pose0)
{
  double max_disp = 0.5*sampler_.radius()*sampler_.step_size();
  vnl_vector<double> vec;
  mfpf_pose dpose;
  for (unsigned i=0;i<n_per_eg_;++i,++ci_)
  {
  // Initial version hard coded for translation

    double dx = 0;
    double dy = 0;
    double s=0.0;
    double A=0.0;
    if (i>0)
    {
      dx=rand_.drand64(-max_disp,max_disp);
      dy=rand_.drand64(-max_disp,max_disp);
    }
    poses_(ci_,0)=dx;
    poses_(ci_,1)=dy;

    double max_dA_ = 0.25;  // radians
    double max_ds = std::log(1.2);

    switch (sampler_.pose_type())
    {
      case translation:
        break;
      case rigid:
        if (i>0) A = rand_.drand64(-max_dA_,max_dA_);
        poses_(ci_,2)=A;
        break;
      case zoom:
        if (i>0) s = rand_.drand64(-max_ds,max_ds);
        poses_(ci_,2)=s;
        break;
      case similarity:
        if (i>0)
        {
          A = rand_.drand64(-max_dA_,max_dA_);
          s = rand_.drand64(-max_ds,max_ds);
        }
        poses_(ci_,2)=s;
        poses_(ci_,3)=A;
        break;
      default:
        assert(!"Unknown pose_type");
        break;
    }

    dpose=mfpf_pose(dx,dy,std::exp(s)*std::cos(A),
                          std::exp(s)*std::sin(A));

    mfpf_pose pose = pose0*dpose;
    sampler_.get_sample_vector(image,pose.p(),pose.u(),vec);
    assert(vec.size()+1==samples_.cols());
    samples_(ci_,0)=1;
    for (unsigned j=0;j<vec.size();++j) samples_(ci_,1+j)=vec[j];
  }
}

//: Build object from the data supplied in add_example()
void mfpf_pose_predictor_builder::build(mfpf_pose_predictor& p)
{
  unsigned nv = poses_.cols();
  vnl_svd<double> svd(samples_);

  // Need to solve samples_*R=poses_ for R
  //                 (ns * np)(np*nv) = (ns * nv)

  unsigned n_samples = samples_.rows();
  if (n_samples>3*samples_.cols())
  {
    // Lots more samples than pixels
    vnl_matrix<double> R(nv,samples_.cols()-1);
    vnl_vector<double> r0(nv);
    for (unsigned i=0;i<nv;++i)
    {
      // Inefficient:
      vnl_vector<double> r = svd.solve(poses_.get_column(i));
      r0[i]=r[0];
      for (unsigned j=1;j<r.size();++j) R(i,j-1)=r[j];
    }
    p=sampler_;  // Define sampling
    p.set_predictor(R,r0);  // Define learned predictor
  }
  else
  {
    // May not be enough samples to train properly, so
    // use a reduced dimensional subspace
    unsigned rank = std::max(nv,unsigned(samples_.cols()/3));
    vnl_matrix<double> R1 = svd.pinverse(rank)*poses_;
    unsigned np=R1.rows();
    p=sampler_;  // Define sampling
    // Define learned predictor
    p.set_predictor(R1.extract(np-1,nv,1,0).transpose(),R1.get_row(0));
  }
}


//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_pose_predictor_builder::is_a() const
{
  return std::string("mfpf_pose_predictor_builder");
}

//: Create a copy on the heap and return base class pointer
mfpf_pose_predictor_builder* mfpf_pose_predictor_builder::clone() const
{
  return new mfpf_pose_predictor_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_pose_predictor_builder::print_summary(std::ostream& os) const
{
  os << "{  sampler: "<<sampler_ << '\n'
     << vsl_indent() << '}';
}

short mfpf_pose_predictor_builder::version_no() const
{
  return 1;
}


void mfpf_pose_predictor_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,sampler_);
  vsl_b_write(bfs,n_per_eg_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_pose_predictor_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,sampler_);
      vsl_b_read(bfs,n_per_eg_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Test equality
bool mfpf_pose_predictor_builder::operator==(const mfpf_pose_predictor_builder& nc) const
{
  if (!(sampler_==nc.sampler_)) return false;
  if (n_per_eg_!=nc.n_per_eg_) return false;
  return true;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mfpf_pose_predictor_builder& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mfpf_pose_predictor_builder& b)
{
    b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mfpf_pose_predictor_builder& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}
