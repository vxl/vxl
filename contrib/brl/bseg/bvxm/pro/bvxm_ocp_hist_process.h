// This is brl/bseg/bvxm/pro/bvxm_ocp_hist_process.h
#ifndef bvxm_ocp_hist_process_h_
#define bvxm_ocp_hist_process_h_
//:
// \file
// \brief A class for generating a histogram out of occupancy probability grid
//        -  Input:
//             - bvxm_voxel_world_sptr
//             - vcl_string             --path for the text file of the histogram
//
//        -  Output:
//             - none
//
// \author  Gamze D. Tunali
// \date    05/22/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>

#include <bprb/bprb_process.h>

#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

class bvxm_ocp_hist_process : public bprb_process
{
 public:

   bvxm_ocp_hist_process();

  //: Copy Constructor (no local data)
  bvxm_ocp_hist_process(const bvxm_ocp_hist_process& other)
  : bprb_process(*static_cast<const bprb_process*>(&other)) { }

  ~bvxm_ocp_hist_process(){}

  //: Clone the process
  virtual bvxm_ocp_hist_process* clone() const
  { return new bvxm_ocp_hist_process(*this); }

  vcl_string name() { return "bvxmOcpHistProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
private:
  bool compute(bvxm_voxel_world_sptr w, vcl_string path);
};

#endif // bvxm_ocp_hist_process_h_
