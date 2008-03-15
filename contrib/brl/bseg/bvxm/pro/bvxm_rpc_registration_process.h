// This is contrib/brl/bseg/bvxm/pro/bvxm_rpc_registration_process.h
#ifndef bvxm_rpc_registration_process_h_
#define bvxm_rpc_registration_process_h_

//:
// \file
// \brief // A process that optimizes rpc camera parameters based on edges in images and the voxel world
//           
// \author Ibrahim Eden
// \date 03/02/08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

#include <vil/vil_image_view.h>

class bvxm_rpc_registration_process : public bprb_process
{
 public:
   bvxm_rpc_registration_process();

  //: Copy Constructor (no local data)
  bvxm_rpc_registration_process(const bvxm_rpc_registration_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_rpc_registration_process(){};

  //: Clone the process
  virtual bvxm_rpc_registration_process* clone() const 
  { return new bvxm_rpc_registration_process(*this); }

  vcl_string name(){return "bvxmRpcRegistrationProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

  void advanced_offset_estimation(const int offset_search_size,const vil_image_view<vxl_byte>& edge_image,const vil_image_view<float>& expected_edge_image,int& max_u,int& max_v);
  void translate_image(const vil_image_view<float>& inp,vil_image_view<float>& out, double ti,double tj);
 private:

};

#endif // bvxm_rpc_registration_process_h_
