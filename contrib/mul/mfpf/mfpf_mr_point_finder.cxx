#include "mfpf_mr_point_finder.h"
//:
// \file
// \author Tim Cootes
// \brief Multi-res point finder.  Searches at range of scales.

#include <mfpf/mfpf_prune_overlaps.h>

#include <vimt/vimt_image_pyramid.h>
#include <vnl/vnl_math.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_mr_point_finder::mfpf_mr_point_finder()
{
}


//=======================================================================
// Destructor
//=======================================================================

mfpf_mr_point_finder::~mfpf_mr_point_finder()
{
}

//: Define point finders.  Clone of each taken
void mfpf_mr_point_finder::set(const vcl_vector<mfpf_point_finder*>& finders)
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
  double model_pixel_size = finder(i).step_size()*pose.scale();
  double rel_size0 = model_pixel_size/im_pyr.base_pixel_width();

  double log_step = vcl_log(im_pyr.scale_step());
  int level = vnl_math_rnd(vcl_log(rel_size0)/log_step);
  if (level<im_pyr.lo()) return im_pyr.lo();
  if (level>im_pyr.hi()) return im_pyr.hi();
  return level;
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
    const vimt_image_2d_of<float>& image
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
    const vimt_image_2d_of<float>& image
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
  const vimt_image_2d_of<float>& image
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
                  vcl_vector<mfpf_pose>& poses,
                  vcl_vector<double>& fits)
{
  poses.resize(0); fits.resize(0);

  // Search for multiple responses at coarsest scale
  int L=size()-1;
  unsigned im_L = image_level(L,pose0,im_pyr);
  assert(im_pyr(im_L).is_a()=="vimt_image_2d_of<float>");
  const vimt_image_2d_of<float>& image
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
                    vcl_vector<mfpf_pose>& poses,
                    vcl_vector<double>& fits,
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
  const vimt_image_2d_of<float>& image
    = static_cast<const vimt_image_2d_of<float>&>(im_pyr(im_L));
  finder(L0).multi_search(image,pose0.p(),pose0.u(),poses,fits);

  if (L0==prune_level)
    mfpf_prune_overlaps(finder(L0),poses,fits);

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
      mfpf_prune_overlaps(finder(L),poses,fits);
    }
  }
}

//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_mr_point_finder::version_no() const
{
  return 1;
}


//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_mr_point_finder::is_a() const
{
  return vcl_string("mfpf_mr_point_finder");
}

//: Print class to os
void mfpf_mr_point_finder::print_summary(vcl_ostream& os) const
{
  os<<vcl_endl;
  unsigned n=finders_.size();
  os<<vsl_indent()<<"n_finders: "<<n<<vcl_endl;
  vsl_indent_inc(os);
  for (unsigned i=0;i<n;i++)
  {
    os<<vsl_indent()<<i<<") ";
    vsl_indent_inc(os);
    os<<finders_[i]<<vcl_endl;
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
  for (unsigned i=0;i<finders_.size();++i)
    vsl_b_write(bfs,finders_[i]);
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
      vsl_b_read(bfs,n);
      finders_.resize(n);
      for (unsigned i=0;i<n;++i) vsl_b_read(bfs,finders_[i]);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mfpf_mr_point_finder& b)
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


