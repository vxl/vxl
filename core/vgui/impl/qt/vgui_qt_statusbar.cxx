#include "vgui_qt_statusbar.h"
 
vgui_qt_statusbar::vgui_qt_statusbar()
:  QStatusBar(0, "boe"),
   statusbuf(new vgui_statusbuf(this)), 
   out(statusbuf)
{
}


vgui_qt_statusbar::~vgui_qt_statusbar()
{
}

//static int context_id = 1;

int vgui_qt_statusbar::write(const char* text, int n) 
{
   this->message(text, n);
   return n;  
}


int vgui_qt_statusbar::write(const char* text) 
{
   this->message(text);
   return 1;
}
