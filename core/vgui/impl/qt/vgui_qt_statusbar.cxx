#include "vgui_qt_statusbar.h"

vgui_qt_statusbar::vgui_qt_statusbar(QMainWindow *parent)
:  statusbuf(new vgui_statusbuf(this)),
   out(statusbuf),
   parent_(parent)
{
}


vgui_qt_statusbar::~vgui_qt_statusbar()
{
  delete statusbuf;
}

//static int context_id = 1;

int vgui_qt_statusbar::write(const char* text, int n)
{
  static bool start_new = false;
  if (n == 1) {
    if (text[0] == '\n') {
      // We are at the end of the message. Set a bool so we know next time
      // to clear the buffer before we start a new message.
      start_new = true;
    }
    else if (start_new == true){
      parent_->statusBar()->message(linebuffer.c_str());
      linebuffer = "";
      linebuffer += text[0];
      start_new = false;
    }
    else
      linebuffer += text[0];
  }
  else {
    linebuffer.append(text, n);
    if (linebuffer.find('\n'))
    {
      parent_->statusBar()->message(linebuffer.c_str());
      linebuffer = "";
    }
  }
  return n;
}


int vgui_qt_statusbar::write(const char* text)
{
  linebuffer = text;
  parent_->statusBar()->message(linebuffer.c_str());
  return 1;
}
