// This is core/vgui/impl/mfc/vgui_mfc_app.cxx
#include "StdAfx.h"
#include "vgui_mfc_app.h"
//:
// \file
// \brief   See vgui_mfc_app.h for a description of this file.
// \author  Marko Bacic, Oxford RRG
// \date    27 July 2000
//
// \verbatim
//  Modifications
//   K.Y.McGaul  18-SEP-2001   Changed indentation from 8 spaces to 2 spaces.
// \endverbatim
//-----------------------------------------------------------------------------

#include "vgui_mfc_view.h"
#include "vgui_mfc_doc.h"
#include "vgui_mfc_mainfrm.h"
#include "resource.h"
#if 0
  #ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
  #endif
#endif

/////////////////////////////////////////////////////////////////////////////
//: vgui_mfc_app construction
vgui_mfc_app::vgui_mfc_app()
{
  // TODO: add construction code here,
  // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
//: The one and only vgui_mfc_app object
class vgui_mfc_app_command_line_info : public CCommandLineInfo
{
 public:
  void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast )
  {
    CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
  }
};

static void f();

/////////////////////////////////////////////////////////////////////////////
//: vgui_mfc_app initialization
BOOL vgui_mfc_app::InitInstance()
{
  AfxEnableControlContainer();

  // Standard initialization
  // If you are not using these features and wish to reduce the size
  //  of your final executable, you should remove from the following
  //  the specific initialization routines you do not need.
#if 0
  #ifdef _DEBUG
    Enable3dControls();       // Call this when using MFC in a shared DLL
  #else
    Enable3dControlsStatic(); // Call this when linking to MFC statically
  #endif
#endif
  // Change the registry key under which our settings are stored.
  // TODO: You should modify this string to be something appropriate
  // such as the name of your company or organization.
  SetRegistryKey(_T("Local AppWizard-Generated Applications"));

  LoadStdProfileSettings();  // Load standard INI file options (including MRU)

  // Register the application's document templates.  Document templates
  //  serve as the connection between documents, frame windows and views.
  CSingleDocTemplate *pDocTemplate;
  pDocTemplate = new CSingleDocTemplate(
    IDR_MAINFRAME,
    RUNTIME_CLASS(vgui_mfc_doc),
    RUNTIME_CLASS(vgui_mfc_mainfrm),// main SDI frame window
    RUNTIME_CLASS(vgui_mfc_view));
  AddDocTemplate(pDocTemplate);

  // Parse command line for standard shell commands, DDE, file open
  vgui_mfc_app_command_line_info cmdInfo;
#if 0
  ParseCommandLine(cmdInfo);
#endif

  // *** IMPORTANT STUFF (marko) ***
  CDocument *pDocument = pDocTemplate->CreateNewDocument();
  // pDocument->m_bAutoDelete = FALSE; //awf added to stop segv on exit.
  //  probly wrong.  Then removed as it did nowt.

  CFrameWnd* pFrame = (CFrameWnd *)(RUNTIME_CLASS(vgui_mfc_mainfrm)->CreateObject());
  CCreateContext context;
  context.m_pCurrentFrame = pFrame;
  context.m_pCurrentDoc = pDocument;
  context.m_pNewViewClass = RUNTIME_CLASS(vgui_mfc_view);
  context.m_pNewDocTemplate = pDocTemplate;
  pFrame->Create(NULL, _T("VGUI"), WS_OVERLAPPEDWINDOW /*|WS_HSCROLL|WS_VSCROLL*/,
                 pFrame->rectDefault,NULL,NULL,0,&context);
  m_pMainWnd = pFrame;
  pDocTemplate->InitialUpdateFrame(pFrame,pDocument);
  // The one and only window has been initialized, so show and update it.
#if 0
  m_pMainWnd->SetScrollRange(SB_HORZ,-1024,1024);
  m_pMainWnd->SetScrollRange(SB_VERT,-1024,1024);
  m_pMainWnd->SetScrollPos(SB_HORZ,0);
  m_pMainWnd->SetScrollPos(SB_VERT,0);
#endif
  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();

  return TRUE;
}

//: CWinApp::Run() calls CWinThread::Run().
BOOL vgui_mfc_app::Run()
{
  // for tracking the idle time state
  BOOL bIdle = TRUE;
  LONG lIdleCount = 0;
  MSG msgCur;

  // acquire and dispatch messages until a WM_QUIT message is received.
  for (;;)
  {
    // phase1: check to see if we can do idle work
    while (bIdle && !::PeekMessage(&msgCur, NULL, NULL, NULL, PM_NOREMOVE))
    {
      // call OnIdle while in bIdle state
      if (!OnIdle(lIdleCount++))
        bIdle = FALSE; // assume "no idle" state
    }

    // phase2: pump messages while available
    do
    {
      MSG tmp_msg;
      if (::PeekMessage(&tmp_msg,NULL,NULL,NULL,PM_NOREMOVE))
      {
        if (true)
        {
          // Collapse multiple move events...
          while (tmp_msg.message == WM_MOVE)
          {
            // Remove the message from the queue
            ::PeekMessage(&tmp_msg,NULL,NULL,NULL,PM_REMOVE);
            // Get the next message
            BOOL status = ::PeekMessage(&tmp_msg,NULL,NULL,NULL,PM_NOREMOVE);
            // If there were no more messages in the queue or
            // the next message retrieved is not a move
            // process the message
            if (!status || !(tmp_msg.message == WM_MOVE))
            {
              ::TranslateMessage(&tmp_msg);
              ::DispatchMessage(&tmp_msg);
            }
          }
        }
      }

      // Set up a breakpoint place for interesting msgs
      {
        switch (tmp_msg.message)
        {
         case 0x036a: // kickidle
         case 0x0362: // setmessagestring
         case WM_PAINT:
         case WM_KEYUP:
         default: f(); break;
        }
      }

      // pump message, but quit on WM_QUIT
      if (!PumpMessage())
        return ExitInstance();

      // reset "no idle" state after pumping "normal" message
      if (IsIdleMessage(&msgCur))
      {
        bIdle = TRUE;
        lIdleCount = 0;
      }
    } while (::PeekMessage(&msgCur, NULL, NULL, NULL, PM_NOREMOVE));
  }
  ASSERT(FALSE);  // not reachable
}

/////////////////////////////////////////////////////////////////////////////
//: vgui_mfc_app message handlers
void f() {}

BOOL vgui_mfc_app::OnIdle( LONG lCount )
{
  // counts 0 and 1 are used by the MFC framework to update menus and
  // such. Process those first.
  //
  if (CWinApp::OnIdle(lCount))
    return TRUE;

  // Send an idle event to each adaptor.
  POSITION tmpl_pos = this->GetFirstDocTemplatePosition();
  while ( tmpl_pos )
  {
    CDocTemplate *tmpl = this->GetNextDocTemplate(tmpl_pos);
    POSITION doc_pos = tmpl->GetFirstDocPosition();
    while ( doc_pos )
    {
      CDocument *pdoc = tmpl->GetNextDoc(doc_pos);
      POSITION view_pos = pdoc->GetFirstViewPosition();
      while ( view_pos )
      {
        vgui_mfc_adaptor *adaptor = (vgui_mfc_adaptor *)pdoc->GetNextView(view_pos);
        if ( adaptor->do_idle() ) {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}
