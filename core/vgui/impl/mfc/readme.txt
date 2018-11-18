
A Frame contains
 A View of
   A Document.

In this implementation, the main VGUI tableau is the document,
and the vgui_adaptor is the MFC "View"


A program (e.g. xcv) does the following:

1. Create a vgui_mfc_app object using vgui_mfc_app_init.
   This is done automatically when you #include <vgui/vgui_linker_hack.h>
   The vgui_mfc_app_init call makes a new vgui_mfc_app, which
   contains the whole application's gui

    vgui_mfc::init()
    {
      AfxWinInit -- initialize mfc
      AfxGetApp.InitInstance
    }

     vgui_mfc_app_init contains
       p: vgui_mfc_app contains
            base: CWinApp
            method: InitInstance
            {
              make document from vgui_mfc_doc
              make frame vgui_mfc_mainfrm
              show window
            }
            method: Run
            {
              while (true)
              {
                get event
                dispatch event to (ultimately) methods on vgui_mfc_view.
              }
            }

     vgui_mfc_doc contains
       base: CDocument
       tableau: vgui_tableau.

     vgui_mfc_mainfrm contains
       base: CFrameWnd
       method: vgui_mfc_mainfrm
       method: ~vgui_mfc_mainfrm
       method: PreCreateWindow
       method: process_menus
       method: OnCreate/*window*/
       method: OnTimer

     vgui_mfc_view contains
       base: vgui_mfc_adaptor
         base: CView, vgui_adaptor, vgui_adaptor_mixin
         pDC: CDC
         method: On/*Events*/


-----------------------------------------------------------------------------
vgui::init
  vgui_mfc::init
    vgui_mfc_app::InitInstance
      CDocTemplate::CreateNewDocument
        new vgui_mfc_doc
      new vgui_mfc_mainfrm
      pFrame->Create
          ::CreateWindowEx
            ... vgui_mfc_mainfrm::OnCreate
                  CFrameWnd::CreateView
                    new vgui_mfc_view
