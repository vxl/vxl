// This is brl/bseg/bvxm/breg3d/pro/breg3d_init_ekf_camera_optimizer_process.h
#ifndef breg3d_init_ekf_camera_optimizer_process_h_
#define breg3d_init_ekf_camera_optimizer_process_h_
//:
// \file
// \brief A process that initializes the state of an extended kalman filter used for camera registration
//
// \author Daniel Crispell
// \date Feb 26, 2008
// \verbatim
//  Modifications
//   Aug 09 2010 jlm  moved to brl/bseg/bvxm/breg3d/pro
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class breg3d_init_ekf_camera_optimizer_process : public bprb_process
{
 public:

   breg3d_init_ekf_camera_optimizer_process();

  //: Copy Constructor (no local data)
  breg3d_init_ekf_camera_optimizer_process(const breg3d_init_ekf_camera_optimizer_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

  ~breg3d_init_ekf_camera_optimizer_process(){}

  //: Clone the process
  virtual breg3d_init_ekf_camera_optimizer_process* clone() const {return new breg3d_init_ekf_camera_optimizer_process(*this);}

  virtual vcl_string name() const {return "breg3dInitEkfCameraOptimizer";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // breg3d_init_ekf_camera_optimizer_process_h_
