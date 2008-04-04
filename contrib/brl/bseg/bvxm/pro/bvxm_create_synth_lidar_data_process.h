#ifndef bvxm_create_synth_lidar_data_process_h_
#define bvxm_create_synth_lidar_data_process_h_

//:
// \file
// \brief A process for creating synthetic lidar data for testing purposes
// \author Gamze Tunali
// \date 04/04/2008
//
// \verbatim
//  Modifications
// 
// \endverbatim


#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_create_synth_lidar_data_process : public bprb_process
{
 public:
  
  bvxm_create_synth_lidar_data_process();

  //: Copy Constructor (no local data)
  bvxm_create_synth_lidar_data_process(const bvxm_create_synth_lidar_data_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~bvxm_create_synth_lidar_data_process();

  //: Clone the process
  virtual bvxm_create_synth_lidar_data_process* clone() const {return new bvxm_create_synth_lidar_data_process(*this);}

  vcl_string name(){return "bvxmCreateSynthLidarDataProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif //bvxm_create_synth_lidar_data_process_h_
