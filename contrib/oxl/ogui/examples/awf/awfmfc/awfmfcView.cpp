// awfmfcView.cpp : implementation of the example_CAwfmfcView class
//

#include <vgui/impl/mfc/StdAfx.h>
#include "awfmfc.h"

#include "awfmfcDoc.h"
#include "awfmfcView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcView

IMPLEMENT_DYNCREATE(example_CAwfmfcView, vgui_mfc_adaptor)

BEGIN_MESSAGE_MAP(example_CAwfmfcView, vgui_mfc_adaptor)
        //{{AFX_MSG_MAP(example_CAwfmfcView)
        ON_WM_CREATE()
        //}}AFX_MSG_MAP
        // Standard printing commands
        ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
        ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
        ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcView construction/destruction

example_CAwfmfcView::example_CAwfmfcView()
{
        // TODO: add construction code here

}

example_CAwfmfcView::~example_CAwfmfcView()
{
}

BOOL example_CAwfmfcView::PreCreateWindow(CREATESTRUCT& cs)
{
        // TODO: Modify the Window class or styles here by modifying
        //  the CREATESTRUCT cs


        return vgui_mfc_adaptor::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcView drawing

void example_CAwfmfcView::OnDraw(CDC* pDC)
{
        example_CAwfmfcDoc* pDoc = GetDocument();
        ASSERT_VALID(pDoc);
        // TODO: add draw code for native data here
        vgui_mfc_adaptor::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcView printing

BOOL example_CAwfmfcView::OnPreparePrinting(CPrintInfo* pInfo)
{
        // default preparation
        return DoPreparePrinting(pInfo);
}

void example_CAwfmfcView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add extra initialization before printing
}

void example_CAwfmfcView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
        // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcView diagnostics

#ifdef _DEBUG
void example_CAwfmfcView::AssertValid() const
{
        CView::AssertValid();
}

void example_CAwfmfcView::Dump(CDumpContext& dc) const
{
        CView::Dump(dc);
}

example_CAwfmfcDoc* example_CAwfmfcView::GetDocument() // non-debug version is inline
{
        ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(example_CAwfmfcDoc)));
        return (example_CAwfmfcDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcView message handlers

int example_CAwfmfcView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
        if (vgui_mfc_adaptor::OnCreate(lpCreateStruct) == -1)
                return -1;

        // awf: when this view is created, grab the tableau
        // from the document, and tell the vgui adaptor about it
        set_tableau(GetDocument()->get_tableau());


        return 0;
}
