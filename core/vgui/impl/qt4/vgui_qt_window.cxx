#include "vgui_qt_window.h"

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>

#include "vgui_qt_adaptor.h"
#include "vgui_qt_menu.h"
#include "vgui_qt_statusbar.h"

#include <QMenuBar>
#include <QLayout>
#include <QFrame>

//-----------------------------------------------------------------------------
void vgui_qt_window::setup_widget(int w, int h, const char* title)
{
   this->setWindowTitle(title);
   this->resize(w,h);

   //QWidget* mainwidget = new QWidget(this);

   //Create a frame to store the GL widget
   QFrame* frame = new QFrame (this);
   QHBoxLayout* hlayout = new QHBoxLayout;
   hlayout->setMargin(2);
   frame->setLayout(hlayout);
   frame->setFrameStyle (QFrame::Sunken | QFrame::StyledPanel);

   //Create the GL widget and put it in the frame
   adaptor = new vgui_qt_adaptor(frame);
   hlayout->addWidget(adaptor);

   setCentralWidget(frame);


#ifndef __SGI_CC // SGI's iostream does not allow re-initialising
   vgui::out.rdbuf(statusbar.statusbuf);
#endif
}

//-----------------------------------------------------------------------------
vgui_qt_window::vgui_qt_window(int w, int h, const char* title)
:  QMainWindow(),
   statusbar (this),
   use_menubar(false),
   use_statusbar(true)
{
   setup_widget(w, h, title);
}


//--------------------------------------------------------------------------------
vgui_qt_window::vgui_qt_window(int w, int h, const vgui_menu& menu, const char* title)
:  QMainWindow(),
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

   for (unsigned int i=0; i < menu.size(); ++i)
   {
      if (menu[i].is_separator())
      {
         menuBar()->addSeparator();
      }
      else if (menu[i].is_command())
      {
         QAction* action = menuBar()->addAction(menu[i].name.c_str());
         action->setShortcut(vgui_key_to_qt(menu[i].short_cut.key,
                                            menu[i].short_cut.mod));
         //commands_[action] = menu[i].cmnd;
      }
      else if (menu[i].is_submenu())
      {
         vgui_qt_menu* qm = new vgui_qt_menu(*(menu[i].menu));
         qm->setTitle(menu[i].name.c_str());
         menuBar()->addMenu((QMenu*)qm);
      }
   }
}
