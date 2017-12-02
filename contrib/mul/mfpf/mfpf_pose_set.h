#ifndef mfpf_pose_set_h_
#define mfpf_pose_set_h_
//:
// \file
// \brief Container for set of poses and associated fit values
// \author Tim Cootes

#include <iostream>
#include <cstdlib>
#include <mfpf/mfpf_pose.h>
#include <vsl/vsl_vector_io.h>
#include <vcl_compiler.h>

struct mfpf_pose_set
{
  //: List of poses
  std::vector<mfpf_pose> poses;

  //: List of associated fits
  std::vector<double> fits;
};

inline void vsl_b_write(vsl_b_ostream& bfs,
                        const mfpf_pose_set& p)
{
  vsl_b_write(bfs,short(1));  // Version number
  vsl_b_write(bfs,p.poses);
  vsl_b_write(bfs,p.fits);
}

inline void vsl_b_read(vsl_b_istream& bfs, mfpf_pose_set& p)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,p.poses);
      vsl_b_read(bfs,p.fits);
      break;
    default:
      std::cerr << "vsl_b_read(bfs,mfpf_pose_set): "
               << "Unexpected version number " << version << std::endl;
      std::abort();
  }
}

//: Write vector of objects to stream
inline void vsl_b_write(vsl_b_ostream& bfs,
                        const std::vector<mfpf_pose_set>& p)
{
  vsl_b_write(bfs,short(1));  // Version number
  vsl_b_write(bfs,unsigned(p.size()));
  for (unsigned i=0;i<p.size();++i)
    vsl_b_write(bfs,p[i]);
}

//: Read in vector of feature points from stream
inline void vsl_b_read(vsl_b_istream& bfs,
                       std::vector<mfpf_pose_set>& p)
{
  short version;
  vsl_b_read(bfs,version);
  unsigned n;
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,n);
      p.resize(n);
      for (unsigned i=0;i<n;++i) vsl_b_read(bfs,p[i]);
      break;
    default:
      std::cerr << "vsl_b_read(bfs,std::vector<mfpf_pose_set>): "
               << "Unexpected version number " << version << std::endl;
      std::abort();
  }
}

#endif // mfpf_pose_set_h_

