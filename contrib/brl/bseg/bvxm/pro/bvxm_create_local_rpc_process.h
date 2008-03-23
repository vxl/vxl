// This is brl/bseg/bvxm/pro/bvxm_create_local_rpc_process.h
#ifndef bvxm_create_local_rpc_process_h_
#define bvxm_create_local_rpc_process_h_
//:
// \file
// \brief // A process that takes a world model and rational camera and returns a local rational camera
//
// \author Ibrahim Eden
// \date 03/14/08
// \verbatim
//
// \Modifications

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_create_local_rpc_process : public bprb_process
{
 public:
   bvxm_create_local_rpc_process();

  //: Copy Constructor (no local data)
  bvxm_create_local_rpc_process(const bvxm_create_local_rpc_process& other)
  : bprb_process(*static_cast<const bprb_process*>(&other)) {}

  ~bvxm_create_local_rpc_process() {}

  //: Clone the process
  virtual bvxm_create_local_rpc_process* clone() const
  { return new bvxm_create_local_rpc_process(*this); }

  vcl_string name() { return "bvxmCreateLocalRpcProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
};

#endif // bvxm_create_local_rpc_process_h_
