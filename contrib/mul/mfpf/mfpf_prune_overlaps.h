#ifndef mfpf_prune_overlaps_h_
#define mfpf_prune_overlaps_h_
//:
// \file
// \brief Function to remove any overlapping matching responses
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>

//: Remove any overlapping matching responses (retaining best fit)
void mfpf_prune_overlaps(mfpf_point_finder& pf,
                         std::vector<mfpf_pose>& poses,
                         std::vector<double>& fits);


//: Find list of poses overlapping given pose
void mfpf_find_overlaps(mfpf_point_finder& pf,
                        const std::vector<mfpf_pose>& poses,
                        const mfpf_pose& pose,
                        std::vector<unsigned>& overlaps);

//: Return true if pose overlaps with any of poses
bool mfpf_any_overlaps(mfpf_point_finder& pf,
                        const std::vector<mfpf_pose>& poses,
                        const mfpf_pose& pose);

//:  Sort responses and return list of non-overlapping responses
//  If max_n>0 then return at most max_n
void mfpf_prune_and_sort_overlaps(mfpf_point_finder& pf,
                         std::vector<mfpf_pose>& poses,
                         std::vector<double>& fits,
                         unsigned max_n=0);


#endif // mfpf_prune_overlaps_h_
