// This is brl/bseg/bvxm/pro/bvxm_pmap_hist_process.h
#ifndef bvxm_pmap_hist_process_h_
#define bvxm_pmap_hist_process_h_
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

class bvxm_pmap_hist_process : public bprb_process
{
 public:

   bvxm_pmap_hist_process();

  //: Copy Constructor (no local data)
  bvxm_pmap_hist_process(const bvxm_pmap_hist_process& other)
  : bprb_process(*static_cast<const bprb_process*>(&other)) { }

  ~bvxm_pmap_hist_process(){}

  //: Clone the process
  virtual bvxm_pmap_hist_process* clone() const
  { return new bvxm_pmap_hist_process(*this); }

  vcl_string name() { return "bvxmPmapHistProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
private:
  bool compute(vcl_string pmap, vcl_string path);
};

#endif // bvxm_pmap_hist_process_h_
