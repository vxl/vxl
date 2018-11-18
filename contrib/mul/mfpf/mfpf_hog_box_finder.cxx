#include <cmath>
#include <iostream>
#include <algorithm>
#include "mfpf_hog_box_finder.h"
//:
// \file
// \brief Searches for rectangular region using HOG features
// \author Tim Cootes

#include <vsl/vsl_binary_loader.h>
#include <cassert>

#include <vil/vil_resample_bilin.h>
#include <vil/io/vil_io_image_view.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_indent.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

#include <mipa/mipa_orientation_histogram.h>
#include <mipa/mipa_sample_histo_boxes.h>
#include <mipa/mipa_identity_normaliser.h>
#include <mipa/mipa_block_normaliser.h>

//: Divide elements of v by sum of last nA elements
//  For histogram vectors these are the total sums
inline void mfpf_norm_histo_vec(vnl_vector<double>& v, unsigned nA)
{
  unsigned n=v.size();
  double sum = 0.0;
  for (unsigned i=1;i<=nA;++i) sum+=v[n-i];
  v/=sum;
}

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_hog_box_finder::mfpf_hog_box_finder():normaliser_(mipa_identity_normaliser())
{
  set_defaults();
}

//: Define default values
void mfpf_hog_box_finder::set_defaults()
{
  step_size_=1.0;
  search_ni_=5;
  search_nj_=5;
  nA_=0; dA_=0.0;
  ns_=0; ds_=1.0;

  nA_bins_=8;
  full360_=true;
  nc_=2;
  ni_=0;
  nj_=0;
  ref_x_=0;
  ref_y_=0;
//  norm_method_=0;
  overlap_f_=1.0;
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_hog_box_finder::~mfpf_hog_box_finder() = default;

//: Define region and cost of region
void mfpf_hog_box_finder::set(unsigned nA_bins, bool full360,
                              unsigned ni, unsigned nj, unsigned nc,
                              double ref_x, double ref_y,
                              const mfpf_vec_cost& cost,
                              const mbl_cloneable_nzptr<mipa_vector_normaliser>& normaliser)
{
  cost_ = cost.clone();
  ref_x_ = ref_x;
  ref_y_ = ref_y;

  nA_bins_ = nA_bins;
  full360_ = full360;
  ni_      = ni;
  nj_      = nj;
  nc_      = nc;

  normaliser_ = normaliser;

  //: Block normalisers (and their derivatives) typically need their regions copying from this
  mipa_vector_normaliser* pNormaliser=normaliser_.ptr();
  auto* pBlockNormaliser= dynamic_cast<mipa_block_normaliser*>(pNormaliser);
  if (pBlockNormaliser)
  {
      pBlockNormaliser->set_region(2*ni_,2*nj_);
      pBlockNormaliser->set_nbins(nA_bins_);
  }
  //assert(norm_method>=0 && norm_method<=1);
  //norm_method_ = norm_method;
}

//: Relative size of region used for estimating overlap
//  If 0.5, then overlap requires pt inside central 50% of region.
void mfpf_hog_box_finder::set_overlap_f(double f)
{
  overlap_f_=f;
}


//: Radius of circle containing modelled region
double mfpf_hog_box_finder::radius() const
{
  // Compute distance to each corner
  double wx = ni_-1;
  double x2 = std::max(ref_x_*ref_x_,(ref_x_-wx)*(ref_x_-wx));
  double wy = nj_-1;
  double y2 = std::max(ref_y_*ref_y_,(ref_y_-wy)*(ref_y_-wy));
  double r2 = x2+y2;
  if (r2<=1) return 1.0;
  return nc_*std::sqrt(r2);
}


//: Evaluate match at p, using u to define scale and orientation
double mfpf_hog_box_finder::evaluate(const vimt_image_2d_of<float>& image,
                                     const vgl_point_2d<double>& p,
                                     const vgl_vector_2d<double>& u)
{
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());

  assert(image.image().nplanes()==1);

  // Set up sample area with 1 unit border
  unsigned sni = 2+2*nc_*ni_;
  unsigned snj = 2+2*nc_*nj_;
  vil_image_view<float> sample(sni,snj);

  const vgl_point_2d<double> p0 = p-(1+nc_*ref_x_)*u1-(1+nc_*ref_y_)*v1;

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> im_p0 = s_w2i(p0);
  vgl_vector_2d<double> im_u = s_w2i.delta(p0, u1);
  vgl_vector_2d<double> im_v = s_w2i.delta(p0, v1);

  vil_resample_bilin(image.image(),sample,
                     im_p0.x(),im_p0.y(),  im_u.x(),im_u.y(),
                     im_v.x(),im_v.y(),
                     sni,snj);

  vil_image_view<float> histo_im;
  mipa_orientation_histogram(sample,histo_im,nA_bins_,nc_,full360_);

  vnl_vector<double> v;
  mipa_sample_histo_boxes_3L(histo_im,0,0,v,ni_,nj_);

  normaliser_->normalise(v);
  //if (norm_method_==1) mfpf_norm_histo_vec(v,nA_bins_);


  return cost().evaluate(v);
}

//: Evaluate match at in a region around p
// Returns a quality of fit at a set of positions.
// response image (whose size and transform is set inside the
// function), indicates the points at which the function was
// evaluated.  response(i,j) is the fit at the point
// response.world2im().inverse()(i,j).  The world2im() transformation
// may be affine.
void mfpf_hog_box_finder::evaluate_region(
                        const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vimt_image_2d_of<double>& response)
{
  // Note: search_ni/nj defined in units of u
  // However, search occurs in units of nc*u due to histogram pooling.
  // So work out how many steps that is, rounding up
  unsigned sni = (nc_+search_ni_-1)/nc_;
  unsigned snj = (nc_+search_nj_-1)/nc_;

  // Total size of region to sample
  int nsi = 2*nc_*(sni + ni_)+2;
  int nsj = 2*nc_*(snj + nj_)+2;

  assert(image.image().nplanes()==1);
  // Set up sample area with 1 unit border
  vil_image_view<float> sample(2+nsi,2+nsj);
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());
  const vgl_point_2d<double> p0 = p-(1+nc_*(sni+ref_x_))*u1
                                   -(1+nc_*(snj+ref_y_))*v1;

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> im_p0 = s_w2i(p0);
  vgl_vector_2d<double> im_u = s_w2i.delta(p0, u1);
  vgl_vector_2d<double> im_v = s_w2i.delta(p0, v1);

  // Sample region of interest
  vil_resample_bilin(image.image(),sample,
                     im_p0.x(),im_p0.y(),  im_u.x(),im_u.y(),
                     im_v.x(),im_v.y(),
                     nsi,nsj);

  // Compute image of histograms (each cell is pool of nc x nc)
  vil_image_view<float> histo_im;
  mipa_orientation_histogram(sample,histo_im,nA_bins_,nc_,full360_);

  vnl_vector<double> v;

  int ni=1+2*sni;
  int nj=1+2*snj;
  response.image().set_size(ni,nj);
  double* r = response.image().top_left_ptr();
  std::ptrdiff_t r_jstep = response.image().jstep();

  for (unsigned j=0;j<(unsigned)nj;++j,r+=r_jstep)
  {
    for (int i=0;i<ni;++i)
    {
      mipa_sample_histo_boxes_3L(histo_im,i,j,v,ni_,nj_);
      //if (norm_method_==1) mfpf_norm_histo_vec(v,nA_bins_);
      normaliser_->normalise(v);
      r[i] = cost().evaluate(v);
      if (vnl_math::isnan(r[i]))
      {
        std::cerr<<is_a()<<"::evaluate_region: Response is NaN.\n"
                <<*this<<'\n'
                <<"i,j="<<i<<','<<j<<'\n'
                <<"v.sum()="<<v.sum()<<'\n';
        std::abort();
      }
    }
  }

  // Set up transformation parameters

  // Point (i,j) in resp corresponds to p1+nc.i.u+nc.j.v,
  // an affine transformation for image to world
  const vgl_point_2d<double> p1 = p-nc_*sni*u1-nc_*snj*v1;

  vimt_transform_2d i2w;
  i2w.set_similarity(vgl_point_2d<double>(nc_*u1.x(),nc_*u1.y()),p1);
  response.set_world2im(i2w.inverse());
}

//: Search given image around p, using u to define scale and orientation
//  On exit, new_p and new_u define position, scale and orientation of
//  the best nearby match.  Returns a quality of fit measure at that
//  point (the smaller the better).
double mfpf_hog_box_finder::search_one_pose(const vimt_image_2d_of<float>& image,
                                            const vgl_point_2d<double>& p,
                                            const vgl_vector_2d<double>& u,
                                            vgl_point_2d<double>& new_p)
{
  // Note: search_ni/nj defined in units of u
  // However, search occurs in units of nc*u due to histogram pooling.
  // So work out how many steps that is, rounding up
  unsigned sni = (nc_+search_ni_-1)/nc_;
  unsigned snj = (nc_+search_nj_-1)/nc_;

  // Total size of region to sample
  int nsi = 2*nc_*(sni + ni_)+2;
  int nsj = 2*nc_*(snj + nj_)+2;

  assert(image.image().nplanes()==1);
  // Set up sample area with 1 unit border
  vil_image_view<float> sample(2+nsi,2+nsj);
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());
  const vgl_point_2d<double> p0 = p-(1+nc_*(sni+ref_x_))*u1
                                   -(1+nc_*(snj+ref_y_))*v1;

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> im_p0 = s_w2i(p0);
  vgl_vector_2d<double> im_u = s_w2i.delta(p0, u1);
  vgl_vector_2d<double> im_v = s_w2i.delta(p0, v1);

  // Sample region of interest
  vil_resample_bilin(image.image(), sample,
                     im_p0.x(),im_p0.y(), im_u.x(),im_u.y(), im_v.x(),im_v.y(),
                     nsi, nsj);

  // Compute image of histograms (each cell is pool of nc x nc)
  vil_image_view<float> histo_im;
  mipa_orientation_histogram(sample,histo_im,nA_bins_,nc_,full360_);

  vnl_vector<double> v;

  unsigned ni=1+2*sni;
  unsigned nj=1+2*snj;

  double best_r=9.99e9;
  int best_i=0,best_j=0;
  for (unsigned j=0;j<nj;++j)
  {
    for (unsigned int i=0;i<ni;++i)
    {
      mipa_sample_histo_boxes_3L(histo_im,i,j,v,ni_,nj_);
      //if (norm_method_==1) mfpf_norm_histo_vec(v,nA_bins_);
      normaliser_->normalise(v);
      double r = cost().evaluate(v);
      if (r<best_r) { best_r=r; best_i=i; best_j=j; }
    }
  }

  // Compute position of best point
  new_p = p+nc_*(best_i-sni)*u1+nc_*(best_j-snj)*v1;
  return best_r;
}

// Returns true if p is inside region at given pose
bool mfpf_hog_box_finder::is_inside(const mfpf_pose& pose,
                                    const vgl_point_2d<double>& p,
                                    double f) const
{
  // Set transform model frame -> World
  vimt_transform_2d t1;
  t1.set_similarity(2*nc_*step_size()*pose.u(),pose.p());
  // Compute position of p in model frame
  vgl_point_2d<double> q=t1.inverse()(p);
  q.x()/=f; q.y()/=f;  // To check that q in the central fraction f
  q.x()+=ref_x_;
  if (q.x()<0 || q.x()>(ni_-1)) return false;
  q.y()+=ref_y_;
  if (q.y()<0 || q.y()>(nj_-1)) return false;
  return true;
}

//: Return true if modelled regions at pose1 and pose2 overlap
//  Checks if reference point of one is inside region of other
bool mfpf_hog_box_finder::overlap(const mfpf_pose& pose1,
                                  const mfpf_pose& pose2) const
{
  if (is_inside(pose1,pose2.p(),overlap_f_)) return true;
  if (is_inside(pose2,pose1.p(),overlap_f_)) return true;
  return false;
}

//: Generate points in ref frame that represent boundary
//  Points of a contour around the shape.
//  Used for display purposes.
void mfpf_hog_box_finder::get_outline(std::vector<vgl_point_2d<double> >& pts) const
{
  double s=2*nc_;
  pts.resize(7);
  vgl_vector_2d<double> r(s*ref_x_,s*ref_y_);
  pts[0]=vgl_point_2d<double>(0,s*nj_)-r;
  pts[1]=vgl_point_2d<double>(0,0);
  pts[2]=vgl_point_2d<double>(s*ni_,s*nj_)-r;
  pts[3]=vgl_point_2d<double>(0,s*nj_)-r;
  pts[4]=vgl_point_2d<double>(0,0)-r;
  pts[5]=vgl_point_2d<double>(s*ni_,0)-r;
  pts[6]=vgl_point_2d<double>(s*ni_,s*nj_)-r;
}


//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_hog_box_finder::is_a() const
{
  return std::string("mfpf_hog_box_finder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder* mfpf_hog_box_finder::clone() const
{
  return new mfpf_hog_box_finder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_hog_box_finder::print_summary(std::ostream& os) const
{
  os << "{ "<<'\n';
  vsl_indent_inc(os);
  os << vsl_indent()<<"size: " << ni_ << 'x' << nj_
     << " nc: " << nc_ <<" nA_bins: "<<nA_bins_
     << " ref_pt: (" << ref_x_ << ',' << ref_y_ << ')' <<'\n';
  if (full360_) os << vsl_indent()<<"Angle range: 0-360"<<'\n';
  else          os << vsl_indent()<<"Angle range: 0-180"<<'\n';
#if 0
  if (norm_method_==0) os << vsl_indent()<<"norm: none"<<'\n';
  else                 os << vsl_indent()<<"norm: linear"<<'\n';
#endif

  std::cout<<"The HOG's normaliser is:"<<std::endl;
  normaliser_->print_summary(os);

  os << vsl_indent()<< "cost: ";
  if (cost_.ptr()==nullptr) os << "--"<<std::endl; else os << cost_<<'\n';
  os << vsl_indent();
  mfpf_point_finder::print_summary(os);
  os << '\n'
     << vsl_indent()<<"overlap_f: "<<overlap_f_<<'\n';
  vsl_indent_dec(os);
  os << vsl_indent()<<'}';
}

short mfpf_hog_box_finder::version_no() const
{
  return 2;
}


void mfpf_hog_box_finder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  mfpf_point_finder::b_write(bfs);  // Save base class
  vsl_b_write(bfs,nc_);
  vsl_b_write(bfs,ni_);
  vsl_b_write(bfs,nj_);
  vsl_b_write(bfs,nA_bins_);
  vsl_b_write(bfs,full360_);
  vsl_b_write(bfs,cost_);
  vsl_b_write(bfs,ref_x_);
  vsl_b_write(bfs,ref_y_);
  vsl_b_write(bfs,normaliser_);
  vsl_b_write(bfs,overlap_f_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_hog_box_finder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
    case (2):
      mfpf_point_finder::b_read(bfs);  // Load in base class
      vsl_b_read(bfs,nc_);
      vsl_b_read(bfs,ni_);
      vsl_b_read(bfs,nj_);
      vsl_b_read(bfs,nA_bins_);
      vsl_b_read(bfs,full360_);
      vsl_b_read(bfs,cost_);
      vsl_b_read(bfs,ref_x_);
      vsl_b_read(bfs,ref_y_);
      vsl_b_read(bfs,normaliser_);
      if (version==1) overlap_f_=1.0;
      else            vsl_b_read(bfs,overlap_f_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Test equality
bool mfpf_hog_box_finder::operator==(const mfpf_hog_box_finder& nc) const
{
  if (!base_equality(nc)) return false;
  if (nc_!=nc.nc_) return false;
  if (ni_!=nc.ni_) return false;
  if (nj_!=nc.nj_) return false;
  if (normaliser_->is_a()!=nc.normaliser_->is_a()) return false; //bit looser than true equality
  if (nA_bins_!=nc.nA_bins_) return false;
  if (full360_!=nc.full360_) return false;
  if (std::fabs(ref_x_-nc.ref_x_)>1e-6) return false;
  if (std::fabs(ref_y_-nc.ref_y_)>1e-6) return false;
  if (std::fabs(overlap_f_-nc.overlap_f_)>1e-6) return false;
  // Strictly should compare costs
  return true;
}
