#ifndef bwm_site_process_h_
#define bwm_site_process_h_

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

/*#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000*/

//#include "StdAfx.h"
#include "bwm_process.h"
#include "bwm/bwm_site.h"

class bwm_site_process : public bwm_process {

public:
  bwm_site_process() : bwm_process() { site_ = 0;}
  ~bwm_site_process() { if (site_) delete site_; }

  void set_site(bwm_site* site) { site_ = site; }

  // overridable
  void RunBackgroundTask();

private:
  bwm_site* site_;

};

#endif