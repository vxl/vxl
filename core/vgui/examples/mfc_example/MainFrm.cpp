// MainFrm.cpp : implementation of the example_CMainFrame class
//

#include "StdAfx.h"
#include "example_mfc.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// example_CMainFrame

IMPLEMENT_DYNCREATE(example_CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(example_CMainFrame, CFrameWnd)
        //{{AFX_MSG_MAP(example_CMainFrame)
                // NOTE - the ClassWizard will add and remove mapping macros here.
                //    DO NOT EDIT what you see in these blocks of generated code !
        ON_WM_CREATE()
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
        ID_SEPARATOR,           // status line indicator
        ID_INDICATOR_CAPS,
        ID_INDICATOR_NUM,
        ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// example_CMainFrame construction/destruction

example_CMainFrame::example_CMainFrame()
{
        // TODO: add member initialization code here
}

example_CMainFrame::~example_CMainFrame()
{
}

int example_CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
        if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
                return -1;

        if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP |
            CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
            !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
        {
                TRACE0("Failed to create toolbar\n");
                return -1;      // fail to create
        }

        if (!m_wndStatusBar.Create(this) ||
            !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
        {
                TRACE0("Failed to create status bar\n");
                return -1;      // fail to create
        }

        // TODO: Delete these three lines if you don't want the toolbar to
        //  be dockable
        m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
        EnableDocking(CBRS_ALIGN_ANY);
        DockControlBar(&m_wndToolBar);

        return 0;
}

BOOL example_CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
        if ( !CFrameWnd::PreCreateWindow(cs) )
                return FALSE;
        // TODO: Modify the Window class or styles here by modifying
        //  the CREATESTRUCT cs

        return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// example_CMainFrame diagnostics

#ifdef _DEBUG
void example_CMainFrame::AssertValid() const
{
        CFrameWnd::AssertValid();
}

void example_CMainFrame::Dump(CDumpContext& dc) const
{
        CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// example_CMainFrame message handlers
