//:
// \file
// \brief Sorts matches so first has smallest fit
// \author Tim Cootes

#include <mfpf/mfpf_sort_matches.h>
#include <mbl/mbl_index_sort.h>

//: Sorts matches so first has smallest fit
//  Sorts so that pose[0] is best fit (ie smallest fit value)
void mfpf_sort_matches(vcl_vector<mfpf_pose>& poses,
                       vcl_vector<double>& fits)
{
  vcl_vector<double> fits0=fits;
  vcl_vector<mfpf_pose> poses0=poses;
  vcl_vector<int> index;
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
