// This is core/vgui/impl/mfc/vgui_mfc_adaptor.cxx
#include "vgui_mfc_adaptor.h"
//:
// \file
// \author RRG, Oxford University
// \brief  See vgui_mfc_adaptor.h for a description of this file.
//
// \verbatim
//  Modifications
//   06-AUG-2002 K.Y.McGaul - Changed key returned by CTRL events.
//   04-OCT-2002 K.Y.McGaul - Use event.set_key() to set key, this makes sure
//                            the key stroke is always lower case.
//                          - Set event.ascii_char to be the actual key stroke.
// \endverbatim

#include <vcl_iostream.h>
#include <vcl_cstring.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/vgui_macro.h>
#include <vgui/impl/mfc/vgui_mfc_utils.h>
#include <vgui/impl/mfc/vgui_mfc_mainfrm.h>

static bool debug = false;
extern bool vgui_mfc_use_bitmap;
/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_adaptor

vgui_menu vgui_mfc_adaptor::last_popup;
IMPLEMENT_DYNCREATE(vgui_mfc_adaptor, CView)


//: Constructor.
vgui_mfc_adaptor::vgui_mfc_adaptor( )
  : m_pDC(0), m_pDC_aux(0),
    win_(0),
    redraw_posted_(true),
    overlay_redraw_posted_(true),
    idle_request_posted_(false)
{
  if (vgui_mfc_use_bitmap)
    // kym - double buffering is not available with
    // acceleration (it crashes windows).
    set_double_buffering(false);

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

  // Delete the RC
  if ( m_hRC && (FALSE == ::wglDeleteContext( m_hRC )) )
  {
    ::AfxMessageBox("wglDeleteContext failed.");
  }

  // Release the device context:
  HDC m_hgldc = ::GetDC(m_hWnd);
  ::ReleaseDC(m_hWnd, m_hgldc);

  // Delete the DC
  if ( m_pDC )
    delete m_pDC;
  if ( m_pDC_aux )
    delete m_pDC_aux;
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
  if (!overlay_redraw_posted_)
  {
    CWnd* wnd;
    if (m_pCWnd != 0)
      wnd = m_pCWnd;
    else
      wnd = AfxGetApp()->GetMainWnd();
    if (wnd)
      wnd->Invalidate(FALSE);
  }
  overlay_redraw_posted_ = true;
}

//: MFC implementation of vgui_adaptor function - redraws rendering area.
void vgui_mfc_adaptor::post_redraw()
{
  if (!redraw_posted_)
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
  redraw_posted_ = true;
}

void vgui_mfc_adaptor::post_idle_request()
{
  idle_request_posted_ = true;
}


//: MFC implementation of vgui_adaptor function - make this the current GL rendering context.
void vgui_mfc_adaptor::make_current()
{
  ::wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC );
}

//: MFC implementation of vgui_adaptor function - swap buffers if using double buffering.
void vgui_mfc_adaptor::swap_buffers()
{
  if ( !vgui_mfc_use_bitmap )
    SwapBuffers(m_pDC->m_hDC);
}

//: Change the default popup menu to the given one (not yet implemented).
void vgui_mfc_adaptor::set_default_popup(vgui_menu)
{
  vcl_cerr << "vgui_mfc_adaptor::set_default_popup\n";
}

//: Return the default popup menu (not yet implemented).
vgui_menu vgui_mfc_adaptor::get_popup()
{
  vcl_cerr<< "vgui_mfc_adaptor::get_popup\n";
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

  if (vgui_mfc_use_bitmap) {
    HBITMAP obmp = 0;
    create_bitmap( 1, 1, m_pDC, obmp );
    DeleteObject( obmp );
    create_bitmap( 1, 1, m_pDC_aux, obmp );
    DeleteObject( obmp );
    set_double_buffering(false);
  } else {
    m_pDC = new CClientDC(this);
  }

  if ( NULL == m_pDC ) // failure to get DC
  {
    ::AfxMessageBox("Couldn't get a valid DC.");
    return FALSE;
  }

  if ( vgui_mfc_use_bitmap )
    m_hRC = setup_for_gl( m_pDC,
                          PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL );
  else
    m_hRC = setup_for_gl( m_pDC,
                          PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER );

  post_redraw();

  return 0;
}


HGLRC vgui_mfc_adaptor::setup_for_gl( CDC* pDC, DWORD dwFlags )
{
  PIXELFORMATDESCRIPTOR pfd;
  ZeroMemory( &pfd, sizeof( pfd ) );
  pfd.nSize = sizeof( pfd );
  pfd.nVersion = 1;
  pfd.dwFlags = dwFlags;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cDepthBits = 16;
  pfd.iLayerType = PFD_MAIN_PLANE;

  int pixelformat = ChoosePixelFormat( pDC->GetSafeHdc(), &pfd );

  if (0 == pixelformat) {
    ::AfxMessageBox("ChoosePixelFormat failed.");
    vcl_cerr<<"Error code:"<<GetLastError();
    return 0;
  }
  if ( FALSE == SetPixelFormat( pDC->GetSafeHdc(), pixelformat, &pfd ) )
  {
    AfxMessageBox("SetPixelFormat failed.");
    vcl_cerr<<"Error code:"<<GetLastError();
    return 0;
  }

  HGLRC glrc = wglCreateContext( pDC->GetSafeHdc() );
  if ( 0 == glrc )
  {
    AfxMessageBox("wglCreateContext failed.");
    return 0;
  }

  if ( FALSE == wglMakeCurrent( pDC->GetSafeHdc(), glrc ) )
  {
    AfxMessageBox("wglMakeCurrent failed.");
    return 0;
  }

  // specify black as clear color
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  // specify the back of the buffer as clear depth
  glClearDepth( 1.0f );
  // enable depth testing
  glEnable( GL_DEPTH_TEST );

  vgui_macro_report_errors;

  return glrc;
}


void vgui_mfc_adaptor::create_bitmap( int cx, int cy,
                                      CDC*& pDC,
                                      HBITMAP& out_old_hbmp )
{
  BITMAPINFOHEADER bih;
  ZeroMemory( &bih, sizeof(bih) );

  bih.biSize = sizeof(bih);
  bih.biWidth = cx;
  bih.biHeight = cy;
  bih.biPlanes = 1;
  bih.biBitCount = 24;
  bih.biCompression = BI_RGB;

  if ( !pDC ){
    pDC = new CDC();
    pDC->CreateCompatibleDC(NULL);
  }

  void *buffer;
  HBITMAP hbmp = CreateDIBSection( pDC->GetSafeHdc(),
                                   (BITMAPINFO *)&bih,
                                   DIB_RGB_COLORS,
                                   &buffer,
                                   NULL,
                                   0 );
  if ( !hbmp ) {
    AfxMessageBox( "Failed to create bitmap" );
    return;
  }
  pDC->SetStretchBltMode(COLORONCOLOR);
  out_old_hbmp = (HBITMAP)SelectObject( pDC->GetSafeHdc(), hbmp );
}


//: Called by MFC when the main window has been destroyed.
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
  if ( redraw_posted_ )
  {
    vgui_macro_report_errors;
    this->make_current();
    dispatch_to_tableau(vgui_event(vgui_DRAW));
    vgui_macro_report_errors;
    redraw_posted_ = false;
    aux_dc_valid_ = false;
  }

  if ( overlay_redraw_posted_ )
  {
    this->make_current();

    if ( vgui_mfc_use_bitmap )
    {
      if ( aux_dc_valid_ ) {
        // copy aux buffer to gl buffer
        m_pDC->BitBlt( 0, 0, m_width, m_height, m_pDC_aux, 0, 0, SRCCOPY );
      } else {
        // copy gl buffer to aux buffer
        m_pDC_aux->BitBlt( 0, 0, m_width, m_height, m_pDC, 0, 0, SRCCOPY );
        aux_dc_valid_ = true;
      }
    }
    else
    {
      // Determine we if just did a redraw (aux_dc_valid_==false iff
      // just did a redraw).
      //
      if ( aux_dc_valid_ ) {
        // Nope. Do a redraw to delete the previous overlay
        dispatch_to_tableau( vgui_event(vgui_DRAW) );
      } else {
        // Yep. But next time we won't have...
        aux_dc_valid_ = true;
      }
    }

    vgui_macro_report_errors;
    dispatch_to_tableau(vgui_event(vgui_DRAW_OVERLAY));
    vgui_macro_report_errors;

    overlay_redraw_posted_ = false;
  }

  if ( vgui_mfc_use_bitmap )
  {
    CWnd* wnd;
    if (m_pCWnd)
      wnd = m_pCWnd;
    else
      wnd = AfxGetApp()->GetMainWnd();
    CDC *win_dc = wnd->GetDC();
    RECT r;
    wnd->GetClientRect(&r);
    win_dc->BitBlt(0,0,r.right,r.bottom,m_pDC,0,0,SRCCOPY);
    wnd->ReleaseDC(win_dc);
  }

  swap_buffers();
}

//: Sets timer to dispatch WM_TIME event to a mainframe every time milliseconds
void vgui_mfc_adaptor::post_timer(float tm,int id)
{
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
  if (debug)
    vcl_cerr << "OnDraw\n";
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

bool vgui_mfc_adaptor::do_idle()
{
  if ( idle_request_posted_ )
    idle_request_posted_ =  dispatch_to_tableau( vgui_event( vgui_IDLE ) );
  return idle_request_posted_;
}


//: Called by MFC when the application requests part of the window is redrawn.
void vgui_mfc_adaptor::OnPaint()
{
  CView::OnPaint();
}

//: Called by MFC when the application is resized.
void vgui_mfc_adaptor::OnSize(UINT nType, int cx, int cy)
{
  CView::OnSize(nType, cx, cy);

  m_width = cx;
  m_height = cy;

  if ( cx != 0 && cy != 0 && vgui_mfc_use_bitmap )
  {
    // create a new GL bitmap and aux bitmap to match the new window
    // size.
    HBITMAP obmp = 0;
    create_bitmap( cx, cy, m_pDC, obmp );
    DeleteObject( obmp );
    m_hRC = setup_for_gl( m_pDC,
                          PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL );
    create_bitmap( cx, cy, m_pDC_aux, obmp );
    DeleteObject( obmp );
  }
  post_redraw();
}

//: Convert MFC key character into an int suitable for vgui.
void mfc_key(UINT nChar, UINT nFlags, int *the_key, int *the_ascii_char)
{
  if (nFlags & 256)
  {
    // Extended code
    switch (nChar)
    {
      case VK_NEXT:
        *the_key = vgui_PAGE_DOWN;
        *the_ascii_char = vgui_PAGE_DOWN;
        return;
      case VK_PRIOR:
        *the_key = vgui_PAGE_UP;
        *the_ascii_char = vgui_PAGE_UP;
        return;
      case VK_LEFT:
        *the_key = vgui_CURSOR_LEFT;
        *the_ascii_char = vgui_CURSOR_LEFT;
        return;
      case VK_UP:
        *the_key = vgui_CURSOR_UP;
        *the_ascii_char = vgui_CURSOR_UP;
        return;
      case VK_RIGHT:
        *the_key = vgui_CURSOR_RIGHT;
        *the_ascii_char = vgui_CURSOR_RIGHT;
        return;
      case VK_DOWN:
        *the_key = vgui_CURSOR_DOWN;
        *the_ascii_char = vgui_CURSOR_DOWN;
        return;
      default:
        *the_key = vgui_key(0);
        *the_ascii_char = vgui_key(0);
        return;
    };
  }
  else
  {
    unsigned short buf[1024];
    unsigned char lpKeyState[256];
    vcl_memset(lpKeyState, 0, 256);
    vcl_memset(buf, 0, 256);

    int is_ok = ToAscii(nChar, nFlags & 0xff, lpKeyState, buf, 0);
    if (is_ok == 1)
      *the_key = buf[0];
    else
      *the_key = nChar;

    // Add modifiers to character:
    lpKeyState[VK_SHIFT] = GetKeyState(VK_SHIFT);
    lpKeyState[VK_CONTROL] = GetKeyState(VK_CONTROL);

    is_ok = ToAscii(nChar, nFlags & 0xff, lpKeyState, buf, 0);
    if (is_ok == 1)
      *the_ascii_char = buf[0];
    else
      *the_ascii_char = nChar;
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
  if (GetKeyState(VK_MENU) & 0x8000)
    evt.modifier = vgui_ALT;
  // kym - VK_MENU (alt key) doesn't seem to reach here - it is used
  // by the menu, so, it seems, there will be no vgui_ALT events for MFC.

  int the_key, the_ascii_char;
  mfc_key(nChar, nFlags, &the_key, &the_ascii_char);
  evt.set_key( vgui_key(the_key) );
  evt.ascii_char = vgui_key(the_ascii_char);
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
//vcl_cerr <<"vgui_mfc_adaptor::domouse: wo = "<< point.x<<", "<< point.y<<'\n';
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
  if (e.modifier == mixin::popup_modifier  && e.button == mixin::popup_button)
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
  //vcl_cerr << "Mouse wheel events are not handled\n";
  return FALSE;
}

