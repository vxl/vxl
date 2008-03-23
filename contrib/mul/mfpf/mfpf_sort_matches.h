#ifndef mfpf_sort_matches_h_
#define mfpf_sort_matches_h_
//:
// \file
// \brief Sorts matches so first has smallest fit
// \author Tim Cootes

#include <mfpf/mfpf_pose_set.h>

//: Sorts matches so first has smallest fit
//  Sorts so that pose[0] is best fit (ie smallest fit value)
void mfpf_sort_matches(vcl_vector<mfpf_pose>& poses,
                       vcl_vector<double>& fits);

//: Sorts so that pose_set.pose[0] is best fit (ie smallest fit value)
void mfpf_sort_matches(mfpf_pose_set& pose_set);

#endif // mfpf_sort_matches_h_
