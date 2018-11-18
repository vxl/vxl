#include "mfpf_prune_overlaps.h"
//:
// \file
// \brief Function to remove any overlapping matching responses
// \author Tim Cootes

#include <mbl/mbl_index_sort.h>

//: Find list of poses overlapping given pose
void mfpf_find_overlaps(mfpf_point_finder& pf,
                        const std::vector<mfpf_pose>& poses,
                        const mfpf_pose& pose,
                        std::vector<unsigned>& overlaps)
{
  overlaps.resize(0);
  for (unsigned i=0;i<poses.size();++i)
  {
    if (pf.overlap(poses[i],pose))
      overlaps.push_back(i);
  }
}

//: If pose not near any poses in list, return false
//  If it is near one, and its fit is better, then replace it.
//  Uses pf.overlap() function to check for proximity
bool mfpf_find_near_pose(mfpf_point_finder& pf,
                         std::vector<mfpf_pose>& poses,
                         std::vector<double>& fits,
                         const mfpf_pose& pose, double fit)
{
  std::vector<unsigned> index;
  mfpf_find_overlaps(pf,poses,pose,index);
  if (index.size()==0) return false;  // No overlaps

  if (index.size()==1)
  {
    // Only overlaps with a single pose
    // Replace that pose if new one is better
    if (fits[index[0]]>fit)
    {
      poses[index[0]]=pose;
      fits[index[0]]=fit;
    }
    return true;
  }

  // More complicated situation
  // We have more than one overlapping pose
  // If new one is worse than all of them, ignore it.
  // If new one is better than all of them, replace them all
  // Otherwise we have a potentially ambiguous situation
  // eg three overlapping objects in a row A-B-C with A not
  // overlapping with C, but f(A)<f(B)<f(C)
  // In this case eliminate all overlapping poses except the best one.
  // There's a danger that this might loose some weaker local
  // minima, so perhaps revise this later.

  // Count the number of overlaps with worse fit
  // and record best fit
  unsigned n_worse=0;
  double best_fit = fit;
  unsigned best_i = 0;
  for (unsigned int i : index)
  {
    if (fits[i]>fit) { n_worse++;}
    if (fits[i]<best_fit)
    { best_i=i; best_fit=fits[i]; }
  }

  if (n_worse==0) return true; // New pose no better than any existing

  // Have to generate new lists
  std::vector<mfpf_pose> poses0=poses;
  std::vector<double> fits0=fits;
  unsigned n1 = poses.size()+1-index.size();
  poses.resize(n1);
  fits.resize(n1);
  unsigned j=0;
  // First add all poses which don't overlap with new pose
  for (unsigned i=0;i<poses0.size();++i)
  {
    if (!pf.overlap(poses0[i],pose))
    {
      poses[j]=poses0[i];
      fits[j]=fits0[i];
      ++j;
    }
  }
  if (n_worse==index.size())
  {
    // New one better than all overlaps
    poses[j]=pose;  fits[j]=fit;
  }
  else
  {
    // Retain best fit
    poses[j]=poses0[best_i]; fits[j]=best_fit;
  }
  return true;
}


//: Remove any overlapping matching responses (retaining best fit)
void mfpf_prune_overlaps(mfpf_point_finder& pf,
                         std::vector<mfpf_pose>& poses,
                         std::vector<double>& fits)
{
  std::vector<mfpf_pose> poses0 = poses;
  std::vector<double> fits0 = fits;

  poses.resize(0); fits.resize(0);

  for (unsigned i=0;i<poses0.size();++i)
  {
    if (!mfpf_find_near_pose(pf,poses,fits,poses0[i],fits0[i]))
    {
      // Pose distinct
      poses.push_back(poses0[i]);
      fits.push_back(fits0[i]);
    }
  }
}

//: Return true if pose overlaps with any of poses
bool mfpf_any_overlaps(mfpf_point_finder& pf,
                       const std::vector<mfpf_pose>& poses,
                       const mfpf_pose& pose)
{
  for (const auto & i : poses)
  {
    if (pf.overlap(i,pose)) return true;
  }
  return false;
}


//:  Sort responses and return list of non-overlapping responses
//  If max_n>0 then return at most max_n
void mfpf_prune_and_sort_overlaps(mfpf_point_finder& pf,
                                  std::vector<mfpf_pose>& poses,
                                  std::vector<double>& fits,
                                  unsigned max_n)
{
  std::vector<mfpf_pose> poses0 = poses;
  std::vector<double> fits0 = fits;

  poses.resize(0); fits.resize(0);

  std::vector<int> index;
  mbl_index_sort(fits0,index);
  for (int i : index)
  {
    if (!mfpf_any_overlaps(pf,poses,poses0[i]))
    {
      poses.push_back(poses0[i]);
      fits.push_back(fits0[i]);
    }
    if (max_n>0 && poses.size()>=max_n) return;
  }
}
