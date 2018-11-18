#ifndef mfpf_searcher_h_
#define mfpf_searcher_h_
//:
// \file
// \brief Algorithms to perform global search for multiple matches
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>
#include <mfpf/mfpf_pose_set.h>
#include <vgl/vgl_fwd.h>

//: Algorithms to perform global search for multiple matches
//  Note that some search algorithms are in mfpf_point_finder
//  itself.
//  More complex algorithms are here, as there are multiple
//  ways of doing things.
class mfpf_searcher
{
 private:
  //: Radius around point (in model coords) for two poses to be similar
  double proximity_r_;

  //: Find list of poses overlapping given pose
  void find_overlaps(mfpf_point_finder& pf,
                     const std::vector<mfpf_pose>& poses,
                     const mfpf_pose& pose,
                     std::vector<unsigned>& overlaps);

  //: If pose not near any poses in list, return false
  //  If it is near one, and its fit is better, then replace it.
  //  Uses pf.overlap() function to check for proximity
  bool find_near_pose(mfpf_point_finder& pf,
                      std::vector<mfpf_pose>& poses,
                      std::vector<double>& fits,
                      const mfpf_pose& pose, double fit);

 public:
  mfpf_searcher();

  void find_refined_matches(mfpf_point_finder& pf,
                            const vimt_image_2d_of<float>& image,
                            const vgl_point_2d<double>& p,
                            const vgl_vector_2d<double>& u,
                            std::vector<mfpf_pose>& pts,
                            std::vector<double>& fit);

  //: For each pose in the set, perform local search+refinement
  //  On exit pose_set contains the improved matches.
  void search_around_set(mfpf_point_finder& pf,
                         const vimt_image_2d_of<float>& image,
                         mfpf_pose_set& pose_set);
};


#endif // mfpf_searcher_h_
