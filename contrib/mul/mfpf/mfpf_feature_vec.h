#ifndef mfpf_feature_vec_h_
#define mfpf_feature_vec_h_
//:
// \file
// \brief Container for set of poses and associated fit values
// \author Martin Roberts

#include <mfpf/mfpf_pose.h>
#include <mfpf/mfpf_pose_set.h>
#include <vsl/vsl_vector_io.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>


//As well as pose_set, it can be helpful to think of vector combined pose/fit
struct mfpf_feature
{
    mfpf_pose pose;
    double fit;
    mfpf_feature() {fit=0.0;}
    mfpf_feature(const mfpf_pose& p,double f):pose(p),fit(f) {}
    
};
typedef vcl_vector<mfpf_feature > mfpf_feature_vec ;



inline void vsl_b_write(vsl_b_ostream& bfs,
                        const mfpf_feature& f)
{
  vsl_b_write(bfs,short(1));  // Version number
  vsl_b_write(bfs,f.pose);
  vsl_b_write(bfs,f.fit);
}
inline void vsl_b_read(vsl_b_istream& bfs,
                        mfpf_feature& f)
{

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,f.pose);
      vsl_b_read(bfs,f.fit);
      break;
    default:
      vcl_cerr << "vsl_b_read(bfs,mfpf_feature): "
               << "Unexpected version number " << version << vcl_endl;
      vcl_abort();
  }    
}

inline void mfpf_pose_set_to_feature_vec(const mfpf_pose_set& pose_set, mfpf_feature_vec& feature_vec)
{
    vcl_vector<mfpf_pose>::const_iterator posesIter=pose_set.poses.begin();
    vcl_vector<mfpf_pose>::const_iterator posesIterEnd=pose_set.poses.end();
    vcl_vector<double >::const_iterator fitsIter=pose_set.fits.begin();

    feature_vec.clear();
    feature_vec.reserve(pose_set.poses.size());
    while(posesIter != posesIterEnd)
    {
        feature_vec.push_back(mfpf_feature(*posesIter++,*fitsIter++));
    }
}
inline void mfpf_feature_vec_to_pose_set(const mfpf_feature_vec& feature_vec, mfpf_pose_set& pose_set )
{
    pose_set.poses.clear();    pose_set.fits.clear();
    pose_set.poses.reserve(feature_vec.size()); pose_set.fits.reserve(feature_vec.size()); 
    mfpf_feature_vec::const_iterator featureIter=feature_vec.begin();
    mfpf_feature_vec::const_iterator featureIterEnd=feature_vec.begin();

    while(featureIter != featureIterEnd)
    {
        pose_set.poses.push_back(mfpf_pose(featureIter->pose));
        pose_set.fits.push_back(featureIter->fit);
        ++featureIter;
    }
}

#endif // mfpf_feature_vec_h_

