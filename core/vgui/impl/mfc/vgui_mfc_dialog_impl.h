// This is core/vgui/impl/mfc/vgui_mfc_dialog_impl.h
#ifndef vgui_mfc_dialog_impl_h_
#define vgui_mfc_dialog_impl_h_
//:
// \file
// \author  Marko Bacic, RRG, University of Oxford
// \date    31 Jul 2000
// \brief   The MFC implementation of vgui_dialog_impl.
//
// \verbatim
//  Modifications
//   31-JUL-2000 Marko Bacic - Initial version.
//   03-MAR-2001 K.Y.McGaul - Added correct modifiers to key press/release events
//                          - Set checkboxes to display current value correctly
//   22-AUG-2001 K.Y.McGaul - Fixed memory leak: font is now deleted.
//   19-SEP-2001 K.Y.McGaul & Marko Bacic - Finally got inline tableau working!
//   16-NOV-2007 Gamze Tunali - private methods are now protected
//                              vgui_mfc_dialog_extensions_impl is inheriting from
// \endverbatim

class CFont;

#include <vgui/internals/vgui_dialog_impl.h>
#include "StdAfx.h"
#undef MAX_ITEMS // also defined in vgui_mfc_dialog_extensions_impl.h
#define MAX_ITEMS 255
// MFC documentation says that our IDs should be in the range 0x8000-0xDFF
#undef ID_BROWSE_FILES // also defined in vgui_mfc_dialog_extensions_impl.h
#define ID_BROWSE_FILES 0x8000  // Assume that there won't be more than 100 browser buttons
#undef ID_CHOOSE_COLOUR // also defined in vgui_mfc_dialog_extensions_impl.h
#define ID_CHOOSE_COLOUR 0x8100 // Assume that there won't be more than 100 color chooser buttons
#undef ID_EDIT // also defined in vgui_mfc_dialog_extensions_impl.h
#define ID_EDIT 0x8200
#undef ID_COMBOBOX // also defined in vgui_mfc_dialog_extensions_impl.h
#define ID_COMBOBOX 0x8300

//: The MFC implementation of vgui_dialog_impl.
//
//  Creates a dialog box in MFC.
//  Based on vgui_gtk_dialog_impl.
class vgui_mfc_dialog_impl : public CDialog,public vgui_dialog_impl
{
 public:
  vgui_mfc_dialog_impl(const char* name);
  ~vgui_mfc_dialog_impl();

  void* choice_field_widget(const char*, const std::vector<std::string>&, int&);
  void* inline_tableau_widget(const vgui_tableau_sptr tab, unsigned width, unsigned height);
  //: Sets the modality of the dialog box.
  // True makes the dialog modal (i.e. the dialog 'grabs' all events), this is
  // the default.  False makes the dialog non-modal. WARNING: It is dangerous to
  // make a dialog that changes data nonmodal, only messages should be nonmodal.
  // NOTE: currently a no-op
  void modal(bool ) {}
  //: Display the dialog box.
  bool ask();
 protected:
  LOGFONT m_logfont;
  //: Called by MFC when the user clicks the OK button.
  virtual void OnOk();
  //: Called by MFC when the user clicks the cancel button.
  virtual void OnCancel();
  //: Called by MFC when the user clicks the (file) browse button.
  virtual void OnBrowse(UINT uID);
  //: Called by MFC when the user clicks the colour chooser button.
  virtual void OnChooseColour(UINT uID);
  //: Called by MFC when the application is about to terminate.
  afx_msg void OnClose();
 protected:
  int nResult;
  //: File browser counter.
  int count_fbsr;
  //: Colour chooser counter.
  int count_csr;
  bool ok_clicked;
  //: Array of MFC file browser objects.
  CWnd *fbsrs[100];
  //: Array of MFC colour chooser objects.
  CWnd *csrs[100];
  //: MFC dialog box object.
  CWnd *dialog_box;
  std::vector<CWnd *> wlist;
  //: List of created MFC objects (so we can delete them).
  std::vector<CWnd *> awlist;
  DECLARE_MESSAGE_MAP()
  //: Type font.
  CFont* font;
};

#endif // vgui_mfc_dialog_impl_h_
