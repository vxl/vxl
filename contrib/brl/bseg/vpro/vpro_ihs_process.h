// This is brl/bseg/vpro/vpro_ihs_process.h
#ifndef vpro_ihs_process_h_
#define vpro_ihs_process_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief Displays color information derived from IHS encoding as a video stream.
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy January, 8, 2004    Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <vpro/vpro_video_process.h>

class vpro_ihs_process : public vpro_video_process
{
 public:
  vpro_ihs_process();
  ~vpro_ihs_process();
  virtual process_data_type get_output_type() { return IMAGE; }

  //: track to next frame
  virtual bool execute();
  virtual bool finish() { return true; }
 private:
  //members
  bool failure_;
};

#endif // vpro_ihs_process_h_
