// This is core/vgui/impl/mfc/vgui_mfc_mainfrm.cxx
#include "StdAfx.h"
#include "vgui_mfc_mainfrm.h"
//:
//  \file
//
// See vgui_mfc_mainfrm.h for a description of this file.
//

#include "vgui_mfc.h"
#include "vgui_mfc_utils.h"
#include "vgui_mfc_adaptor.h"
#include "vgui_mfc_statusbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_mainfrm

IMPLEMENT_DYNCREATE(vgui_mfc_mainfrm, CFrameWnd)

BEGIN_MESSAGE_MAP(vgui_mfc_mainfrm, CFrameWnd)
  ON_WM_CREATE()
  ON_WM_HSCROLL() // - We are processing both horizontal scroll bar
  ON_WM_VSCROLL() // and vertical scroll bar events
  ON_WM_TIMER()   //
  ON_WM_CLOSE()   // added manually by awf
  ON_COMMAND_RANGE(ID_MENU_ITEMS,ID_MENU_ITEMS+MAX_ITEM_COUNT,process_menus)
  ON_UPDATE_COMMAND_UI(ID_SEPARATOR,OnUpdateStatusBar)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_mainfrm construction/destruction

vgui_mfc_mainfrm::vgui_mfc_mainfrm()
{
        // TODO: add member initialization code here
        // Set the scroll range
}

vgui_mfc_mainfrm::~vgui_mfc_mainfrm()
{
}

//: Called by MFC before window creation.
//  Store any desired info in cs.
//  Returns non-zero value to indicate creation should continue.
BOOL vgui_mfc_mainfrm::PreCreateWindow(CREATESTRUCT& cs)
{
        if ( !CFrameWnd::PreCreateWindow(cs) )
                return FALSE;
        // TODO: Modify the Window class or styles here by modifying
        //  the CREATESTRUCT cs

        return TRUE;
}

//: Called by MFC on window creation
int vgui_mfc_mainfrm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;
  return 0;
}

void vgui_mfc_mainfrm::OnClose()
{
  // hmmm.  don't know if it's bad not to do this,
  // but I don't like the segv. -awf 0201
  //CFrameWnd::OnClose();

  DestroyWindow();
  DestroyAcceleratorTable(vgui_mfc_utils::instance()->AccelTable);
  PostQuitMessage(0);
}

//: Called by MFC before a message is sent to the menus.
//  We are overriding this function so we can use our own menu
//  accelerators defined AccelTable.
//  FIXME - kym, this doesn't seem to work!!!
BOOL vgui_mfc_mainfrm::PreTranslateMessage(MSG* pmsg)
{
  if (pmsg->message >= WM_KEYFIRST && pmsg->message <= WM_KEYLAST)
  {
    // m_hWnd is the handle of this Windows window attached to this CWnd:
    TranslateAccelerator(m_hWnd, vgui_mfc_utils::instance()->AccelTable, (LPMSG)pmsg);
  }
  return CFrameWnd::PreTranslateMessage(pmsg);
}

//: Called whenever a menu item has been selected
void vgui_mfc_mainfrm::process_menus(UINT nID)
{
  vgui_mfc_utils::instance()->menu_dispatcher(nID);
}

//: Called when ON_WM_HSCROLL message is received
void vgui_mfc_mainfrm::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
  // --- For the time being this routine only deals with this windows' scroll
  // bar and not to any controls within the window
  CWinApp *app = AfxGetApp();
  POSITION pos = app->GetFirstDocTemplatePosition();
  CDocTemplate *tmpl = app->GetNextDocTemplate(pos);
  pos = tmpl->GetFirstDocPosition();
  CDocument *pdoc = tmpl->GetNextDoc(pos);
  pos = pdoc->GetFirstViewPosition();
  vgui_mfc_adaptor *adaptor = (vgui_mfc_adaptor *)pdoc->GetNextView(pos);
  ASSERT(pScrollBar == NULL);
  vgui_event e(vgui_HSCROLL);

  switch (nSBCode)
  {
   case SB_THUMBTRACK:
   case SB_THUMBPOSITION:
    SetScrollPos(SB_HORZ,nPos);
    e.data = &nPos;
    adaptor->dispatch_to_tableau(e);
    adaptor->service_redraws();
    break;
   case SB_LINELEFT:
    SetScrollPos(SB_HORZ,GetScrollPos(SB_HORZ)-1);
    nPos = GetScrollPos(SB_HORZ);
    e.data = &nPos;
    adaptor->dispatch_to_tableau(e);
    adaptor->service_redraws();
    break;
   case SB_LINERIGHT:
    SetScrollPos(SB_HORZ,GetScrollPos(SB_HORZ)+1);
    nPos = GetScrollPos(SB_HORZ);
    e.data = &nPos;
    adaptor->dispatch_to_tableau(e);
    adaptor->service_redraws();
    break;
  }
}

//: Called when ON_WM_VSCROLL message is received
void vgui_mfc_mainfrm::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
  // For the time being this routine only deals with this windows' scroll
  // bar and not to any controls within the window
  CWinApp *app = AfxGetApp();
  POSITION pos = app->GetFirstDocTemplatePosition();
  CDocTemplate *tmpl = app->GetNextDocTemplate(pos);
  pos = tmpl->GetFirstDocPosition();
  CDocument *pdoc = tmpl->GetNextDoc(pos);
  pos = pdoc->GetFirstViewPosition();
  vgui_mfc_adaptor *adaptor = (vgui_mfc_adaptor *)pdoc->GetNextView(pos);

  ASSERT(pScrollBar == NULL);
  vgui_event e(vgui_VSCROLL);
  switch (nSBCode)
  {
   case SB_THUMBTRACK:
   case SB_THUMBPOSITION:
    SetScrollPos(SB_VERT,nPos);
    e.data = &nPos;
    adaptor->dispatch_to_tableau(e);
    adaptor->service_redraws();
    break;
   case SB_LINEUP:
    SetScrollPos(SB_VERT,GetScrollPos(SB_VERT)-1);
    nPos = GetScrollPos(SB_VERT);
    e.data = &nPos;
    adaptor->dispatch_to_tableau(e);
    adaptor->service_redraws();
    break;
   case SB_LINEDOWN:
    SetScrollPos(SB_VERT,GetScrollPos(SB_VERT)+1);
    nPos = GetScrollPos(SB_VERT);
    e.data = &nPos;
    adaptor->dispatch_to_tableau(e);
    adaptor->service_redraws();
    break;
  }
}

void vgui_mfc_mainfrm::OnUpdateStatusBar(CCmdUI *sbar)
{
  sbar->Enable();
  sbar->SetText(statusbar->linebuffer.c_str());
}

void vgui_mfc_mainfrm::OnTimer(UINT id)
{
  // This thing should go only once. Kill the timer
  // before dispatching the event because the event
  // might take a long time to handle and we don't
  // want the timers building up.
  KillTimer(id);
  vgui_adaptor::current->dispatch_to_tableau(vgui_TIMER);
}

#ifdef _DEBUG
void vgui_mfc_mainfrm::AssertValid() const
{
        CFrameWnd::AssertValid();
}

void vgui_mfc_mainfrm::Dump(CDumpContext& dc) const
{
        CFrameWnd::Dump(dc);
}

#endif //_DEBUG
