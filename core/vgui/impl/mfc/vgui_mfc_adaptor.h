#ifndef AFX_VGUI_MFC_ADAPTOR_H__6BE405F7_0910_4C83_A175_5E8FBDCE88A5__INCLUDED_
#define AFX_VGUI_MFC_ADAPTOR_H__6BE405F7_0910_4C83_A175_5E8FBDCE88A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// vgui_mfc_adaptor.h : header file
// .LIBRARY vgui-mfc
//

#include <afxwin.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_adaptor_mixin.h>
/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_adaptor view
struct vgui_overlay_helper;
class vgui_mfc_adaptor : public CView, public vgui_adaptor, public vgui_adaptor_mixin
{
protected:
        vgui_mfc_adaptor();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(vgui_mfc_adaptor)

// Attributes
public:
        CDC* m_pDC;

// Operations
        typedef vgui_adaptor_mixin mixin;

        // 0. vgui_adaptor implementations
  //--------------------------------
  //: MFC implementation of vgui_adaptor function - returns width of rendering area.
        unsigned int get_width() const { return m_width; }
  //: MFC implementation of vgui_adaptor function - returns height of rendering area.
        unsigned int get_height() const { return m_height; }
  //: MFC implementation of vgui_adaptor function - redraws rendering area.
        void post_redraw();
  //: MFC implementation of vgui_adaptor function - redraws overlay buffer.
        void post_overlay_redraw();
  //: MFC implementation of vgui_adaptor function - swap buffers if using double buffering.
        void swap_buffers();
  //: MFC implementation of vgui_adaptor function -
        void make_current();
  //: MFC implementation of vgui_adaptor function - redraw everything now.
        void draw();

        // 0.5 fsm stuff
  //--------------
  //: Redraws the OpenGL area.
        void service_redraws();
        //bool come_out_now;
  //: True while a redraw event has been requested but not implemented.
        bool redraw_posted;
  //: Change the default popup menu to the given one (not yet implemented).
        void set_default_popup(vgui_menu);
  //: Sets the timer to dispatch WM_TIME event to a mainframe every time miliseconds
  void post_timer(float,int);
  //: Return the default popup menu (not yet implemented).
        vgui_menu get_popup();
  //: MFC implementation of vgui_adaptor function - set button & modifier to display popup.
  void bind_popups(vgui_modifier m, vgui_button b) {
    mixin::popup_modifier = m;
    mixin::popup_button = b;
  }
  //: MFC implementation of vgui_adaptor function - get button & modifier which display popup.
  void get_popup_bindings(vgui_modifier &m,vgui_button &b) const {
    m = mixin::popup_modifier;
    b = mixin::popup_button;
  }

        // 1. MFC stuff
  //-------------
        // ClassWizard generated virtual function overrides
protected:
  //: Called by MFC when a draw event is required - overridden to draw this view.
        virtual void OnDraw(CDC* pDC);
  //: Called by MFC before the creation of the window attached to this object.
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
        virtual ~vgui_mfc_adaptor();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

        // Generated message map functions
  //--------------------------------
  //: Handles mouse press/release events.
        void domouse(vgui_event_type e, UINT nFlags, CPoint point, vgui_button b);
        HGLRC m_hRC;
        BOOL SetupPixelFormat();
  //: Width of rendering area.
        int m_width;
  //: Height of rendering area.
  int m_height;
  HBITMAP m_oldbitmap;
  //: Whether or not double buffering is being used.
  bool double_buffered;
        vgui_overlay_helper *ovl_helper;
        static vgui_menu last_popup;
  //: Called by MFC when the application requests the creation of a window.
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //: Called by MFC when the window has been destroyed.
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
  //: Called by MFC when the left mouse button is pressed inside the application.
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  //: Called by MFC when the left mouse button is released inside the application.
        afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  //: Called by MFC when the mouse is moved inside the application.
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  //: Called by MFC when the right mouse button is pressed inside the application.
        afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  //: Called by MFC when the right mouse button is released inside the application.
        afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  //: Called by MFC when a user rotates a mouse wheel.
        afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  //: Called by MFC when the middle mouse button is pressed inside the application.
        afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
  //: Called by MFC when the middle mouse button is released inside the application.
        afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_VGUI_MFC_ADAPTOR_H__6BE405F7_0910_4C83_A175_5E8FBDCE88A5__INCLUDED_
