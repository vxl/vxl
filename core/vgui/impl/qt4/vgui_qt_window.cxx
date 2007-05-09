#include "vgui_qt_window.h"

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>

#include "vgui_qt_adaptor.h"
#include "vgui_qt_menu.h"
#include "vgui_qt_statusbar.h"

#include <qmenubar.h>
#include <q3vgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <q3frame.h>
//Added by qt3to4:
#include <Q3PopupMenu>
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>

//-----------------------------------------------------------------------------
void vgui_qt_window::setup_widget(int w, int h, const char* title)
{
   this->setCaption(title);
   this->resize(w,h);

   QWidget* mainwidget = new QWidget(this, "vgui_qt_gl_main_widget");

   //Create a frame to store the GL widget
   Q3Frame* frame = new Q3Frame (mainwidget, "vgui_qt_gl_frame");
   frame->setFrameStyle (Q3Frame::Sunken | Q3Frame::StyledPanel);
   frame->setLineWidth (2);

   //Create the GL widget and put it in the frame
   adaptor = new vgui_qt_adaptor(frame);
   Q3HBoxLayout* hlayout = new Q3HBoxLayout (frame, 2, 2, "vgui_qt_gl_hlayout");
   hlayout->addWidget (adaptor, 1);

   //Top level layout
   Q3VBoxLayout* vlayout = new Q3VBoxLayout (mainwidget, 15, 5, "vgui_qt_gl_vlayout");
   vlayout->addWidget(frame, 1);

   setCentralWidget(mainwidget);

#ifndef __SGI_CC // SGI's iostream does not allow re-initialising
   vgui::out.rdbuf(statusbar.statusbuf);
#endif
}

//-----------------------------------------------------------------------------
vgui_qt_window::vgui_qt_window(int w, int h, const char* title)
:  Q3MainWindow(0, "vgui_qt_mainwindow_without_menu"),
   statusbar (this),
   use_menubar(false),
   use_statusbar(true)
{
   setup_widget(w, h, title);
}


//--------------------------------------------------------------------------------
vgui_qt_window::vgui_qt_window(int w, int h, const vgui_menu& menu, const char* title)
:  Q3MainWindow(0, "vgui_qt_mainwindow_with_menu"),
   statusbar (this),
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
   for (unsigned int i=0; i < menu.size(); ++i)
   {
      qm = new vgui_qt_menu(*(menu[i].menu));
      menuBar()->insertItem(menu[i].name.c_str(), (Q3PopupMenu*) qm);
   }
}
