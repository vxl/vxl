// This is ./oxl/vgui/impl/mfc/vgui_mfc_adaptor.cxx

//:
//  \file
// See vgui_mfc_adaptor.h for a description of this file

#include "vgui_mfc_adaptor.h"

#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_cstring.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/internals/vgui_overlay_helper.h>
#include <vgui/internals/vgui_accelerate.h>
#include <vgui/impl/mfc/vgui_mfc_utils.h>
#include <vgui/impl/mfc/vgui_mfc_mainfrm.h>

CDC *vgui_mfc_adaptor_global_dc;
static bool debug = false;
/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_adaptor

vgui_menu vgui_mfc_adaptor::last_popup;
IMPLEMENT_DYNCREATE(vgui_mfc_adaptor, CView)


//: Constructor.
vgui_mfc_adaptor::vgui_mfc_adaptor():ovl_helper(0), /*come_out_now(false),*/ redraw_posted(true)
{
  if (vgui_accelerate::vgui_mfc_acceleration)
    double_buffered = false; // kym - double buffering is not available with acceleration (it
                             // crashes windows).
  else
    double_buffered = true;

  // If m_pCWnd is not set (using setup_adaptor) assume we use the main window:
  m_pCWnd = 0;
  hOldDC = 0;
  hOldRC = 0;
}

//: Destructor.
//  kym - moved stuff here from OnDestroy because OnDestroy seems to only be
//  called when the main window is deleted.
vgui_mfc_adaptor::~vgui_mfc_adaptor()
{
  CView::OnDestroy();

  // This call makes the current RC not current
  if (FALSE == ::wglMakeCurrent(hOldDC, hOldRC))
    ::AfxMessageBox("wglMakeCurrent failed" );

  // delete the RC
  if ( m_hRC && (FALSE == ::wglDeleteContext( m_hRC )) )
  {
    ::AfxMessageBox("wglDeleteContext failed.");
  }

  // Release the device context:
  HDC m_hgldc = ::GetDC(m_hWnd);
  ::ReleaseDC(m_hWnd, m_hgldc);

  // delete the DC
  if ( m_pDC )
    delete m_pDC;
}

BEGIN_MESSAGE_MAP(vgui_mfc_adaptor, CView)
  ON_WM_CREATE()
  ON_WM_DESTROY()
  ON_WM_ERASEBKGND()
  ON_WM_SIZE()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEWHEEL()
  ON_WM_MBUTTONDOWN()
  ON_WM_MBUTTONUP()
  ON_WM_PAINT()
END_MESSAGE_MAP()


// 0. vgui_adaptor methods
//------------------------

//: MFC implementation of vgui_adaptor function - redraws overlay buffer.
void vgui_mfc_adaptor::post_overlay_redraw()
{
  redraw_posted = true;
}

//: MFC implementation of vgui_adaptor function - redraws rendering area.
void vgui_mfc_adaptor::post_redraw()
{
  if (!redraw_posted)
  {
    //CWnd *wnd = AfxGetApp()->GetMainWnd();
    CWnd* wnd;
    if (m_pCWnd != 0)
      wnd = m_pCWnd;
    else
      wnd = AfxGetApp()->GetMainWnd();
    if (wnd)
      wnd->Invalidate(FALSE);
  }
  redraw_posted = true;
}

//: MFC implementation of vgui_adaptor function - make this the current GL rendering context.
void vgui_mfc_adaptor::make_current()
{
  ::wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC );
}

//: MFC implementation of vgui_adaptor function - swap buffers if using double buffering.
void vgui_mfc_adaptor::swap_buffers()
{
  vgui_mfc_adaptor_global_dc = m_pDC;
  if(double_buffered)
    SwapBuffers(m_pDC->m_hDC);
}

//: Change the default popup menu to the given one (not yet implemented).
void vgui_mfc_adaptor::set_default_popup(vgui_menu)
{
  vcl_cerr << "vgui_mfc_adaptor::set_default_popup"<< vcl_endl;
}

//: Return the default popup menu (not yet implemented).
vgui_menu vgui_mfc_adaptor::get_popup()
{
  vcl_cerr<< "vgui_mfc_adaptor::get_popup"<< vcl_endl;
  return vgui_menu();
}

//: If your adaptor is not in the main window of the application call this function.
//  The first parameter tells it which window this adaptor is associated with,
//  the other parameters are so we can go back to our old rendering context.
void vgui_mfc_adaptor::setup_adaptor(CWnd* this_cwnd, HDC oldDC, HGLRC oldContext)
{
  m_pCWnd = this_cwnd;
  hOldDC = oldDC;
  hOldRC = oldContext;
}

#ifdef _DEBUG
void vgui_mfc_adaptor::AssertValid() const
{
  CView::AssertValid();
}

void vgui_mfc_adaptor::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}
#endif //_DEBUG

//: Called by MFC before the creation of the window attached to this object.
BOOL vgui_mfc_adaptor::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Add your specialized code here and/or call the base class
  // An OpenGL window must be created with the following
  // flags and must not include CS_PARENTDC for the
  // class style.

  cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

  return CView::PreCreateWindow(cs);
}

//: Called by MFC when the application requests the creation of a window.
//  This function must return 0 to continue the creation of the CWind
//  object, returning -1 destroys the window.
int vgui_mfc_adaptor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO: Add your specialized creation code here

  if(vgui_accelerate::vgui_mfc_acceleration)
  {
    CDC *tmp = new CClientDC(this);
    m_pDC = new CDC();
    m_pDC->CreateCompatibleDC(tmp);
    delete tmp;
    lpCreateStruct->cx = 1024;
    lpCreateStruct->cy = 768;
    BITMAPINFOHEADER bmi = {sizeof(BITMAPINFOHEADER),
      lpCreateStruct->cx,
      lpCreateStruct->cy,
      GetDeviceCaps(m_pDC->GetSafeHdc(),PLANES),
      GetDeviceCaps(m_pDC->GetSafeHdc(),BITSPIXEL),
      BI_RGB,/*lpCreateStruct->cx*lpCreateStruct->cy*4*/0,0,0,0,0};

    void *buffer;
    //: Create offscreen bitmap
    // You can draw on DIB sections with GDI, and you can directly modify the bits in memory.
    // This differs from device dependent bitmaps (on which only GDI can draw),
    // and DIBs (which don't support GDI but which you can modify directly).
    // DIB sections (especially large DIBs) can be blted to the screen faster than normal
    // DIBs and bitmaps.

    HBITMAP hbmp = CreateDIBSection(m_pDC->GetSafeHdc(),(BITMAPINFO *)&bmi,DIB_RGB_COLORS,&buffer,NULL,0);
    m_oldbitmap = (HBITMAP)::SelectObject(m_pDC->GetSafeHdc(),hbmp);
    double_buffered = false;
  }
  else
    m_pDC = new CClientDC(this);

  if ( NULL == m_pDC ) // failure to get DC
  {
    ::AfxMessageBox("Couldn't get a valid DC.");
    return FALSE;
  }
  vgui_mfc_adaptor_global_dc = m_pDC;
  if ( !SetupPixelFormat() )
  {
    ::AfxMessageBox("SetupPixelFormat failed.\n");
    return FALSE;
  }

  if ( 0 == (m_hRC =
    ::wglCreateContext( m_pDC->GetSafeHdc() ) ) )
  {
    ::AfxMessageBox("wglCreateContext failed.");
    return FALSE;
  }

  if ( FALSE ==
    ::wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC ) )
  {
    ::AfxMessageBox("wglMakeCurrent failed.");
    return FALSE;
  }

  // specify black as clear color
  ::glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  // specify the back of the buffer as clear depth
  ::glClearDepth( 1.0f );
  // enable depth testing
  ::glEnable( GL_DEPTH_TEST );

  if (0) {
    m_pDC->SetStretchBltMode(HALFTONE);
    ::SetBrushOrgEx(m_pDC->GetSafeHdc(), 0, 0, 0);
  } else {
    m_pDC->SetStretchBltMode(COLORONCOLOR);
  }

  post_redraw();

  return 0;
}

BOOL vgui_mfc_adaptor::SetupPixelFormat()
{
  // default pixel format for a single-buffered,
  // OpenGL-supporting, hardware-accelerated,
  // RGBA-mode format. Pass in a pointer to a different
  // pixel format if you want something else

  PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof(PIXELFORMATDESCRIPTOR),// size of this pfd
      1,                      // version number
      PFD_DOUBLEBUFFER |
      PFD_DRAW_TO_WINDOW |    // support window
      PFD_SUPPORT_OPENGL,     // support OpenGL
      PFD_TYPE_RGBA,          // RGBA type
      GetDeviceCaps(m_pDC->GetSafeHdc(),BITSPIXEL)* //: color depth
      GetDeviceCaps(m_pDC->GetSafeHdc(),PLANES),
      0, 0, 0, 0, 0, 0,       // color bits ignored
      0,                      // no alpha buffer
      0,                      // shift bit ignored
      0,                      // no accumulation buffer
      0, 0, 0, 0,             // accum bits ignored
      32,                     // 32-bit z-buffer
      0,                      // no stencil buffer
      0,                      // no auxiliary buffer
      PFD_MAIN_PLANE,         // main layer
      0,                      // reserved
      0, 0, 0                 // layer masks ignored
  };

  if (vgui_accelerate::vgui_mfc_acceleration)
    pfd.dwFlags = PFD_DRAW_TO_BITMAP|PFD_SUPPORT_OPENGL;

  PIXELFORMATDESCRIPTOR* pPFDtoUse;
  // let the user override the default pixel format
  pPFDtoUse = &pfd; //(0 == pPFD)? &pfd : pPFD;

  int pixelformat = ::ChoosePixelFormat( m_pDC->GetSafeHdc(), pPFDtoUse );

  if (0 == pixelformat) {
    ::AfxMessageBox("ChoosePixelFormat failed.");
    return FALSE;
  }

  if ( FALSE == ::SetPixelFormat( m_pDC->GetSafeHdc(), pixelformat, pPFDtoUse ) )
  {
    ::AfxMessageBox("SetPixelFormat failed.");
    vcl_cerr<<"Error code:"<<GetLastError();
    ASSERT(0);
    return FALSE;
  }

  return TRUE;
}

//: Called by MFC when the window has been destroyed.
void vgui_mfc_adaptor::OnDestroy()
{
  // kym - moved stuff to the destructor because this only seems
  // to be called when the main window is destroyed.
}

//: Called by MFC when the background needs erasing.
//  For example this would be called if the window was resized.
BOOL vgui_mfc_adaptor::OnEraseBkgnd(CDC* pDC)
{
  // don't clear - gl will do it.
  return TRUE;
}

//: Redraws the OpenGL area.
void vgui_mfc_adaptor::service_redraws()
{
  if (redraw_posted)
  {
    if(double_buffered)  // kym - changed from if(!double_buffered) - why change the buffer otherwise?
      glDrawBuffer(GL_BACK);
    dispatch_to_tableau(vgui_event(vgui_DRAW));
    if(!double_buffered)
    {
      //CWnd *wnd = AfxGetApp()->GetMainWnd();
      CWnd* wnd;
      if (m_pCWnd)
        wnd = m_pCWnd;
      else
        wnd = AfxGetApp()->GetMainWnd();
      CDC *win_dc = wnd->GetDC();
      RECT r;
      wnd->GetClientRect(&r);
      win_dc->BitBlt(0,0,r.right,r.bottom,vgui_mfc_adaptor_global_dc,0,0,SRCCOPY);
    }

    swap_buffers();
    redraw_posted = false;
  }
}

//: Sets the timer to dispatch WM_TIME event to a mainframe every time miliseconds
void vgui_mfc_adaptor::post_timer(float tm,int id)
{
  //CWnd *wnd = AfxGetApp()->GetMainWnd();
  CWnd* wnd;
  if (m_pCWnd)
    wnd = m_pCWnd;
  else
    wnd = AfxGetApp()->GetMainWnd();
  wnd->SetTimer(id,tm,NULL);
}

//: Called by MFC when a draw event is required - overridden to draw this view.
void vgui_mfc_adaptor::OnDraw(CDC* pDC)
{
  if(debug)
    vcl_cerr << "OnDraw" << vcl_endl;
  // post_redraw();

  service_redraws();
  // CView::OnDraw(pDC);
}

//: MFC implementation of vgui_adaptor function - redraw everything now!
void vgui_mfc_adaptor::draw()
{
  post_redraw();
  service_redraws();
}

//: Called by MFC when the application requests part of the window is redrawn.
void vgui_mfc_adaptor::OnPaint()
{
  // if i get a paint that isn't due to my posting a redraw, it must be an expose?
  redraw_posted = true; // ensure that draw does something
  // don't call 'post_redraw' as that invalidates the window again...

  CView::OnPaint();
}

//: Called by MFC when the application is resized.
void vgui_mfc_adaptor::OnSize(UINT nType, int cx, int cy)
{
  CView::OnSize(nType, cx, cy);

  m_width = cx;
  m_height = cy;
  if(cx && cy)
  {
    BITMAPINFOHEADER bmi = {sizeof(BITMAPINFOHEADER),
      cx,
      cy,
      GetDeviceCaps(m_pDC->GetSafeHdc(),PLANES),
      GetDeviceCaps(m_pDC->GetSafeHdc(),BITSPIXEL),
      BI_RGB,/*lpCreateStruct->cx*lpCreateStruct->cy*4*/0,0,0,0,0};

    void *buffer;
    //: Create offscreen bitmap
    // You can draw on DIB sections with GDI, and you can directly modify the bits in memory.
    // This differs from device dependent bitmaps (on which only GDI can draw),
    // and DIBs (which don't support GDI but which you can modify directly).
    // DIB sections (especially large DIBs) can be blted to the screen faster than normal
    // DIBs and bitmaps.

    HBITMAP hbmp = CreateDIBSection(m_pDC->GetSafeHdc(),(BITMAPINFO *)&bmi,DIB_RGB_COLORS,&buffer,
      NULL,0);
    m_oldbitmap = (HBITMAP)SelectObject(m_pDC->GetSafeHdc(),hbmp);
    DeleteObject(m_oldbitmap);
  }
  post_redraw();
}

//: Convert MFC key character into an int suitable for vgui.
int mfc_key(UINT nChar, UINT nFlags)
{
  if (nFlags & 256) {
    // Extended code
    switch (nChar) {
    case VK_NEXT: return vgui_PAGE_DOWN;
    case VK_PRIOR: return vgui_PAGE_UP;
    case VK_LEFT: return vgui_CURSOR_LEFT;
    case VK_UP: return vgui_CURSOR_UP;
    case VK_RIGHT: return vgui_CURSOR_RIGHT;
    case VK_DOWN: return vgui_CURSOR_DOWN;
    default: return vgui_key(0);
    };
  } else {
    unsigned short buf[1024];
    unsigned char lpKeyState[256];
    vcl_memset(lpKeyState, 0, 256);
    vcl_memset(buf, 0, 256);

    lpKeyState[VK_SHIFT] = GetKeyState(VK_SHIFT);
    lpKeyState[VK_CONTROL] = GetKeyState(VK_CONTROL);
    int k= ToAscii(nChar, nFlags & 0xff, lpKeyState, buf, 0);

    int c = nChar;
    if (k == 1)
      c = buf[0];

    return c;
  }
}

//: Create the corresponding vgui_event from an MFC event.
vgui_event vgui_mfc_adaptor::generate_vgui_event(UINT nChar, UINT nRepCnt, UINT nFlags, vgui_event_type evttype)
{
  vgui_event evt(evttype);
  if (GetKeyState(VK_SHIFT) & 0x8000)
    evt.modifier = vgui_SHIFT;
  if (GetKeyState(VK_CONTROL) & 0x8000)
    evt.modifier = vgui_CTRL;

  int k = mfc_key(nChar, nFlags);
  evt.key = vgui_key(k);
  return evt;
}

//: Called by MFC when a key is pressed inside the application.
void vgui_mfc_adaptor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   // Ignore Ctrl and Shift pressed alone:
  if (nChar == VK_SHIFT || nChar == VK_CONTROL)
    return;

  dispatch_to_tableau(generate_vgui_event(nChar, nRepCnt, nFlags, vgui_KEY_PRESS));
  service_redraws();
}

//: Called by MFC when a key is released inside the application.
void vgui_mfc_adaptor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  // Ignore Ctrl and Shift pressed alone:
  if (nChar == VK_SHIFT || nChar == VK_CONTROL)
    return;

  dispatch_to_tableau(generate_vgui_event(nChar, nRepCnt, nFlags, vgui_KEY_RELEASE));
  service_redraws();
}

//: Handles mouse press/release events.
void vgui_mfc_adaptor::domouse(vgui_event_type et, UINT nFlags, CPoint point, vgui_button b)
{
  //vcl_cerr << "vgui_mfc_adaptor::domouse: wo = " << point.x << ", " << point.y << vcl_endl;
  // awf: BLETCH. This offset is consistent over resize, depth, screen position, machines,
  // and I can't find it... Sorry.
  point.x += 2;
  point.y += 2;
  // FIXME

  //come_out_now = true;
  vgui_event e(et);

  e.button = b;
  if (nFlags & MK_LBUTTON) e.button = vgui_LEFT;
  if (nFlags & MK_MBUTTON) e.button = vgui_MIDDLE;
  if (nFlags & MK_RBUTTON) e.button = vgui_RIGHT;
  if (nFlags & MK_SHIFT)   e.modifier = vgui_modifier((int)e.modifier | vgui_SHIFT);
  if (nFlags & MK_CONTROL) e.modifier = vgui_modifier((int)e.modifier | vgui_CTRL);
  e.wx = point.x;
  e.wy = m_height - point.y;
  //: Deals with right popup menu
  if(e.modifier == mixin::popup_modifier  && e.button == mixin::popup_button)
  {
    vgui_popup_params params;
    params.x = point.x;
    params.y = point.y;
    last_popup = get_total_popup(params);
    CMenu *popup = vgui_mfc_utils::instance()->set_popup_menu(last_popup);
    //CWnd *wnd = AfxGetApp()->GetMainWnd();
    CWnd* wnd;
    if (m_pCWnd)
      wnd = m_pCWnd;
    else
      wnd = AfxGetApp()->GetMainWnd();
    //: 'point' is window coordinates whereas TrackPopup menu requires screen coordinates.
    // So translate them into screen coordinates
    ClientToScreen(&point);
    popup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,wnd);
    delete popup;
  }

  dispatch_to_tableau(e);

  // Grab mouse?
  {
    if (et == vgui_BUTTON_DOWN) {
      SetCapture();
    } else if (et != vgui_MOTION) {
      ReleaseCapture();
    }
  }
}

//: Called by MFC when the left mouse button is pressed inside the application.
void vgui_mfc_adaptor::OnLButtonDown(UINT nFlags, CPoint point)
{
  domouse(vgui_BUTTON_DOWN, nFlags, point, vgui_LEFT);
}

//: Called by MFC when the left mouse button is released inside the application.
void vgui_mfc_adaptor::OnLButtonUp(UINT nFlags, CPoint point)
{
  domouse(vgui_BUTTON_UP, nFlags, point, vgui_LEFT);
}

//: Called by MFC when the middle mouse button is pressed inside the application.
void vgui_mfc_adaptor::OnMButtonDown(UINT nFlags, CPoint point)
{
  domouse(vgui_BUTTON_DOWN, nFlags, point, vgui_MIDDLE);
}

//: Called by MFC when the middle mouse button is released inside the application.
void vgui_mfc_adaptor::OnMButtonUp(UINT nFlags, CPoint point)
{
  domouse(vgui_BUTTON_UP, nFlags, point, vgui_MIDDLE);
}

//: Called by MFC when the right mouse button is pressed inside the application.
void vgui_mfc_adaptor::OnRButtonDown(UINT nFlags, CPoint point)
{
  domouse(vgui_BUTTON_DOWN, nFlags, point, vgui_RIGHT);
}

//: Called by MFC when the right mouse button is released inside the application.
void vgui_mfc_adaptor::OnRButtonUp(UINT nFlags, CPoint point)
{
  domouse(vgui_BUTTON_UP, nFlags, point, vgui_RIGHT);
}

//: Called by MFC when the mouse is moved inside the application.
void vgui_mfc_adaptor::OnMouseMove(UINT nFlags, CPoint point)
{
  domouse(vgui_MOTION, nFlags, point, vgui_BUTTON_NULL);
}

//: Called by MFC when a user rotates a mouse wheel.
BOOL vgui_mfc_adaptor::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  //vcl_cerr << "Mouse wheel events are not handled" << vcl_endl;
  return FALSE;
}
