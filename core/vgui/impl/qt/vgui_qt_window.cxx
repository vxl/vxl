#include "vgui_qt_window.h"

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>

#include "vgui_qt_adaptor.h"
#include "vgui_qt_menu.h"
#include "vgui_qt_statusbar.h"

#include <qmenubar.h>
#include <qlayout.h>
#include <qframe.h>

//-----------------------------------------------------------------------------
void vgui_qt_window::setup_widget(int w, int h, const char* title)
{
   this->setCaption(title);
   this->resize(w,h);
  
   QWidget* dummy = new QWidget(this);
   QFrame* frame = new QFrame (dummy, "frame1");
   frame->setFrameStyle (QFrame::Sunken | QFrame::Panel);
   frame->setLineWidth (2);

   adaptor = new vgui_qt_adaptor(frame);
   
   QHBoxLayout* flayout = new QHBoxLayout(frame, 2, 2, "flayout");
   flayout->addWidget (adaptor , 1);

   QBoxLayout* hlayout = new QHBoxLayout (dummy, 20, 10, "halyout");
   hlayout->addWidget (frame, 1);

   setCentralWidget(dummy);   

// No fancy frame
//   adaptor = new vgui_qt_adaptor(this);
//   setCentralWidget(adaptor);   
}

//-----------------------------------------------------------------------------
vgui_qt_window::vgui_qt_window(int w, int h, const char* title)
:  QMainWindow(),
   use_menubar(false),
   use_statusbar(true)
{
   setup_widget(w, h, title);
}


//--------------------------------------------------------------------------------
vgui_qt_window::vgui_qt_window(int w, int h, const vgui_menu& menu, const char* title) 
:  QMainWindow(),
   use_menubar(true),
   use_statusbar(true)
{
   setup_widget(w, h, title);
   set_menubar(menu);
}


//--------------------------------------------------------------------------------
void vgui_qt_window::set_menubar(const vgui_menu &menu) 
{
   use_menubar = true;

   vgui_qt_menu* qm;
   for (unsigned i=0; i < menu.size();i++) 
   {
      qm = new vgui_qt_menu(*(menu[i].menu));
      menuBar()->insertItem(menu[i].name.c_str(), (QPopupMenu*) qm);
   }
}


//--------------------------------------------------------------------------------
void vgui_qt_window::set_adaptor(vgui_adaptor* a) 
{
   vcl_cerr << "Ola dit moet nog gebeuren\n";
}
