// example_mfcView.cpp : implementation of the example_CExample_mfcView class

#include "StdAfx.h"
#include "example_mfc.h"

#include "example_mfcDoc.h"
#include "example_mfcView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// example_CExample_mfcView

//awf: It's crucial to change the baseclass from CView to vgui_mfc_adaptor,
// otherwise none of the adaptor's message handlers will be called.

IMPLEMENT_DYNCREATE(example_CExample_mfcView, vgui_mfc_adaptor)

BEGIN_MESSAGE_MAP(example_CExample_mfcView, vgui_mfc_adaptor)
        //{{AFX_MSG_MAP(example_CExample_mfcView)
        ON_WM_CREATE()
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// example_CExample_mfcView construction/destruction

example_CExample_mfcView::example_CExample_mfcView()
{
        // TODO: add construction code here
}

example_CExample_mfcView::~example_CExample_mfcView()
{
}

BOOL example_CExample_mfcView::PreCreateWindow(CREATESTRUCT& cs)
{
   // awf: Call adaptor's PreCreateWindow - add anything else you like here.
   // If you want to be tidy, and nothing goes here, delete the method from
   // within devstudio.
        return vgui_mfc_adaptor::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// example_CExample_mfcView drawing

void example_CExample_mfcView::OnDraw(CDC* pDC)
{
        example_CExample_mfcDoc* pDoc = GetDocument();
        ASSERT_VALID(pDoc);
        // awf:  Call adaptor's draw.  One can do OpenGL jiggerypokery here if
        // one wishes.
        vgui_mfc_adaptor::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// example_CExample_mfcView diagnostics

#ifdef _DEBUG
void example_CExample_mfcView::AssertValid() const
{
        CView::AssertValid();
}

void example_CExample_mfcView::Dump(CDumpContext& dc) const
{
        CView::Dump(dc);
}

example_CExample_mfcDoc* example_CExample_mfcView::GetDocument() // non-debug version is inline
{
        ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(example_CExample_mfcDoc)));
        return (example_CExample_mfcDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// example_CExample_mfcView message handlers

int example_CExample_mfcView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
        if (vgui_mfc_adaptor::OnCreate(lpCreateStruct) == -1)
                return -1;

        // awf: when this view is created, grab the tableau
        // from the document, and tell the vgui adaptor about it
        set_tableau(GetDocument()->get_tableau());
        return 0;
}
