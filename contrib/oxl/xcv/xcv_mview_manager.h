// This is oxl/xcv/xcv_mview_manager.h
#ifndef xcv_mview_manager_h_
#define xcv_mview_manager_h_
//:
// \file
// \author  K.Y.McGaul
// \brief   Base class for multiview managers.
//
// \verbatim
//  Modifications:
//   K.Y.McGaul     05-MAY-2000    Initial version.
// \endverbatim

class vgui_event;
struct vgui_tableau_sptr;

class xcv_mview_manager
{
 public:
  //: Destructor (default dtor would be non-virtual).
  virtual ~xcv_mview_manager() { }

  //: Handle all events sent to this manager.
  virtual void handle_tjunction_event(vgui_event const& e, vgui_tableau_sptr const& child_tab) = 0;
};

#endif // xcv_mview_manager_h_
