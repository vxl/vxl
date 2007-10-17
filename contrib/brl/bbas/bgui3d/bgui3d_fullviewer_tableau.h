// This is basic/bgui3d/bgui3d_fullviewer_tableau.h
#ifndef bgui3d_fullviewer_tableau_h_
#define bgui3d_fullviewer_tableau_h_

//:
// \file
// \brief  Abstract base tableau for 3D viewers with interaction
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   5/27/04
//
// \verbatim
//  Modifications
// \endverbatim

#include "bgui3d_viewer_tableau.h"


//:  Abstract base tableau for 3D viewers with interaction
class bgui3d_fullviewer_tableau : public bgui3d_viewer_tableau
{
public:
  //: Constructor
  bgui3d_fullviewer_tableau(SoNode * scene_root = NULL);

  //: Destructor
  virtual ~bgui3d_fullviewer_tableau();

public:
  //: Handle vgui events
  virtual bool handle(const vgui_event& event);

  //: Return the type name of this tableau
  virtual vcl_string type_name() const = 0;

  //: Builds a popup menu
  virtual void get_popup(const vgui_popup_params&, vgui_menu &m);

};



#endif // bgui3d_fullviewer_tableau_h_
