// This is brl/bseg/vpro/vpro_corr_tracker_process.h
#ifndef vpro_corr_tracker_process_h_
#define vpro_corr_tracker_process_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief tracks a correlation window across frames
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy August 21, 2002    Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <sdet/sdet_tracker_params.h>
#include <sdet/sdet_tracker.h>
#include <vpro/vpro_video_process.h>

class vpro_corr_tracker_process : public vpro_video_process
{
 public:
  vpro_corr_tracker_process(sdet_tracker_params & tp);
  ~vpro_corr_tracker_process();
  virtual process_data_type get_output_type(){return TOPOLOGY;}

  //: track to next frame
  virtual bool execute();
  virtual bool finish(){return true;}
 private:
  //members
  bool failure_;
  bool first_frame_;
  sdet_tracker tracker_;
};

#endif // vpro_corr_tracker_process_h_
