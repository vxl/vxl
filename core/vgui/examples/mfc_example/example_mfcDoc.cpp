// example_mfcDoc.cpp : implementation of the example_CExample_mfcDoc class
//

#include "StdAfx.h"
#include "example_mfc.h"

#include "example_mfcDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// example_CExample_mfcDoc

IMPLEMENT_DYNCREATE(example_CExample_mfcDoc, CDocument)

BEGIN_MESSAGE_MAP(example_CExample_mfcDoc, CDocument)
        //{{AFX_MSG_MAP(example_CExample_mfcDoc)
                // NOTE - the ClassWizard will add and remove mapping macros here.
                //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// example_CExample_mfcDoc construction/destruction

//awf: make an easy2d

#undef new

#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>


example_CExample_mfcDoc::example_CExample_mfcDoc()
{
  // Use an easy2D tableau which allows us to draw 2D objects:
  vgui_easy2D_tableau_new easy2D("unnamed");

  // Add a line:
  easy2D->set_foreground(0,0,1);
  easy2D->set_line_width(2);
  easy2D->add_line(100,100,200,400);
  easy2D->set_foreground(1,1,0);
  easy2D->add_line(200,400, 600,300);
  easy2D->set_foreground(1,0,0);
  easy2D->add_line(600, 300, 100,100);

  // Put into a viewer2D tableau to get zooming, etc:
  vgui_viewer2D_tableau_new viewer(easy2D);

  this->tableau = vgui_shell_tableau_new(viewer);
}

example_CExample_mfcDoc::~example_CExample_mfcDoc()
{
}

BOOL example_CExample_mfcDoc::OnNewDocument()
{
        if (!CDocument::OnNewDocument())
                return FALSE;

        // TODO: add reinitialization code here
        // (SDI documents will reuse this document)

        return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// example_CExample_mfcDoc serialization

void example_CExample_mfcDoc::Serialize(CArchive& ar)
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
// example_CExample_mfcDoc diagnostics

#ifdef _DEBUG
void example_CExample_mfcDoc::AssertValid() const
{
        CDocument::AssertValid();
}

void example_CExample_mfcDoc::Dump(CDumpContext& dc) const
{
        CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// example_CExample_mfcDoc commands

vgui_tableau_sptr example_CExample_mfcDoc::get_tableau()
{
  return tableau;
}
