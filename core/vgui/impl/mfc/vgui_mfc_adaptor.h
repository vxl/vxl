// This is core/vgui/impl/mfc/vgui_mfc_adaptor.h
#ifndef AFX_VGUI_MFC_ADAPTOR_H__6BE405F7_0910_4C83_A175_5E8FBDCE88A5__INCLUDED_
#define AFX_VGUI_MFC_ADAPTOR_H__6BE405F7_0910_4C83_A175_5E8FBDCE88A5__INCLUDED_
//:
// \file
// \brief  The MFC implementation of vgui_adaptor.
// \author RRG, Oxford University
//
//  Contains class vgui_mfc_adaptor
//
// \verbatim
//  Modifications
//   13-08-2000  Marko Bacic, Oxford RRG - Fixed textures
//   14-08-2000  FSM, Oxford RRG - Fixed double buffering/rubber banding issues
//   14-08-2000  Marko Bacic, Oxford RRG - Added right popup menu
//   30-08-2000  Marko Bacic, Oxford RRG - Support for Windows/MFC acceleration
//   06-02-2001  AWF, Oxford RRG - Make acceleration work...
//   02-03-2001  K.Y.McGaul - Add shift & ctrl modifiers to key press/release
//                            events.
//                          - Edited and added Doxygen comments.
//   14-09-2001  K.Y.McGaul - Moved stuff in OnDestroy to destructor.
//   18-09-2001  K.Y.McGaul - Major changes to allow for multiple adaptors in
//                            an application.  In particular using
//                            setup_adaptor we can now specify which window
//                            our adaptor is in.  If you don't call
//                            setup_adaptor it is assumed to be the main
//                            window.
//   07-07-2003  M.R.Johnson - Disabled MFC acceleration by default so that new
//                             overlay code will work (which requires double
//                             buffering). Overlays necessary to enable new
//                             highlighting mechanism in vgui tableaux.
//   07-21-2003  A.Perera   - Update the overlay mechanism for Windows, since copying
//                            the frame buffer to memory is *slow*.
// \endverbatim

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_gl.h>
#include <vgui/internals/vgui_adaptor_mixin.h>

//: MFC implementation of vgui_adaptor.
//
//  The adaptor allows you to draw an OpenGL area in your MFC application.
//
//  If this adaptor is not being used inside the main window of the application
//  then you will need to call setup_adaptor().  See vgui_mfc_dialog_impl where
//  this is done, when an OpenGL area is used in a dialog box.
class vgui_mfc_adaptor : public CView, public vgui_adaptor, public vgui_adaptor_mixin
{
 public:
  //: Constructor used by dynamic creation.
  //  kym - this used to be protected, but I changed it to public so I can add
  //  adaptors to dialog boxes.
  vgui_mfc_adaptor();
  DECLARE_DYNCREATE(vgui_mfc_adaptor)

  //: Device context for this adaptor.
  CDC* m_pDC;

  typedef vgui_adaptor_mixin mixin;

  // 0. vgui_adaptor implementations
  //--------------------------------
  //: MFC implementation from vgui_adaptor -  returns width of rendering area.
  unsigned int get_width() const { return m_width; }

  //: MFC implementation from vgui_adaptor - returns height of rendering area.
  unsigned int get_height() const { return m_height; }

  //: MFC implementation from vgui_adaptor - redraws rendering area.
  void post_redraw();

  //: MFC implementation from vgui_adaptor - redraws overlay buffer.
  void post_overlay_redraw();

  //: MFC impl. from vgui_adaptor - swap buffers if using double buffering.
  void swap_buffers();

  //: MFC impl. from vgui_adaptor - make this the current GL rendering context.
  void make_current();

  //: MFC implementation from vgui_adaptor  - redraw everything now.
  void draw();

  // Do any idle processing that needs to be done.
  // Return true if idle processing is not complete
  bool do_idle();

  // 0.5 fsm stuff
  //--------------
  //: Redraws the OpenGL area.
  void service_redraws();

  //: Change the default popup menu to the given one (not yet implemented).
  void set_default_popup(vgui_menu);

  //: Sets timer to dispatch WM_TIME event to a mainframe every time milliseconds
  void post_timer(float,int);

  //: Flags than a child requests idle processing
  void post_idle_request() { idle_request_posted_ = true; }

  //: Return the default popup menu (not yet implemented).
  vgui_menu get_popup();

  //: MFC impl. from vgui_adaptor - set button & modifier to display popup.
  void bind_popups(vgui_modifier m, vgui_button b) {
    mixin::popup_modifier = m;
    mixin::popup_button = b;
  }

  //: MFC impl. from vgui_adaptor - get button & modifier which display popup.
  void get_popup_bindings(vgui_modifier &m,vgui_button &b) const {
    m = mixin::popup_modifier;
    b = mixin::popup_button;
  }

  virtual vgui_window* get_window() const { return win_; }
  void set_window( vgui_window* win ) { win_ = win; }

  // kym stuff
  //----------
  //:
  //  If your adaptor is not in the main window of the application call this
  //  function. The first parameter tells it which window this adaptor is
  //  associated with, the other parameters are so we can go back to our old
  //  rendering context when this adaptor is deleted.
  void setup_adaptor(CWnd* this_cwnd, HDC OldDC, HGLRC oldContext);


 private:
  void create_bitmap( int cx, int cy,
                      CDC*& out_pDC,
                      HBITMAP& out_old_hbmp );

  HGLRC setup_for_gl( CDC* pDC, DWORD dwFlags );

  //: Device context for the auxiliary buffer used for simulating overlays
  CDC* m_pDC_aux;
  //: True if the aux buffer is a copy of the main GL buffer
  bool aux_dc_valid_;

  //: True while a redraw event has been requested but not implemented.
  bool redraw_posted_;

  //: True while a overlay redraw event has been requested but not implemented.
  bool overlay_redraw_posted_;

  //: True while an idle time has been requested but not implemented.
  bool idle_request_posted_;

 protected:
  //: The window associated with this adaptor if it is not the main window.
  CWnd* m_pCWnd;

  //: The previous device context to this one.
  HDC hOldDC;

  //: The previous rendering context to this one.
  HGLRC hOldRC;

  // 1. MFC stuff -  ClassWizard generated virtual function overrides
  //-------------
 protected:
  //: Called by MFC when a draw event is required - overridden to draw this view
  virtual void OnDraw(CDC* pDC);

  //: Called by MFC before the creation of the window attached to this object.
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
 public:
  virtual ~vgui_mfc_adaptor();
 protected:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
  //--------------------------------
  //: Handles mouse press/release events.
  void domouse(vgui_event_type e, UINT nFlags, CPoint point, vgui_button b);

  //: The Win-OpenGL resource context.
  HGLRC m_hRC;

  //: Width of rendering area.
  int m_width;

  //: Height of rendering area.
  int m_height;

  vgui_window* win_;

  static vgui_menu last_popup;

  //: Called by MFC when the application requests the creation of a window.
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

  //: Called by MFC when the main window has been destroyed.
  //  Note, this function is not called when your adaptor is destroyed
  //  inside a non-main window (eg. a dialog box).
  afx_msg void OnDestroy();

  //: Called by MFC when the application requests part of the window is redrawn.
  afx_msg void OnPaint();

  //: Called by MFC when the background needs erasing.
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);

  //: Called by MFC when the application is resized.
  afx_msg void OnSize(UINT nType, int cx, int cy);

  //: Create the corresponding vgui_event from an MFC event.
  vgui_event generate_vgui_event(UINT nChar, UINT nRepCnt, UINT nFlags, vgui_event_type evttype);

  //: Called by MFC when a key is pressed inside the application.
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

  //: Called by MFC when a key is released inside the application.
  afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

  //: Called by MFC when the left mouse button is pressed inside the application
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

  //: Called by MFC when left mouse button is released inside the application.
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

  //: Called by MFC when the mouse is moved inside the application.
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);

  //: Called by MFC when right mouse button is pressed inside the application.
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

  //: Called by MFC when right mouse button is released inside the application.
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

  //: Called by MFC when a user rotates a mouse wheel.
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

  //: Called by MFC when middle mouse button is pressed inside the application.
  afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

  //: Called by MFC when middle mouse button is released inside the application.
  afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_VGUI_MFC_ADAPTOR_H__6BE405F7_0910_4C83_A175_5E8FBDCE88A5__INCLUDED_
