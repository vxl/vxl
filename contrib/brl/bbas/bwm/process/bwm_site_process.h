#ifndef bwm_site_process_h_
#define bwm_site_process_h_

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

/*#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000*/

#include "bwm_process.h"
#include "bwm/io/bwm_site_sptr.h"
#include <vul/vul_timer.h>

class bwm_site_process : public bwm_process {

public:
  bwm_site_process() : bwm_process() { site_ = 0; timer_.mark();}
  ~bwm_site_process() {}

  void set_site(bwm_site_sptr site) { site_ = site; }

  // overridable
  void RunBackgroundTask();

private:
  bwm_site_sptr site_;
  static double version_num;
  vul_timer timer_;
};

#endif