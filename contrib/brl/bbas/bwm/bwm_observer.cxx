#include "bwm_observer.h"
#include "bwm_observable.h"

#include <vgui/vgui_message.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d.h>

void bwm_observer::update (vgui_message const& msg) 
{
  vgui_message m = const_cast <vgui_message const& > (msg);
  
  const bwm_observable* o = static_cast<const bwm_observable*> (m.from);
  bwm_observable* observable = const_cast<bwm_observable*> (o);
  handle_update(msg, observable);
}
