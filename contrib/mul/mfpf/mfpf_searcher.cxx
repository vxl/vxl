//:
// \file
// \brief Algorithms to perform global search for multiple matches
// \author Tim Cootes

#include <mfpf/mfpf_searcher.h>
#include <mbl/mbl_index_sort.h>

mfpf_searcher::mfpf_searcher()
{
  proximity_r_= 3;
}

//: Find list of poses overlapping given pose
void mfpf_searcher::find_overlaps(mfpf_point_finder& pf,
                     const vcl_vector<mfpf_pose>& poses,
                     const mfpf_pose& pose, 
                     vcl_vector<unsigned>& overlaps)
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
bool mfpf_searcher::find_near_pose(vcl_vector<mfpf_pose>& poses,
                      vcl_vector<double>& fits,
                      const mfpf_pose& pose, double fit,
                      double r, double r_thresh)
{
  double t = r_thresh*r_thresh;

  for (unsigned i=0;i<poses.size();++i)
  {
    double d2 = mfpf_max_sqr_diff(poses[i],pose,r);
    if (d2<t)
    {
      if (fit<fits[i])
      {
        fits[i]=fit; poses[i]=pose;
      }
      return true;
    }
  }
  return false;
}

//: If pose not near any poses in list, return false
//  If it is near one, and its fit is better, then replace it.
//  Uses pf.overlap() function to check for proximity
bool mfpf_searcher::find_near_pose(mfpf_point_finder& pf,
                      vcl_vector<mfpf_pose>& poses,
                      vcl_vector<double>& fits,
                      const mfpf_pose& pose, double fit)
{
/*
  // Incorrect approach as it may lead to overlaps left in.
  for (unsigned i=0;i<poses.size();++i)
  {
    if (pf.overlap(poses[i],pose))
    {
      if (fit<fits[i])
      {
        fits[i]=fit; poses[i]=pose;
      }
      return true;
    }
  }
  return false;
*/
  vcl_vector<unsigned> index;
  find_overlaps(pf,poses,pose,index);
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
  // In this case elliminate all overlapping poses except the best one.
  // There's a danger that this might loose some weaker local
  // minima, so perhaps revise this later.

  // Count the number of overlaps with worse fit
  // and record best fit
  unsigned n_worse=0;
  double best_fit = fit;
  unsigned best_i = 0;
  for (unsigned i=0;i<index.size();++i)
  {
    if (fits[index[i]]>fit) { n_worse++;}
    if (fits[index[i]]<best_fit) 
    { best_i=index[i]; best_fit=fits[index[i]]; }
  }

  if (n_worse==0) return true; // New pose no better than any existing

  // Have to generate new lists
  vcl_vector<mfpf_pose> poses0=poses;
  vcl_vector<double> fits0=fits;
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

void mfpf_searcher::find_refined_matches(mfpf_point_finder& pf,
                           const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u,
                           vcl_vector<mfpf_pose>& poses,
                           vcl_vector<double>& fits)
{
  vcl_vector<mfpf_pose> poses1;
  vcl_vector<double> fits1;
  // Exhaustive search at multiple angles and scales
  // Returns local minima in (x,y)
  // However, single object may return multiple responses,
  // one at each angle/scale.
  pf.grid_search(image,p,u,poses1,fits1);
vcl_cout<<"N.responses: "<<poses1.size()<<vcl_endl;

/*
  double step = pf.step_size();
  double r = pf.radius()*step;

  // Two poses assumed similar if within about r/4 of each other
  // Or two pixels in the model frame
  double r_thresh = vcl_max(0.5*r,2*step);
*/

  // Refine each one in turn, and add it to list if new
  poses.resize(0); fits.resize(0);
  for (unsigned i=0;i<poses1.size();++i)
  {
    mfpf_pose pose=poses1[i];
    double f = fits1[i];
    pf.refine_match(image,pose.p(),pose.u(),f);
    if (f>fits1[i]) vcl_cerr<<"Refinement failed!!!"<<vcl_endl;
    if (fits1[i]<-0.99)
    {
      vcl_cout<<"Good match: "<<poses1[i]<<" fit: "<<fits1[i]<<vcl_endl;
      vcl_cout<<"Refined to: "<<pose<<" fit: "<<f<<vcl_endl;
    }
    if (!find_near_pose(pf,poses,fits,pose,f))
    {
      // Point distinct
      poses.push_back(pose);
      fits.push_back(f);
    }
  }
}

//: Sorts so that pose[0] is best fit (ie smallest fit value)
void mfpf_searcher::sort_matches(vcl_vector<mfpf_pose>& poses,
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


