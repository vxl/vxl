//:
// \file
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//     10 Nov 2008 Created (A. Garrido)
//\endverbatim


#include <cstdio>
#include <cstring>
#include <iostream>
#include "vidl_v4l2_control.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

extern "C" {
#include <sys/ioctl.h>
};

vidl_v4l2_control * vidl_v4l2_control::new_control(const v4l2_queryctrl& ctr, int f)
{
  if ( (ctr.flags & V4L2_CTRL_FLAG_DISABLED)
#ifdef V4L2_CTRL_FLAG_INACTIVE
       || (ctr.flags & V4L2_CTRL_FLAG_INACTIVE)
#endif
     )
    return 0;
  switch (ctr.type) {
    case V4L2_CTRL_TYPE_INTEGER:
      return new vidl_v4l2_control_integer(ctr,f);
      break;
    case V4L2_CTRL_TYPE_BOOLEAN:
      return new vidl_v4l2_control_boolean(ctr,f);
      break;
    case V4L2_CTRL_TYPE_MENU:
      {
      vidl_v4l2_control_menu *p= new vidl_v4l2_control_menu(ctr,f);
      if (p->n_items()==0) {
        delete p;
        p= 0;
      }
      return p;
      }
      break;
    case V4L2_CTRL_TYPE_BUTTON:
      return new vidl_v4l2_control_button(ctr,f);
      break;
    default:
      break;
  }

  return 0;
}

void vidl_v4l2_control::set_value(int v) const
{
  struct v4l2_control control;
  std::memset (&control, 0, sizeof (control));
  control.id = ctrl_.id;
  control.value = v;
  ioctl (fd, VIDIOC_S_CTRL, &control); // error ignored
}

int vidl_v4l2_control::get_value() const
{
  struct v4l2_control control;
  std::memset (&control, 0, sizeof (control));
  control.id = ctrl_.id;
  ioctl (fd, VIDIOC_G_CTRL, &control); // error ignored
  return control.value;
}

// ----------------- Control integer ---------------

void vidl_v4l2_control_integer::set(int value) const
{
  if (value<ctrl_.minimum) value= ctrl_.minimum;
  else if (value>ctrl_.maximum) value= ctrl_.maximum;
       else value= ctrl_.minimum+(value-ctrl_.minimum)/ctrl_.step * ctrl_.step;
  set_value(value);
}

void vidl_v4l2_control_integer::set_100(int value) const
{
  if (value<=0) value= ctrl_.minimum;
  else if (value>=100) value= ctrl_.maximum;
       else value= ctrl_.minimum+(ctrl_.maximum-ctrl_.minimum)*value/100;
  set_value(value);
}

std::string vidl_v4l2_control_integer::description() const
{
  char cad[256];
  std::snprintf(cad,256,"Control \"%s\": integer (min: %d, max: %d, step: %d, default: %d)",
               (const char *) ctrl_.name, minimum(), maximum(), step(), default_value());
  return cad;
}

// ----------------- Control menu ---------------

vidl_v4l2_control_menu::vidl_v4l2_control_menu(const v4l2_queryctrl& ctr, int f):
                                                                vidl_v4l2_control(ctr,f)
{
  struct v4l2_querymenu menu;
  std::memset(&menu, 0, sizeof (menu));
  menu.id= ctrl_.id;
  for (menu.index = ctrl_.minimum; (int) menu.index <= ctrl_.maximum;menu.index++) {
                if (0 == ioctl (fd, VIDIOC_QUERYMENU, &menu)) {
                        items.push_back((char *)menu.name);
                } else {
                        //std::cerr << "VIDIOC_QUERYMENU\n";
                        items.clear(); // control menu is not added to the list
                        return;
                }
        }
}

std::string vidl_v4l2_control_menu::description() const
{
  char cad[256];
  std::snprintf(cad,256,"Control \"%s\": menu (%d items, default: %d)",
               (const char *) ctrl_.name, n_items(), default_value());
  return cad;
}

// ----------------- Control boolean ---------------
