// This is core/vgui/impl/mfc/vgui_mfc_doc.cxx
#include "StdAfx.h"
#include "vgui_mfc_doc.h"
//:
// \file
//
// See vgui_mfc_doc.h for a description of this file.
//
// \author Marko Bacic, Oxford RRG
// \date   27 July 2000
//

#include "vgui_mfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_doc

IMPLEMENT_DYNCREATE(vgui_mfc_doc, CDocument)


vgui_mfc_doc::vgui_mfc_doc()
{
}

vgui_mfc_doc::~vgui_mfc_doc()
{
}


/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_doc serialization

void vgui_mfc_doc::Serialize(CArchive& ar)
{
        if (ar.IsStoring())
        {
                // TODO: add storing code here
        }
        else
        {
                // TODO: add loading code here
        }
}

/////////////////////////////////////////////////////////////////////////////
// vgui_mfc_doc diagnostics

#ifdef _DEBUG
void vgui_mfc_doc::AssertValid() const
{
        CDocument::AssertValid();
}

void vgui_mfc_doc::Dump(CDumpContext& dc) const
{
        CDocument::Dump(dc);
}
#endif //_DEBUG
