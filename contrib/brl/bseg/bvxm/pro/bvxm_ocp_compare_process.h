// This is brl/bseg/bvxm/pro/bvxm_ocp_compare_process.h
#ifndef bvxm_ocp_compare_process_h_
#define bvxm_ocp_compare_process_h_
//:
// \file
// \brief A class for comparing to occupancey grid for the probability 
//        values. It makes a search in the kxkxk neighborhood and returns 
//        the higher value. If the value is higher, the similarity is bigger.
//        -  Input:
//             - bvxm_voxel_world_sptr
//             - bvxm_voxel_world_sptr
//
//        -  Output:
//             - double    the similarity measure
//
// \author  Gamze D. Tunali
// \date    05/15/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>

#include <bgeo/bgeo_lvcs_sptr.h>
#include <bprb/bprb_process.h>
#include <vpgl/vpgl_local_rational_camera.h>

#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

class bvxm_ocp_compare_process : public bprb_process
{
 public:

   bvxm_ocp_compare_process();

  //: Copy Constructor (no local data)
  bvxm_ocp_compare_process(const bvxm_ocp_compare_process& other)
  : bprb_process(*static_cast<const bprb_process*>(&other)) { }

  ~bvxm_ocp_compare_process(){}

  //: Clone the process
  virtual bvxm_ocp_compare_process* clone() const
  { return new bvxm_ocp_compare_process(*this); }

  vcl_string name() { return "bvxmOcpCompareProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
private:
  double compare(bvxm_voxel_world_sptr w1, bvxm_voxel_world_sptr w2, unsigned n);
  bool save_raw(char *ocp_array, int x, int y, int z, vcl_string filename);
};

#endif // bvxm_ocp_compare_process_h_
