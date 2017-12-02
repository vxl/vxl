#include <iostream>
#include <cmath>
#include "mfpf_region_finder.h"
//:
// \file
// \brief Searches for an arbitrary shaped region
// \author Tim Cootes

#include <vsl/vsl_binary_loader.h>
#include <vcl_compiler.h>
#include <vcl_cassert.h>

#include <vil/vil_resample_bilin.h>
#include <vil/io/vil_io_image_view.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_indent.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <mfpf/mfpf_sample_region.h>
#include <mfpf/mfpf_norm_vec.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_c_vector.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_region_finder::mfpf_region_finder()
{
  set_defaults();
}

//: Define default values
void mfpf_region_finder::set_defaults()
{
  step_size_=1.0;
  search_ni_=5;
  search_nj_=5;
  nA_=0; dA_=0.0;
  ns_=0; ds_=1.0;
  n_pixels_=0;
  roi_.resize(0);
  roi_ni_=0;
  roi_nj_=0;
  ref_x_=0;
  ref_y_=0;
  norm_method_=1;
  overlap_f_=1.0;
  var_min_ = 1.0E-6;
  draw_only_1st_plane_=true;
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_region_finder::~mfpf_region_finder()
{
}

//: Define region and cost of region
void mfpf_region_finder::set(const std::vector<mbl_chord>& roi,
                             double ref_x, double ref_y,
                             const mfpf_vec_cost& cost,
                             short norm_method)
{
  cost_ = cost.clone();
  ref_x_ = ref_x;
  ref_y_ = ref_y;

  // Check bounding box
  if (roi.size()==0) { roi_ni_=0; roi_nj_=0; return; }
  int ilo=roi[0].start_x(), ihi=roi[0].end_x();
  int jlo=roi[0].y(), jhi=roi[0].y();

  for (unsigned k=1;k<roi.size();++k)
  {
    if (roi[k].start_x()<ilo) ilo=roi[k].start_x();
    if (roi[k].end_x()>ihi)   ihi=roi[k].end_x();
    if (roi[k].y()<jlo) jlo=roi[k].y();
    if (roi[k].y()>jhi) jhi=roi[k].y();
  }
  roi_ni_=1+ihi-ilo;
  roi_nj_=1+jhi-jlo;

  // Apply offset of (-ilo,-jlo) to ensure bounding box is +ive
  ref_x_-=ilo; ref_y_-=jlo;
  roi_.resize(roi.size());
  n_pixels_=0;
  for (unsigned k=0;k<roi.size();++k)
  {
    roi_[k]= mbl_chord(roi[k].start_x()-ilo,
                       roi[k].end_x()-ilo,   roi[k].y()-jlo);
    n_pixels_+=1+roi[k].end_x()-roi[k].start_x();
  }

  assert(norm_method>=0 && norm_method<=1);
  norm_method_ = norm_method;
}

//: Relative size of region used for estimating overlap
//  If 0.5, then overlap requires pt inside central 50% of region.
void mfpf_region_finder::set_overlap_f(double f)
{
  overlap_f_=f;
}


//: Radius of circle containing modelled region
double mfpf_region_finder::radius() const
{
  // Compute distance to each corner
  double wx = roi_ni_-1;
  double x2 = std::max(ref_x_*ref_x_,(ref_x_-wx)*(ref_x_-wx));
  double wy = roi_nj_-1;
  double y2 = std::max(ref_y_*ref_y_,(ref_y_-wy)*(ref_y_-wy));
  double r2 = x2+y2;
  if (r2<=1) return 1.0;
  return std::sqrt(r2);
}


//: Evaluate match at p, using u to define scale and orientation
// Returns -1*edge strength at p along direction u
double mfpf_region_finder::evaluate(const vimt_image_2d_of<float>& image,
                                    const vgl_point_2d<double>& p,
                                    const vgl_vector_2d<double>& u)
{
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());

  unsigned np=image.image().nplanes();
  // Set up sample area with interleaved planes (ie planestep==1)
  vil_image_view<float> sample(roi_ni_,roi_nj_,1,np);

  const vgl_point_2d<double> p0 = p-ref_x_*u1-ref_y_*v1;

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> im_p0 = s_w2i(p0);
  vgl_vector_2d<double> im_u = s_w2i.delta(p0, u1);
  vgl_vector_2d<double> im_v = s_w2i.delta(p0, v1);

  vil_resample_bilin(image.image(),sample,
                     im_p0.x(),im_p0.y(),
                     im_u.x(),im_u.y(),
                     im_v.x(),im_v.y(),
                     roi_ni_,roi_nj_);

  vnl_vector<double> v(n_pixels_*sample.nplanes());
  mfpf_sample_region(sample.top_left_ptr(),sample.jstep(),
                     np,roi_,v);

  if (norm_method_==1) mfpf_norm_vec(v,var_min_);
  return cost().evaluate(v);
}

//: Evaluate match at in a region around p
// Returns a quality of fit at a set of positions.
// response image (whose size and transform is set inside the
// function), indicates the points at which the function was
// evaluated.  response(i,j) is the fit at the point
// response.world2im().inverse()(i,j).  The world2im() transformation
// may be affine.
void mfpf_region_finder::evaluate_region(
                        const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vimt_image_2d_of<double>& response)
{
  int ni=1+2*search_ni_;
  int nj=1+2*search_nj_;
  int nsi = 2*search_ni_ + roi_ni_;
  int nsj = 2*search_nj_ + roi_nj_;

  unsigned np=image.image().nplanes();
  // Set up sample area with interleaved planes (ie planestep==1)
  vil_image_view<float> sample(nsi,nsj,1,np);
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());
  const vgl_point_2d<double> p0 = p-(search_ni_+ref_x_)*u1
                                   -(search_nj_+ref_y_)*v1;

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> im_p0 = s_w2i(p0);
  vgl_vector_2d<double> im_u = s_w2i.delta(p0, u1);
  vgl_vector_2d<double> im_v = s_w2i.delta(p0, v1);

  vil_resample_bilin(image.image(),sample,
                     im_p0.x(),im_p0.y(),
                     im_u.x(),im_u.y(),
                     im_v.x(),im_v.y(),
                     nsi,nsj);

  vnl_vector<double> v(n_pixels_*np);

  response.image().set_size(ni,nj);
  double* r = response.image().top_left_ptr();
  const float* s = sample.top_left_ptr();
  std::ptrdiff_t r_jstep = response.image().jstep();
  std::ptrdiff_t s_jstep = sample.jstep();

  for (unsigned j=0;j<(unsigned)nj;++j,r+=r_jstep,s+=s_jstep)
  {
    for (int i=0;i<ni;++i)
    {
      mfpf_sample_region(s+i*np,s_jstep,np,roi_,v);
      if (norm_method_==1) mfpf_norm_vec(v,var_min_);
      r[i] = cost().evaluate(v);
    }
  }

  // Set up transformation parameters

  // Point (i,j) in dest corresponds to p1+i.u+j.v,
  // an affine transformation for image to world
  const vgl_point_2d<double> p1 = p-search_ni_*u1-search_nj_*v1;

  vimt_transform_2d i2w;
  i2w.set_similarity(vgl_point_2d<double>(u1.x(),u1.y()),p1);
  response.set_world2im(i2w.inverse());
}

   //: Search given image around p, using u to define scale and orientation
   //  On exit, new_p and new_u define position, scale and orientation of
   //  the best nearby match.  Returns a quality of fit measure at that
   //  point (the smaller the better).
double mfpf_region_finder::search_one_pose(const vimt_image_2d_of<float>& image,
                                           const vgl_point_2d<double>& p,
                                           const vgl_vector_2d<double>& u,
                                           vgl_point_2d<double>& new_p)
{
  int ni=1+2*search_ni_;
  int nj=1+2*search_nj_;
  int nsi = 2*search_ni_ + roi_ni_;
  int nsj = 2*search_nj_ + roi_nj_;

  unsigned np=image.image().nplanes();
  // Set up sample area with interleaved planes (ie planestep==1)
  vil_image_view<float> sample(nsi,nsj,1,np);
  vgl_vector_2d<double> u1=step_size_*u;
  vgl_vector_2d<double> v1(-u1.y(),u1.x());
  const vgl_point_2d<double> p0 = p-(search_ni_+ref_x_)*u1
                                   -(search_nj_+ref_y_)*v1;

  const vimt_transform_2d& s_w2i = image.world2im();
  vgl_point_2d<double> im_p0 = s_w2i(p0);
  vgl_vector_2d<double> im_u = s_w2i.delta(p0, u1);
  vgl_vector_2d<double> im_v = s_w2i.delta(p0, v1);

  vil_resample_bilin(image.image(),sample,
                     im_p0.x(),im_p0.y(),  im_u.x(),im_u.y(),
                     im_v.x(),im_v.y(),
                     nsi,nsj);

  vnl_vector<double> v(n_pixels_*np);

  const float* s = sample.top_left_ptr();
  std::ptrdiff_t s_jstep = sample.jstep();

  double best_r=9.99e9;
  int best_i=0,best_j=0;
  for (unsigned j=0;j<(unsigned)nj;++j,s+=s_jstep)
  {
    for (int i=0;i<ni;++i)
    {
      mfpf_sample_region(s+i*np,s_jstep,np,roi_,v);
      if (norm_method_==1) mfpf_norm_vec(v,var_min_);
      double r = cost().evaluate(v);
      if (r<best_r) { best_r=r; best_i=i; best_j=j; }
    }
  }

  // Compute position of best point
  new_p = p+(best_i-search_ni_)*u1+(best_j-search_nj_)*v1;
  return best_r;
}

// Returns true if p is inside region at given pose
bool mfpf_region_finder::is_inside(const mfpf_pose& pose,
                                   const vgl_point_2d<double>& p,
                                   double f) const
{
  // Set transform model frame -> World
  vimt_transform_2d t1;
  t1.set_similarity(step_size()*pose.u(),pose.p());
  // Compute position of p in model frame
  vgl_point_2d<double> q=t1.inverse()(p);
  q.x()/=f; q.y()/=f;  // To check that q in the central fraction f
  q.x()+=ref_x_;
  if (q.x()<0 || q.x()>(roi_ni_-1)) return false;
  q.y()+=ref_y_;
  if (q.y()<0 || q.y()>(roi_nj_-1)) return false;
  return true;
}

//: Return true if modelled regions at pose1 and pose2 overlap
//  Checks if reference point of one is inside region of other
bool mfpf_region_finder::overlap(const mfpf_pose& pose1,
                                 const mfpf_pose& pose2) const
{
  if (is_inside(pose1,pose2.p(),overlap_f_)) return true;
  if (is_inside(pose2,pose1.p(),overlap_f_)) return true;
  return false;
}

//: Generate points in ref frame that represent boundary
//  Points of a contour around the shape.
//  Used for display purposes.
void mfpf_region_finder::get_outline(std::vector<vgl_point_2d<double> >& pts) const
{
  pts.resize(7);
  vgl_vector_2d<double> r(ref_x_,ref_y_);
  pts[0]=vgl_point_2d<double>(0,roi_nj_)-r;
  pts[1]=vgl_point_2d<double>(0,0);
  pts[2]=vgl_point_2d<double>(roi_ni_,roi_nj_)-r;
  pts[3]=vgl_point_2d<double>(0,roi_nj_)-r;
  pts[4]=vgl_point_2d<double>(0,0)-r;
  pts[5]=vgl_point_2d<double>(roi_ni_,0)-r;
  pts[6]=vgl_point_2d<double>(roi_ni_,roi_nj_)-r;
}

//: Return an image of the region of interest
void mfpf_region_finder::get_image_of_model(vimt_image_2d_of<vxl_byte>& image) const
{
  vnl_vector<double> mean;
  cost().get_average(mean);

  assert(mean.size()>=n_pixels_);

  // Just copy first plane
  vnl_vector<double> meanL2;
  unsigned nplanes=mean.size()/n_pixels_;

  double min1=1.0E30;
  double max1=-1.0E30;
  if (nplanes==1)
  {
    min1=mean.min_value();
    max1=mean.max_value();
  }
  else if (draw_only_1st_plane_)
  {
      double* pData=mean.data_block();
      double* pDataEnd=mean.data_block()+mean.size();
      while (pData != pDataEnd)
      {
          double z=*pData;
          min1=std::min(z,min1);
          max1=std::max(z,max1);
          pData += nplanes;
      }
  }
  else //Compute L2 norm over all planes
  {
      meanL2.set_size(n_pixels_);
      double* pData=mean.data_block();
      double* pDataEnd=mean.data_block()+mean.size();
      unsigned i=0;

      while (pData != pDataEnd)
      {
          double z = vnl_c_vector<double>::two_norm(pData, nplanes);

          min1=std::min(z,min1);
          max1=std::max(z,max1);
          meanL2[i++]=z;
          pData += nplanes;
      }
  }

  double s =255/(max1-min1);
  image.image().set_size(roi_ni_,roi_nj_);
  image.image().fill(0);
  unsigned q=0;
  if (nplanes==1)
  {
    for (unsigned k=0;k<roi_.size();++k)
    {
      for (int i=roi_[k].start_x();i<=roi_[k].end_x();++i,++q)
        image.image()(i,roi_[k].y())=vxl_byte(s*(mean[q]-min1));
    }
  }
  else if (draw_only_1st_plane_)
  {
    for (unsigned k=0;k<roi_.size();++k)
    {
      for (int i=roi_[k].start_x();i<=roi_[k].end_x();++i,q+=nplanes)
        image.image()(i,roi_[k].y())=vxl_byte(s*(mean[q]-min1));
    }
  }
  else
  {
      //Compute L2 norm over all planes
    for (unsigned k=0;k<roi_.size();++k)
    {
      for (int i=roi_[k].start_x();i<=roi_[k].end_x();++i,++q)
        image.image()(i,roi_[k].y())=vxl_byte(s*(meanL2[q]-min1));
    }
  }

  vimt_transform_2d ref2im;
  ref2im.set_zoom_only(1.0/step_size_,ref_x_,ref_y_);
  image.set_world2im(ref2im);
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_region_finder::is_a() const
{
  return std::string("mfpf_region_finder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder* mfpf_region_finder::clone() const
{
  return new mfpf_region_finder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_region_finder::print_summary(std::ostream& os) const
{
  os << "{  size: "<<roi_ni_<<" x "<<roi_nj_
     << " n_pixels: "<<n_pixels_
     << " ref_pt: ("<<ref_x_<<','<<ref_y_<<')'<<'\n';
  vsl_indent_inc(os);
  if (norm_method_==0) os<<vsl_indent()<<"norm: none"<<'\n';
  else                 os<<vsl_indent()<<"norm: linear"<<'\n';
  os <<vsl_indent()<< "cost: ";
  if (cost_.ptr()==VXL_NULLPTR) os << "--"<<std::endl; else os << cost_<<'\n';
  os<<vsl_indent();
  mfpf_point_finder::print_summary(os);
  os <<std::endl <<vsl_indent()<<"overlap_f: "<<overlap_f_<<'\n';
  vsl_indent_dec(os);
  os<<vsl_indent()<<'}';
}

void mfpf_region_finder::print_shape(std::ostream& os) const
{
  vil_image_view<vxl_byte> im(roi_ni_,roi_nj_);
  im.fill(0);
  for (unsigned k=0;k<roi_.size();++k)
    for (int i=roi_[k].start_x();i<=roi_[k].end_x();++i)
      im(i,roi_[k].y())=1;
  for (unsigned j=0;j<im.nj();++j)
  {
    for (unsigned i=0;i<im.ni();++i)
      if (im(i,j)==0) os<<' ';
      else            os<<'X';
    os<<'\n';
  }
}

short mfpf_region_finder::version_no() const
{
  return 2;
}


void mfpf_region_finder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  mfpf_point_finder::b_write(bfs);  // Save base class
  vsl_b_write(bfs,roi_);
  vsl_b_write(bfs,roi_ni_);
  vsl_b_write(bfs,roi_nj_);
  vsl_b_write(bfs,n_pixels_);
  vsl_b_write(bfs,cost_);
  vsl_b_write(bfs,ref_x_);
  vsl_b_write(bfs,ref_y_);
  vsl_b_write(bfs,norm_method_);
  vsl_b_write(bfs,overlap_f_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_region_finder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
    case (2):
      mfpf_point_finder::b_read(bfs);  // Load in base class
      vsl_b_read(bfs,roi_);
      vsl_b_read(bfs,roi_ni_);
      vsl_b_read(bfs,roi_nj_);
      vsl_b_read(bfs,n_pixels_);
      vsl_b_read(bfs,cost_);
      vsl_b_read(bfs,ref_x_);
      vsl_b_read(bfs,ref_y_);
      vsl_b_read(bfs,norm_method_);
      if (version==1) overlap_f_=1.0;
      else            vsl_b_read(bfs,overlap_f_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Test equality
bool mfpf_region_finder::operator==(const mfpf_region_finder& nc) const
{
  if (!base_equality(nc)) return false;
  if (roi_ni_!=nc.roi_ni_) return false;
  if (roi_nj_!=nc.roi_nj_) return false;
  if (norm_method_!=nc.norm_method_) return false;
  if (n_pixels_!=nc.n_pixels_) return false;
  if (std::fabs(ref_x_-nc.ref_x_)>1e-6) return false;
  if (std::fabs(ref_y_-nc.ref_y_)>1e-6) return false;
  if (std::fabs(overlap_f_-nc.overlap_f_)>1e-6) return false;
  // Strictly should compare costs
  return true;
}


