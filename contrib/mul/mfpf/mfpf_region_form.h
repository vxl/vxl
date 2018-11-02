#ifndef mfpf_region_form_h_
#define mfpf_region_form_h_
//:
// \file
// \brief Defines pose and shape (box/ellipse etc) of a region
// \author Tim Cootes

#include <cstdlib>
#include <iostream>
#include <utility>
#include <mfpf/mfpf_pose.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Defines pose and shape (box/ellipse etc) of a region
//  General purpose object.  form() gives shape (eg "box", "ellipse")
//  pose gives position, orientation and scale.
//  wi() and wj() give width and height in a reference frame,
//  which is mapped to the world frame by pose.
struct mfpf_region_form
{
  //: Pose (position + scale/orientation)
  mfpf_pose pose_;

  //: Width of shape along pose().u()
  double wi_;
  //: Width of shape along pose().v()
  double wj_;

  //: Name of form of shape ("box","ellipse")
  std::string form_;

 public:

  //: Constructor
  mfpf_region_form(const mfpf_pose& p, std::string form,
                   double wi, double wj)
   : pose_(p),wi_(wi),wj_(wj),form_(std::move(form)) {}

  //: Default constructor
  mfpf_region_form() = default;

  //: Pose (position + scale/orientation)
  mfpf_pose& pose() { return pose_; }

  //: Width of shape along pose().u()
  double& wi() { return wi_; }
  //: Width of shape along pose().v()
  double& wj() { return wj_; }

  //: Name of form of shape ("box","ellipse")
  std::string& form() { return form_; }

  //: Pose (position + scale/orientation)
  const mfpf_pose& pose() const { return pose_; }

  //: Width of shape along pose().u()
  double wi() const { return wi_; }
  //: Width of shape along pose().v()
  double wj() const { return wj_; }

  //: Name of form of shape ("box","ellipse")
  const std::string& form() const { return form_; }
};

inline std::ostream& operator<<(std::ostream& os,
                               const mfpf_region_form& p)
{
  os<<p.form()<<" wi: "<<p.wi()<<" wj: "<<p.wj()<<' '<<p.pose();
  return os;
}


inline void vsl_b_write(vsl_b_ostream& bfs,
                        const mfpf_region_form& p)
{
  vsl_b_write(bfs,short(1));  // Version number
  vsl_b_write(bfs,p.pose());
  vsl_b_write(bfs,p.form());
  vsl_b_write(bfs,p.wi());
  vsl_b_write(bfs,p.wj());
}

inline void vsl_b_read(vsl_b_istream& bfs, mfpf_region_form& p)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      vsl_b_read(bfs,p.pose());
      vsl_b_read(bfs,p.form());
      vsl_b_read(bfs,p.wi());
      vsl_b_read(bfs,p.wj());
      break;
    default:
      std::cerr << "vsl_b_read(bfs,mfpf_region_form): "
               << "Unexpected version number " << version << std::endl;
      std::abort();
  }
}

//: Write vector of region forms to stream
inline void vsl_b_write(vsl_b_ostream& bfs,
                        const std::vector<mfpf_region_form>& p)
{
  vsl_b_write(bfs,short(1));  // Version number
  vsl_b_write(bfs,unsigned(p.size()));
  for (const auto & i : p)
    vsl_b_write(bfs,i);
}

//: Read in vector of feature points from stream
inline void vsl_b_read(vsl_b_istream& bfs,
                       std::vector<mfpf_region_form>& p)
{
  short version;
  vsl_b_read(bfs,version);
  unsigned n;
  switch (version)
  {
    case 1:
      vsl_b_read(bfs,n);
      p.resize(n);
      for (unsigned i=0;i<n;++i) vsl_b_read(bfs,p[i]);
      break;
    default:
      std::cerr << "vsl_b_read(bfs,std::vector<mfpf_region_form>): "
               << "Unexpected version number " << version << std::endl;
      std::abort();
  }
}

#endif // mfpf_region_form_h_
