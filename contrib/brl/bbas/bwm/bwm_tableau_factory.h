#ifndef bwm_tableau_factory_h_
#define bwm_tableau_factory_h_

#include "bwm_tableau_img.h"
#include "io/bwm_io_structs.h"
#include <vgui/vgui_tableau_sptr.h>

class bwm_tableau_factory
{
 public:
  bwm_tableau_factory() {}
  ~bwm_tableau_factory() {}
  bwm_tableau_img* create_tableau(bwm_io_tab_config* tab);
  
private:

};

#endif // bwm_tableau_factory_h_
