// This is core/vgui/impl/mfc/vgui_mfc_view.cxx
#include "StdAfx.h"
#include "vgui_mfc_view.h"
//:
//  \file

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_view

IMPLEMENT_DYNCREATE(vgui_mfc_view, vgui_mfc_adaptor)

BEGIN_MESSAGE_MAP(vgui_mfc_view, vgui_mfc_adaptor)
  //{{AFX_MSG_MAP(vgui_mfc_view)
  ON_WM_CREATE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_view construction/destruction
vgui_mfc_view::vgui_mfc_view()
{
  // TODO: add construction code here
}

vgui_mfc_view::~vgui_mfc_view()
{
}

BOOL vgui_mfc_view::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  return vgui_mfc_adaptor::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_view drawing

void vgui_mfc_view::OnDraw(CDC* pDC)
{
  CDocument* pDoc = GetDocument();
  ASSERT_VALID(pDoc);
  // TODO: add draw code for native data here
  vgui_mfc_adaptor::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_view printing

BOOL vgui_mfc_view::OnPreparePrinting(CPrintInfo* pInfo)
{
  // default preparation
  return DoPreparePrinting(pInfo);
}

void vgui_mfc_view::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add extra initialization before printing
}

void vgui_mfc_view::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_view diagnostics

#ifdef _DEBUG
void vgui_mfc_view::AssertValid() const
{
  CView::AssertValid();
}

void vgui_mfc_view::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

CDocument* vgui_mfc_view::GetDocument() // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocument)));
  return (CDocument*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
//: vgui_mfc_view message handlers
int vgui_mfc_view::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (vgui_mfc_adaptor::OnCreate(lpCreateStruct) == -1)
    return -1;

  // awf: when this view is created, grab the tableau
  // from the document, and tell the vgui adaptor about it
//  set_tableau(GetDocument()->get_tableau());


  return 0;
}
