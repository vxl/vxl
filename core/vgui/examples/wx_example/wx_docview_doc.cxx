// This is core/vgui/examples/wx_example/wx_docview_doc.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets document class.
//
// See wx_docview_doc.h for details.
//=========================================================================

#include "wx_docview_doc.h"

#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wx_docview_doc, wxDocument)

wx_docview_doc::wx_docview_doc(void)
{
  // Put the image.tableau into an easy2D tableau
  vgui_easy2D_tableau_new easy2D;

  // Add a point, line, and infinite line
  easy2D->set_foreground(0,1,0);
  easy2D->set_point_radius(5);
  easy2D->add_point(10, 20);

  easy2D->set_foreground(0,0,1);
  easy2D->set_line_width(2);
  easy2D->add_line(100,100,200,400);

  easy2D->set_foreground(0,1,0);
  easy2D->set_line_width(2);
  easy2D->add_infinite_line(1,1,-100);

  vgui_viewer2D_tableau_new viewer(easy2D);
  vgui_shell_tableau_new shell(viewer);
  shell->set_enable_key_bindings(true);
  tableau_ = shell;
}

bool wx_docview_doc::OnCreate(const wxString& path, long flags)
{
  return wxDocument::OnCreate(path, flags);
}
