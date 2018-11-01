// This is core/vgui/impl/gtk2/vgui_gtk2_statusbar.cxx
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   21 Nov 99
// \brief  See vgui_gtk2_statusbar.h for a description of this file.
//-----------------------------------------------------------------------------

#include "vgui_gtk2_statusbar.h"

vgui_gtk2_statusbar::vgui_gtk2_statusbar()
  : statusbuf(new vgui_statusbuf(this))
  , out(statusbuf)
{
}


vgui_gtk2_statusbar::~vgui_gtk2_statusbar()
{
//  if (widget)
//    gtk_widget_destroy(widget);
  delete statusbuf;
}

static int context_id = 1;

int vgui_gtk2_statusbar::write(const char* text, int n) {
  if (n == 1) {
    if (text[0] == '\n') {
      gtk_statusbar_pop(GTK_STATUSBAR(widget), context_id);
      gtk_statusbar_push(GTK_STATUSBAR(widget), context_id, linebuffer.c_str() );
      linebuffer = "";
    }
    else
      linebuffer += text[0];
  }
  else {
    linebuffer.append(text, n);
    if (linebuffer.find('\n')) {
      gtk_statusbar_pop(GTK_STATUSBAR(widget), context_id);
      gtk_statusbar_push(GTK_STATUSBAR(widget), context_id, linebuffer.c_str() );
      linebuffer = "";
    }
  }
  return n;
}


int vgui_gtk2_statusbar::write(const char* text) {

  gtk_statusbar_pop(GTK_STATUSBAR(widget), context_id);
  gtk_statusbar_push(GTK_STATUSBAR(widget), context_id, text );
  return 1;
}
