#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include "mfpf_mr_point_finder_builder.h"
//:
// \file
// \author Tim Cootes
// \brief Builder for mfpf_mr_point_finder objects.

#include <mfpf/mfpf_mr_point_finder.h>

#include <vimt/vimt_image_pyramid.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>

//=======================================================================
// Constructors
//=======================================================================

mfpf_mr_point_finder_builder::mfpf_mr_point_finder_builder() = default;

//: Copy ctor
mfpf_mr_point_finder_builder::mfpf_mr_point_finder_builder(const mfpf_mr_point_finder_builder& b)
{
 *this=b;
}

//: Copy operator
mfpf_mr_point_finder_builder& mfpf_mr_point_finder_builder::operator=(const mfpf_mr_point_finder_builder& b)
{
  if (&b==this) return *this;

  delete_all();

  // Take clones of all builders.
  unsigned n=b.size();
  builders_.resize(n);
  for (unsigned i=0;i<n;++i) builders_[i]=b.builders_[i]->clone();

  return *this;
}

//=======================================================================
// Destructor
//=======================================================================


mfpf_mr_point_finder_builder::~mfpf_mr_point_finder_builder()
{
  delete_all();
}

//: Delete all the builders
void mfpf_mr_point_finder_builder::delete_all()
{
  for (auto & builder : builders_)
    delete builder;
  builders_.resize(0);
}


//: Define point builders.  Clone of each taken
void mfpf_mr_point_finder_builder::set(
             const std::vector<mfpf_point_finder_builder*>& builders)
{
  delete_all();
  builders_.resize(builders.size());
  for (unsigned i=0;i<builders.size();++i)
    builders_[i]=builders[i]->clone();
}

//: Set number of builders. Any existing builders are retained
void mfpf_mr_point_finder_builder::set_n_levels(unsigned n)
{
  unsigned imax=builders_.size();
  builders_.resize(n);
  for (unsigned i=imax;i<n;i++)
    builders_[i]=builders_[imax]->clone();
}

//: Set up n builders, with step size step0*scale_step^L
//  Takes clones of builder and sets up step sizes.
//  Top level search parameters retained.
//  Finer res models have search area and scale/angle
//  ranges set to allow efficient refinement.
void mfpf_mr_point_finder_builder::set(
          const mfpf_point_finder_builder& builder0,
          unsigned n, double step0, double scale_step)
{
  delete_all();
  builders_.resize(n);
  for (unsigned i=0;i<n;++i)
  {
    builders_[i]=builder0.clone();
    builder(i).set_step_size(step0*std::pow(scale_step,int(i)));
  }

  int dx = std::max(1,int(0.99+1.0/scale_step));
  double dA = builder0.search_dA();
  double ds = builder0.search_ds();

  for (int i=n-2;i>=0;--i)  // int because unsigned always >=0
  {
    // Shrink scale/angle step at each stage
    dA*=0.7;
    ds = 1.0+0.7*(ds-1.0);
    builder(i).set_search_area(dx,dx);
    if (builder0.search_ns()>0)
      builder(i).set_search_scale_range(1,ds);
    if (builder0.search_nA()>0)
      builder(i).set_search_angle_range(1,dA);
  }
}

//: Define region size in world co-ordinates
//  Sets up ROI in each model to cover given box (in world coords),
//  with ref point at centre.
void mfpf_mr_point_finder_builder::set_region_size(double wi, double wj)
{
  for (unsigned i=0;i<size();++i)
  {
    builder(i).set_region_size(wi,wj);
  }
}

void mfpf_mr_point_finder_builder::set_size_and_levels(
                const mfpf_point_finder_builder& builder0,
                double wi, double wj,
                double scale_step,
                int min_n_samples,
                int max_n_samples,
                double base_pixel_width)
{
  // Max width in pixel units
  double max_w = std::max(wi/base_pixel_width,wj/base_pixel_width);

  double log_s = std::log(scale_step);

  // Estimate level above which size falls below min_pixel_width pixels
  int max_L = int(std::log(max_w/min_n_samples)/log_s);
  // Estimate level below which size is above max_pixel_width pixels
  int min_L = vnl_math::rnd(0.5+std::log(max_w/max_n_samples)/log_s);
  if (min_L>max_L) max_L=min_L;

  double step0 = base_pixel_width*std::pow(scale_step,min_L);
  int n_levels = 1+max_L-min_L;

  set(builder0,n_levels,step0,scale_step);
  set_region_size(wi,wj);
}


//: Select best level for building model using u as basis
//  Selects pyramid level with pixel sizes best matching
//  the model pixel size at given basis vector u.
unsigned mfpf_mr_point_finder_builder::image_level(
                      unsigned i,
                      const vgl_vector_2d<double>& u,
                      const vimt_image_pyramid& im_pyr) const
{
  double model_pixel_size = builder(i).step_size()*u.length();
  double rel_size0 = model_pixel_size/im_pyr.base_pixel_width();

  double log_step = std::log(im_pyr.scale_step());
  int level = vnl_math::rnd(std::log(rel_size0)/log_step);
  if (level<im_pyr.lo()) return im_pyr.lo();
  if (level>im_pyr.hi()) return im_pyr.hi();
  return level;
}

// Find non-empty image in pyramid closest to given level
static unsigned nearest_valid_level(const vimt_image_pyramid& im_pyr,
                             unsigned level)
{
  int L0=int(level);
  int bestL=0;
  int min_d2=999;
  for (int L=0;L<=im_pyr.hi();++L)
  {
    if (im_pyr(L).image_size()[0]>0)  // This level is not empty
    {
      int d2 = (L-L0)*(L-L0);
      if (d2<min_d2) { min_d2=d2; bestL=L; }
    }
  }
  return unsigned(bestL);
}

//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_mr_point_finder_builder::clear(unsigned n_egs)
{
  for (unsigned i=0;i<size();++i) builder(i).clear(n_egs);
}

//: Get sample image at specified point for level L of the point_finder hierarchy
void mfpf_mr_point_finder_builder::get_sample_vector(
                        const vimt_image_pyramid& image_pyr,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        unsigned L,
                        std::vector<double>& v)
{
  assert( L<size() );

  unsigned im_L = image_level(L,u,image_pyr);

  if (image_pyr(im_L).image_size()[0]==0)
  {
    std::cerr<<"Image at level "<<im_L<<" in pyramid has not been set up.\n"
            <<"This is required for level "<<L<<" of the mfpf model.\n"
            <<"Check range for which pyramid is defined.\n";

    im_L=nearest_valid_level(image_pyr,im_L);
    if (image_pyr(im_L).image_size()[0]==0)
    {
       std::cerr << "No image pyramid levels set up.\n";
       std::abort();
    }
  }

  assert(image_pyr(im_L).is_a()=="vimt_image_2d_of<float>");
  const auto& image
    = static_cast<const vimt_image_2d_of<float>&>(image_pyr(im_L));

  builder(L).get_sample_vector(image,p,u,v);
}

//: Add one example to the model
void mfpf_mr_point_finder_builder::add_example(
                  const vimt_image_pyramid& image_pyr,
                  const vgl_point_2d<double>& p,
                  const vgl_vector_2d<double>& u)
{
  for (unsigned L=0;L<size();++L)
  {
    unsigned im_L = image_level(L,u,image_pyr);

    if (image_pyr(im_L).image_size()[0]==0)
    {
      std::cerr << "Image at level "<<im_L<<" in pyramid has not been set up.\n"
               << "This is required for level "<<L<<" of the mfpf model.\n"
               << "Check range for which pyramid is defined.\n";

      im_L=nearest_valid_level(image_pyr,im_L);
      if (image_pyr(im_L).image_size()[0]==0)
      {
         std::cerr << "No image pyramid levels set up.\n";
         std::abort();
      }
    }

    assert(image_pyr(im_L).is_a()=="vimt_image_2d_of<float>");
    const auto& image
      = static_cast<const vimt_image_2d_of<float>&>(image_pyr(im_L));

    builder(L).add_example(image,p,u);
  }
}

//: Build object from the data supplied in add_example()
void mfpf_mr_point_finder_builder::build(mfpf_mr_point_finder& mr_pf)
{
  std::vector<mfpf_point_finder*> finders(size());
  for (unsigned i=0;i<size();++i)
  {
    finders[i] = builder(i).new_finder();
    builder(i).build(*finders[i]);
  }

  mr_pf.set(finders);

  // Tidy up
  for (unsigned i=0;i<size();++i) delete finders[i];
}

//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_mr_point_finder_builder::version_no() const
{
  return 1;
}


//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_mr_point_finder_builder::is_a() const
{
  return std::string("mfpf_mr_point_finder_builder");
}

//: Print class to os
void mfpf_mr_point_finder_builder::print_summary(std::ostream& os) const
{
  os<<'\n';
  unsigned n=builders_.size();
  os<<vsl_indent()<<"n_builders: "<<n<<'\n';
  vsl_indent_inc(os);
  for (unsigned i=0;i<n;i++)
  {
    os<<vsl_indent()<<i<<") ";
    vsl_indent_inc(os);
    os<<builders_[i]<<'\n';
    vsl_indent_dec(os);
  }
  vsl_indent_dec(os);
}

//=======================================================================
// Method: save
//=======================================================================

void mfpf_mr_point_finder_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,builders_.size());
  for (auto builder : builders_)
    vsl_b_write(bfs,builder);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_mr_point_finder_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  delete_all();

  short version;
  vsl_b_read(bfs,version);
  unsigned n;
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,n);
      builders_.resize(n);
      for (unsigned i=0;i<n;++i)
      {
        builders_[i]=nullptr;
        vsl_b_read(bfs,builders_[i]);
      }
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mfpf_mr_point_finder_builder& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_mr_point_finder_builder& b)
{
  b.b_write(bfs);
}

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_mr_point_finder_builder& b)
{
  b.b_read(bfs);
}
