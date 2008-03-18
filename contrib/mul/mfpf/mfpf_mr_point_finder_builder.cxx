
//:
// \file
// \author Tim Cootes
// \brief Builder for mfpf_mr_point_finder objects.

#include <vcl_cstdlib.h>
#include <mfpf/mfpf_mr_point_finder_builder.h>
#include <mfpf/mfpf_mr_point_finder.h>

#include <vimt/vimt_image_pyramid.h>
#include <vnl/vnl_math.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_mr_point_finder_builder::mfpf_mr_point_finder_builder()
{
}


//=======================================================================
// Destructor
//=======================================================================

mfpf_mr_point_finder_builder::~mfpf_mr_point_finder_builder()
{
}

//: Define point builders.  Clone of each taken
void mfpf_mr_point_finder_builder::set(
             const vcl_vector<mfpf_point_finder_builder*>& builders)
{
  builders_.resize(builders.size());
  for (unsigned i=0;i<builders.size();++i)
    builders_[i]=*builders[i];  // Clone taken by copy operator
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
  builders_.resize(n);
  for (unsigned i=0;i<n;++i)
  {
    builders_[i]=builder0;  // Clone taken by copy operator
    builder(i).set_step_size(step0*vcl_pow(scale_step,int(i)));
  }

  int dx = vcl_max(1,int(0.99+1.0/scale_step));
  double dA = builder0.search_dA();
  double ds = builder0.search_ds();

  for (int i=n-2;i>=0;--i)  // int because unsigned always >=0
  {
    // Shrink scale/angle step at each stage
    dA*=0.7;
    ds = 1.0+0.7*(ds-1.0);
    builder(i).set_search_area(dx,dx);
    if (builder0.search_nA()>0)
      builder(i).set_search_scale_range(1,ds);
    if (builder0.search_ns()>0)
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
                int min_pixel_width, 
                int max_pixel_width)
{
  double max_w = vcl_max(wi,wj);

  double log_s = vcl_log(scale_step);

  // Estimate level above which size falls below min_pixel_width pixels
  int max_L = int(vcl_log(max_w/min_pixel_width)/log_s);
  // Estimate level below which size is above max_pixel_width pixels
  int min_L = vnl_math_rnd(0.5+vcl_log(max_w/max_pixel_width)/log_s);
  if (min_L>max_L) max_L=min_L;

  double step0 = vcl_pow(scale_step,min_L);
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

  double log_step = vcl_log(im_pyr.scale_step());
  int level = vnl_math_rnd(vcl_log(rel_size0)/log_step);
  if (level<im_pyr.lo()) return im_pyr.lo();
  if (level>im_pyr.hi()) return im_pyr.hi();
  return level;
}

//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_mr_point_finder_builder::clear(unsigned n_egs)
{
  for (unsigned i=0;i<size();++i) builder(i).clear(n_egs);
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
    assert(image_pyr(im_L).is_a()=="vimt_image_2d_of<float>");
    const vimt_image_2d_of<float>& image
      = static_cast<const vimt_image_2d_of<float>&>(image_pyr(im_L));

    builder(L).add_example(image,p,u);
  }
}

//: Build object from the data supplied in add_example()
void mfpf_mr_point_finder_builder::build(mfpf_mr_point_finder& mr_pf)
{
  vcl_vector<mfpf_point_finder*> finders(size());
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

vcl_string mfpf_mr_point_finder_builder::is_a() const
{
  return vcl_string("mfpf_mr_point_finder_builder");
}

//: Print class to os
void mfpf_mr_point_finder_builder::print_summary(vcl_ostream& os) const
{
  os<<vcl_endl;
  unsigned n=builders_.size();
  os<<vsl_indent()<<"n_builders: "<<n<<vcl_endl;
  vsl_indent_inc(os);
  for (unsigned i=0;i<n;i++)
  {
    os<<vsl_indent()<<i<<") ";
    vsl_indent_inc(os);
    os<<builders_[i]<<vcl_endl;
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
  for (unsigned i=0;i<builders_.size();++i)
    vsl_b_write(bfs,builders_[i]);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_mr_point_finder_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  unsigned n;
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,n);
      builders_.resize(n);
      for (unsigned i=0;i<n;++i) vsl_b_read(bfs,builders_[i]);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&) \n";
      vcl_cerr << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }

}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mfpf_mr_point_finder_builder& b)
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


