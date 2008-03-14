// This is contrib/bvam/pro/bvam_create_local_rpc_process.h
#ifndef bvam_create_local_rpc_process_h_
#define bvam_create_local_rpc_process_h_

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

class bvam_create_local_rpc_process : public bprb_process
{
 public:
   bvam_create_local_rpc_process();

  //: Copy Constructor (no local data)
  bvam_create_local_rpc_process(const bvam_create_local_rpc_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvam_create_local_rpc_process(){};

  //: Clone the process
  virtual bvam_create_local_rpc_process* clone() const 
  { return new bvam_create_local_rpc_process(*this); }

  vcl_string name(){return "BvamCreateLocalRpcProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif // bvam_create_local_rpc_process_h_
