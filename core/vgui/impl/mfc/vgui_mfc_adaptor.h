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
public:
	typedef vgui_adaptor_mixin mixin;
	// 0. vgui_adaptor implementations
	unsigned int get_width() const { return m_width; }
	unsigned int get_height() const { return m_height; }
	void post_redraw();
	void post_overlay_redraw();
	void swap_buffers();
	void make_current();
	void draw();
	// 0.5 fsm stuff
	void service_redraws();
	bool come_out_now;
	bool redraw_posted;
	void set_default_popup(vgui_menu);
        void post_timer(float,int);
	vgui_menu get_popup();
  void bind_popups(vgui_modifier m, vgui_button b) {
    mixin::popup_modifier = m;
    mixin::popup_button = b;
  }
  void get_popup_bindings(vgui_modifier &m,vgui_button &b) const {
    m = mixin::popup_modifier;
    b = mixin::popup_button;
  }
	// 1. MFC stuff
	// Overrides
	// ClassWizard generated virtual function overrides

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
// Implementation
protected:
	virtual ~vgui_mfc_adaptor();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	void domouse(vgui_event_type e, UINT nFlags, CPoint point, vgui_button b);
	HGLRC m_hRC;
	BOOL SetupPixelFormat();
	int m_width;
        int m_height;
        HBITMAP m_oldbitmap;
        bool double_buffered;
	vgui_overlay_helper *ovl_helper;
	static vgui_menu last_popup;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_VGUI_MFC_ADAPTOR_H__6BE405F7_0910_4C83_A175_5E8FBDCE88A5__INCLUDED_
