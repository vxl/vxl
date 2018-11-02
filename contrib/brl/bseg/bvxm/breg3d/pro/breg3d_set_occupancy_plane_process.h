// This is brl/bseg/bvxm/breg3d/pro/breg3d_set_occupancy_plane_process.h
#ifndef breg3d_set_occupancy_plane_process_h_
#define breg3d_set_occupancy_plane_process_h_
//:
// \file
// \brief A process that sets the occupancy grid of the voxel world to a single plane
//
// \author Daniel Crispell
// \date Feb 26, 2008
// \verbatim
//  Modifications
//   Aug 09 2010 jlm  moved to brl/bseg/bvxm/breg3d/pro
// \endverbatim

#include <iostream>
#include <string>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_process.h>


#include <bvxm/bvxm_voxel_world.h>

class breg3d_set_occupancy_plane_process : public bprb_process
{
 public:

   breg3d_set_occupancy_plane_process();

  //: Copy Constructor (no local data)
  breg3d_set_occupancy_plane_process(const breg3d_set_occupancy_plane_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

  ~breg3d_set_occupancy_plane_process() override= default;

  //: Clone the process
  breg3d_set_occupancy_plane_process* clone() const override {return new breg3d_set_occupancy_plane_process(*this);}

  std::string name() const override {return "breg3dSetOccupancyPlane";}

  bool init() override { return true; }
  bool execute() override;
  bool finish() override{return true;}
};


#endif // breg3d_set_occupancy_plane_process_h_
