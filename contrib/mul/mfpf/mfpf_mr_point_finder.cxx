#include <iostream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include "mfpf_mr_point_finder.h"
//:
// \file
// \author Tim Cootes
// \brief Multi-res point finder.  Searches at range of scales.

#include <mfpf/mfpf_prune_overlaps.h>

#include <vimt/vimt_image_pyramid.h>
#include <vil/vil_save.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_mr_point_finder::mfpf_mr_point_finder()
  : max_after_pruning_(0)
{
}


//=======================================================================
// Destructor
//=======================================================================

mfpf_mr_point_finder::~mfpf_mr_point_finder() = default;

//: Maximum number of candidates to retain during multi_search_and_prune
//  If zero, then refine all.
void mfpf_mr_point_finder::set_max_after_pruning(unsigned max_n)
{
  max_after_pruning_=max_n;
}

//: Define point finders.  Clone of each taken
void mfpf_mr_point_finder::set(const std::vector<mfpf_point_finder*>& finders)
{
  finders_.resize(finders.size());
  for (unsigned i=0;i<finders.size();++i)
    finders_[i]=*finders[i];  // Clone taken by copy operator
}

//: Select best level for searching around pose with finder i
//  Selects pyramid level with pixel sizes best matching
//  the model pixel size at given pose.
unsigned mfpf_mr_point_finder::image_level(
                      unsigned i, const mfpf_pose& pose,
                      const vimt_image_pyramid& im_pyr) const
{
  return finder(i).image_level(pose,im_pyr);
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

//: Get sample image at specified point for level L of the point_finder hierarchy
void mfpf_mr_point_finder::get_sample_vector(
                        const vimt_image_pyramid& image_pyr,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        unsigned L,
                        std::vector<double>& v)
{
  assert( L<finders_.size() );

  unsigned im_L = image_level(L,mfpf_pose(p,u),image_pyr);

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

  finders_[L]->get_sample_vector(image,p,u,v);
}

//: Searches around given pose, starting at coarsest model.
//  Searches with coarsest model, and feeds best result into
//  search for next model.  Result can be further improved
//  by a call to refine()
double mfpf_mr_point_finder::search(const vimt_image_pyramid& im_pyr,
                                    const mfpf_pose& pose0,
                                    mfpf_pose& best_pose)
{
  mfpf_pose pose=pose0;
  double fit = 9e99; // initialize to a "bad" value; in case iteration is empty

  // First search at coarsest level
  for (int L = size()-1; L>=0;--L) // use int 'cos unsigned always>0!
  {
    unsigned im_L = image_level(L,pose0,im_pyr);
    assert(im_pyr(im_L).is_a()=="vimt_image_2d_of<float>");
    const auto& image
      = static_cast<const vimt_image_2d_of<float>&>(im_pyr(im_L));
    fit = finder(L).search_with_opt(image,pose.p(),pose.u(),
                           best_pose.p(),best_pose.u());
    pose=best_pose;
  }

  return fit;
}

//: Searches around given pose, starting at coarsest model.
//  Searches with finder(L_hi) and feeds best result into
//  search for next model, until level L_lo.
//  Result can be further improved by a call to refine_match()
double mfpf_mr_point_finder::mr_search(
                   const vimt_image_pyramid& im_pyr,
                   mfpf_pose& pose, int L_lo, int L_hi)
{
  mfpf_pose pose0=pose;
  double fit = 9e99; // initialize to a "bad" value; in case iteration is empty

  assert(L_hi>=L_lo);

  // First search at coarsest level
  for (int L = L_hi; L>=L_lo;--L) // use int 'cos unsigned is always >= 0!
  {
    unsigned im_L = image_level(L,pose0,im_pyr);
    assert(im_pyr(im_L).is_a()=="vimt_image_2d_of<float>");
    const auto& image
      = static_cast<const vimt_image_2d_of<float>&>(im_pyr(im_L));
    fit = finder(L).search_with_opt(image,pose0.p(),pose0.u(),
                                    pose.p(),pose.u());
    pose0=pose;
  }

  return fit;
}


//: Perform local optimisation to refine position,scale and angle
//  Uses finder(L) to do refinement.
void mfpf_mr_point_finder::refine_match(
                  const vimt_image_pyramid& im_pyr,
                  mfpf_pose& pose, double& fit, unsigned L)
{
  unsigned im_L = image_level(L,pose,im_pyr);
  assert(im_pyr(im_L).is_a()=="vimt_image_2d_of<float>");
  const auto& image
    = static_cast<const vimt_image_2d_of<float>&>(im_pyr(im_L));
  finder(L).refine_match(image,pose.p(),pose.u(),fit);
}

//: Find all local optima at coarsest scale and search around each
//  Runs search at coarsest resolution, to find all local optima.
//  If multiple angles/scales considered, the there may be many
//  nearby responses.
//  Each candidate is then localised by searching at finer and
//  finer resolutions.
//  Final responses may be further improved with refine_match()
void mfpf_mr_point_finder::multi_search(
                  const vimt_image_pyramid& im_pyr,
                  const mfpf_pose& pose0,
                  std::vector<mfpf_pose>& poses,
                  std::vector<double>& fits)
{
  poses.resize(0); fits.resize(0);

  // Search for multiple responses at coarsest scale
  int L=size()-1;
  unsigned im_L = image_level(L,pose0,im_pyr);
  assert(im_pyr(im_L).is_a()=="vimt_image_2d_of<float>");
  const auto& image
    = static_cast<const vimt_image_2d_of<float>&>(im_pyr(im_L));
  finder(L).multi_search(image,pose0.p(),pose0.u(),poses,fits);

  if (L==0) return;

  // Now search around each one
  for (unsigned i=0;i<poses.size();++i)
  {
    fits[i] = mr_search(im_pyr,poses[i],0,L-1);
  }
}

//: Find all non-overlapping local optima.
//  Runs search at coarsest resolution, to find all local optima.
//  If multiple angles/scales considered, the there may be many
//  nearby responses.
//  Each candidate is then localised by searching at finer and
//  finer resolutions.
//  After searching to level prune_level, overlapping responses
//  are pruned (best match in any overlapping group is retained).
//  prune_level is defined modulo size(), so -1 is equivalent to
//  pruning at the coarsest level (size()-1).
//  Final responses may be further improved with refine_match().
void mfpf_mr_point_finder::multi_search_and_prune(
                    const vimt_image_pyramid& im_pyr,
                    const mfpf_pose& pose0,
                    std::vector<mfpf_pose>& poses,
                    std::vector<double>& fits,
                    int prune_level)
{
  poses.resize(0); fits.resize(0);

  // Force prune_level into range [0,size()]
  prune_level=(prune_level+size())%size();
  if (prune_level<0) prune_level+=size();

  // Search for multiple responses at coarsest scale
  int L0=size()-1;
  unsigned im_L = image_level(L0,pose0,im_pyr);
  assert(im_pyr(im_L).is_a()=="vimt_image_2d_of<float>");
  const auto& image
    = static_cast<const vimt_image_2d_of<float>&>(im_pyr(im_L));
  finder(L0).multi_search(image,pose0.p(),pose0.u(),poses,fits);

  if (poses.size()==0)
  {
    std::cerr<<"Warning: No poses returned by mfpf_point_finder\n";
    // Perform search to find single good point
    vgl_point_2d<double> new_p;
    double f = finder(L0).search_one_pose(image,pose0.p(),pose0.u(),new_p);
    poses.resize(1); poses[0]=mfpf_pose(new_p,pose0.u());
    fits.resize(1); fits[0]=f;
  }

  if (L0==prune_level)
    mfpf_prune_and_sort_overlaps(finder(L0),poses,fits,max_after_pruning_);
//    mfpf_prune_overlaps(finder(L0),poses,fits);

  if (L0==0) return;

  for (int L=L0-1;L>=0;--L)
  {
    // Perform local search one each pose
    for (unsigned i=0;i<poses.size();++i)
    {
      fits[i] = mr_search(im_pyr,poses[i],L,L);
    }

    // Remove overlaps if we are at prune_level
    if (L==prune_level)
    {
      mfpf_prune_and_sort_overlaps(finder(L),poses,fits,max_after_pruning_);
//      mfpf_prune_overlaps(finder(L),poses,fits);
    }
  }
}

//: Save an image summarising each model in the hierarchy
//  Saves images to basepath_L0.png, basepath_L1.png ...
void mfpf_mr_point_finder::save_images_of_models(const std::string& basepath) const
{
  for (unsigned L=0;L<size();++L)
  {
    std::stringstream s;
    s<<basepath<<"_L"<<L<<".png";
    vimt_image_2d_of<vxl_byte> image;
    finder(L).get_image_of_model(image);
    if (vil_save(image.image(),s.str().c_str()))
      std::cout<<"Saved image to "<<s.str()<<std::endl;
    else
      std::cout<<"Failed to save image to "<<s.str()<<std::endl;
  }
}


//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_mr_point_finder::version_no() const
{
  return 2;
}


//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_mr_point_finder::is_a() const
{
  return std::string("mfpf_mr_point_finder");
}

//: Print class to os
void mfpf_mr_point_finder::print_summary(std::ostream& os) const
{
  os<<'\n';
  unsigned n=finders_.size();
  os<<vsl_indent()<<"n_finders: "<<n<<'\n';
  vsl_indent_inc(os);
  for (unsigned i=0;i<n;i++)
  {
    os<<vsl_indent()<<i<<") ";
    vsl_indent_inc(os);
    os<<finders_[i]<<'\n';
    vsl_indent_dec(os);
  }
  vsl_indent_dec(os);
}

//=======================================================================
// Method: save
//=======================================================================

void mfpf_mr_point_finder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,finders_.size());
  for (const auto & finder : finders_)
    vsl_b_write(bfs,finder);
  vsl_b_write(bfs,max_after_pruning_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_mr_point_finder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  unsigned n;
  switch (version)
  {
    case (1):
    case (2):
      vsl_b_read(bfs,n);
      finders_.resize(n);
      for (unsigned i=0;i<n;++i) vsl_b_read(bfs,finders_[i]);
      if (version==1) max_after_pruning_=0;
      else vsl_b_read(bfs,max_after_pruning_);
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

std::ostream& operator<<(std::ostream& os,const mfpf_mr_point_finder& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_mr_point_finder& b)
{
  b.b_write(bfs);
}

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_mr_point_finder& b)
{
  b.b_read(bfs);
}
