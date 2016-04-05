#include "mfpf_sort_matches.h"
//:
// \file
// \brief Sorts matches so first has smallest fit
// \author Tim Cootes

#include <mbl/mbl_index_sort.h>

//: Sorts matches so first has smallest fit
//  Sorts so that pose[0] is best fit (ie smallest fit value)
void mfpf_sort_matches(std::vector<mfpf_pose>& poses,
                       std::vector<double>& fits)
{
  std::vector<double> fits0=fits;
  std::vector<mfpf_pose> poses0=poses;
  std::vector<int> index;
  mbl_index_sort(fits,index);
  for (unsigned i=0;i<index.size();++i)
  {
    poses[i]=poses0[index[i]];
    fits[i] =fits0[index[i]];
  }
}

//: Sorts so that pose_set.pose[0] is best fit (ie smallest fit value)
void mfpf_sort_matches(mfpf_pose_set& pose_set)
{
  mfpf_sort_matches(pose_set.poses,pose_set.fits);
}
