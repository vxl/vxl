// This is core/vgui/impl/mfc/vgui_mfc_mainfrm.h
#ifndef AFX_VGUI_MFC_MAINFRM_H__B3175B74_0F91_44DD_8EFD_41CEA35852A1__INCLUDED_
#define AFX_VGUI_MFC_MAINFRM_H__B3175B74_0F91_44DD_8EFD_41CEA35852A1__INCLUDED_
//:
// \file
// \brief   Defines main window of the application
// \author  Marko Bacic, Oxford RRG
// \date    27 July 2000
//
// \verbatim
//  Modifications
//   22-AUG-2000 - Marko Bacic, Oxford RRG -- Added support for horizontal and
//                                            vertical scroll bars
// \endverbatim

#if defined(_MSC_VER)
#pragma once
#endif
#include "StdAfx.h"

class vgui_mfc_statusbar;

//: Defines main window of the application.
class vgui_mfc_mainfrm : public CFrameWnd
{
 protected:
  vgui_mfc_statusbar *statusbar;

 protected: // create from serialization only
  vgui_mfc_mainfrm();
  DECLARE_DYNCREATE(vgui_mfc_mainfrm)
 public:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual void process_menus(UINT nID);
 public:
  virtual ~vgui_mfc_mainfrm();
  virtual void SetStatusBar(vgui_mfc_statusbar *sb) {statusbar = sb;}
  virtual void UpdateStatusBar(CCmdUI *sb) {OnUpdateStatusBar(sb);}
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
 protected:
  // Manually added by awf
  afx_msg void OnClose();
  // Manually added by kym
  BOOL PreTranslateMessage(MSG* pMsg);

  //: Called when the window is created
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //: Called when a vertical scroll bar sends a signal
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
  //: Called when a horizontal scroll bar sends a signal
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
  //: Updates status bar
  afx_msg void OnUpdateStatusBar(CCmdUI *sbar);
  //: Called when specified timer has expired
  afx_msg void OnTimer(UINT_PTR);

  DECLARE_MESSAGE_MAP()
};

#endif // AFX_VGUI_MFC_MAINFRM_H__B3175B74_0F91_44DD_8EFD_41CEA35852A1__INCLUDED_
