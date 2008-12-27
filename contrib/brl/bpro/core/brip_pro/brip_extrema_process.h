// This is brl/bpro/core/brip_pro/brip_extrema_process.h
#ifndef brip_extrema_process_h_
#define brip_extrema_process_h_
//:
// \file
// \brief A process for computing intensity extrema
// \author J.L. Mundy
// \date November 15, 2008
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class brip_extrema_process : public bprb_process
{
 public:
  brip_extrema_process();

  //: Copy Constructor (no local data)
  brip_extrema_process(const brip_extrema_process& othe_pr) : bprb_process(*static_cast<const bprb_process*>(&othe_pr)) { }

  ~brip_extrema_process();

  //: Clone the process
  virtual brip_extrema_process* clone() const { return new brip_extrema_process(*this); }

  vcl_string name() { return "BripExtremaProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }

};

#endif // brip_extrema_process_h_
