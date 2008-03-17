#ifndef mfpf_prune_overlaps_h_
#define mfpf_prune_overlaps_h_
//:
// \file
// \brief Function to remove any overlapping matching responses
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>

//: Remove any overlapping matching responses (retaining best fit)
void mfpf_prune_overlaps(mfpf_point_finder& pf,
                         vcl_vector<mfpf_pose>& poses,
                         vcl_vector<double>& fits);


//: Find list of poses overlapping given pose
void mfpf_find_overlaps(mfpf_point_finder& pf,
                        const vcl_vector<mfpf_pose>& poses,
                        const mfpf_pose& pose, 
                        vcl_vector<unsigned>& overlaps);

#endif // mfpf_prune_overlaps_h_

