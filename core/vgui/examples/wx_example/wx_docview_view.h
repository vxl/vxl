// This is core/vgui/examples/wx_example/wx_docview_view.h
#ifndef wx_docview_view_h_
#define wx_docview_view_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets view class.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   03/19/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <wx/docview.h>
class wx_docview_frame;

#include <vgui/impl/wx/vgui_wx_adaptor.h>

//-------------------------------------------------------------------------
//: The wxWidgets view class.
//-------------------------------------------------------------------------
class wx_docview_view : public wxView
{
  DECLARE_DYNAMIC_CLASS(wx_docview_view)
  DECLARE_EVENT_TABLE()

 public:
  wx_docview_view();
  virtual ~wx_docview_view();

  virtual bool OnCreate(wxDocument* doc, long flags);
  virtual void OnDraw(wxDC* dc);
  virtual void OnUpdate(wxView* sender, wxObject* hint = 0);
  virtual bool OnClose(bool delete_window = true);

 private:
  wx_docview_frame* frame_;
  vgui_wx_adaptor*  canvas_;
};

#endif // wx_docview_view_h_
