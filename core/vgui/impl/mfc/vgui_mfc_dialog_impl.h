#ifndef vgui_mfc_dialog_impl_h_
#define vgui_mfc_dialog_impl_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_mfc_dialog_impl - Provides support for a dialog box
// .LIBRARY vgui-mfc
// .HEADER vxl Package
// .INCLUDE vgui/impl/mfc/vgui_mfc_dialog_impl.h
// .FILE vgui_mfc_dialog_impl.cxx
//
// .SECTION Description:
//
//   Specialization of vgui_dialog_impl for mfc. Creates a mfc dialog box.
//   Based on vgui_gtk_dialog_impl
//
// .SECTION Author:
//              Marko Bacic, 31 Jul 2000
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//-----------------------------------------------------------------------------

#include <vgui/internals/vgui_dialog_impl.h>
#include "stdafx.h"
#define MAX_ITEMS 255
// -- MFC documentation says that our IDs should be in the range 0x8000-0xDFF
#define ID_BROWSE_FILES 0x8000  // -- Assume that there won't be more than 100 browser buttons
#define ID_CHOOSE_COLOUR 0x8100 // -- Assume that there won't be more than 100 color chooser buttons
#define ID_EDIT 0x8200
#define ID_COMBOBOX 0x8300

class vgui_mfc_dialog_impl : public CWnd,public vgui_dialog_impl {
public:
  vgui_mfc_dialog_impl(const char* name);
  ~vgui_mfc_dialog_impl();
  
  void* choice_field_widget(const char*, const vcl_vector<vcl_string>&, int&);

  void modal(const bool);
  bool ask();
protected:
  LOGFONT m_logfont;
  virtual void OnOk();
  virtual void OnCancel();
  virtual void OnBrowse(UINT uID);
  virtual void OnChooseColour(UINT uID);
  afx_msg void OnClose();
private:
  int nResult;
  int count_fbsr;
  int count_csr;
  bool ok_clicked;
  CWnd *fbsrs[100];
  CWnd *csrs[100];
  CWnd *dialog_box;
  vcl_vector<CWnd *> wlist;
  vcl_vector<CWnd *> awlist;
  DECLARE_MESSAGE_MAP()
};

#endif // vgui_mfc_dialog_impl_h_
