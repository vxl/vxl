#ifndef mfpf_mr_point_finder_h_
#define mfpf_mr_point_finder_h_
//:
// \file
// \author Tim Cootes
// \brief Multi-res point finder.  Searches at range of scales.

#include <mbl/mbl_cloneable_ptr.h>
#include <mfpf/mfpf_point_finder.h>
#include <vcl_cassert.h>
#include <vcl_iosfwd.h>

class vimt_image_pyramid;

//: Multi-res point finder.
// Contains a set of mfpf_point_finders, each trained at a different
// resolution.  Contains search algorithms to take advantage of this.
class mfpf_mr_point_finder
{
 protected:

  //: Set of cost function objects.
  vcl_vector<mbl_cloneable_ptr<mfpf_point_finder> > finders_;

 public:

    //: Dflt ctor
  mfpf_mr_point_finder();

  //: Destructor
  virtual ~mfpf_mr_point_finder();

  //: Number of finders
  unsigned size() const { return finders_.size(); }

  //: Point finder at level L
  const mfpf_point_finder& finder(unsigned L) const
  { assert (L<finders_.size()); return *finders_[L]; }

  //: Point finder at level L
  mfpf_point_finder& finder(unsigned L)
  { assert (L<finders_.size()); return *finders_[L]; }

  //: Define point finders.  Clone of each taken
  void set(const vcl_vector<mfpf_point_finder*>& finders);

  //: Select best level for searching around pose with finder i
  //  Selects pyramid level with pixel sizes best matching
  //  the model pixel size at given pose.
  unsigned image_level(unsigned i, const mfpf_pose& pose,
                       const vimt_image_pyramid& im_pyr) const;

  //: Searches around given pose, starting at coarsest model.
  //  Searches with coarsest model, and feeds best result into
  //  search for next model.  Result can be further improved
  //  by a call to refine_match().
  //
  //  Returns match fit with finder(0) at best_pose.
  double search(const vimt_image_pyramid& im_pyr,
                const mfpf_pose& pose0,
                mfpf_pose& best_pose);

  //: Searches around given pose, starting at coarsest model.
  //  Searches with finder(L_hi) and feeds best result into
  //  search for next model, until level L_lo.
  //  Result can be further improved by a call to refine_match()
  double mr_search(const vimt_image_pyramid& im_pyr,
                   mfpf_pose& pose, int L_lo, int L_hi);

  //: Perform local optimisation to refine position,scale and angle
  //  Uses finder(L) to do refinement.
  void refine_match(const vimt_image_pyramid& im_pyr,
                    mfpf_pose& pose, double& fit, unsigned L=0);

  //: Find all local optima at coarsest scale and search around each
  //  Runs search at coarsest resolution, to find all local optima.
  //  If multiple angles/scales considered, the there may be many
  //  nearby responses.
  //  Each candidate is then localised by searching at finer and
  //  finer resolutions.
  //  Final responses may be further improved with refine_match()
  void multi_search(const vimt_image_pyramid& im_pyr,
                    const mfpf_pose& pose0,
                    vcl_vector<mfpf_pose>& poses,
                    vcl_vector<double>& fits);

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
  void multi_search_and_prune(const vimt_image_pyramid& im_pyr,
                    const mfpf_pose& pose0,
                    vcl_vector<mfpf_pose>& poses,
                    vcl_vector<double>& fits,
                    int prune_level=-1);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_mr_point_finder& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_mr_point_finder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_mr_point_finder& b);

#endif // mfpf_mr_point_finder_h_

