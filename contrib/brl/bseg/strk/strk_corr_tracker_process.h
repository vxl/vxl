// This is brl/bseg/strk/strk_corr_tracker_process.h
#ifndef strk_corr_tracker_process_h_
#define strk_corr_tracker_process_h_
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
#include <strk/strk_tracker_params.h>
#include <strk/strk_tracker.h>
#include <vpro/vpro_video_process.h>

class strk_corr_tracker_process : public vpro_video_process
{
 public:
  strk_corr_tracker_process(strk_tracker_params & tp);
  ~strk_corr_tracker_process();
  virtual process_data_type get_output_type(){return TOPOLOGY;}

  //: track to next frame
  virtual bool execute();
  virtual bool finish(){return true;}
 private:
  //members
  bool failure_;
  bool first_frame_;
  strk_tracker tracker_;
};


#endif // strk_corr_tracker_process_h_
