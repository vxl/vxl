#ifndef xcv_mview_manager_h_
#define xcv_mview_manager_h_

//--------------------------------------------------------------------------------
// .NAME	xcv_mview_manager
// .INCLUDE	xcv/xcv_mview_manager.h
// .FILE	xcv_mview_manager.cxx
// .SECTION Description:
//
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications:
//   K.Y.McGaul     05-MAY-2000    Initial version.
//--------------------------------------------------------------------------------

class vgui_event;
struct vgui_tableau_ref;

class xcv_mview_manager
{
public:
  virtual ~xcv_mview_manager() { } // default dtor would be non-virtual.

  virtual void handle_tjunction_event(vgui_event const& e, vgui_tableau_ref const& child_tab) = 0;
};

#endif // xcv_mview_manager_
