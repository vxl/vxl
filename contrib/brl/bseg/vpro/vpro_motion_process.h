// This is brl/bseg/vpro/vpro_motion_process.h
#ifndef vpro_motion_process_h_
#define vpro_motion_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief detects linear motion
// \author J.L. Mundy
//
// \verbatim
//  Modifications
//   J.L. Mundy - February 16, 2003 - Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vpro/vpro_motion_params.h>
#include <vpro/vpro_video_process.h>

class vpro_motion_process: public vpro_video_process, public vpro_motion_params
{
 public:
  enum state_symbol {NO_IMAGE=0, FIRST_IMAGE, IN_PROCESS};
  vpro_motion_process(vpro_motion_params& vmp);
  ~vpro_motion_process();
  virtual process_data_type get_output_type() { return IMAGE; }
  //: compute motion
  virtual bool execute();
  virtual bool finish();
 private:
  //local methods
  vpro_motion_process();//prevent default constructor
  void compute_motion(vil1_image ix, vil1_image iy);
  void update_queue(vil1_image ix, vil1_image iy);
  //members
  state_symbol state_;
  bool first_frame_;
  vcl_vector<vil1_image > queuex_;
  vcl_vector<vil1_image > queuey_;
};

#endif // vpro_motion_process_h_
