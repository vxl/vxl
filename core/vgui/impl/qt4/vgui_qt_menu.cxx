#include "vgui_qt_menu.h"
//:
// \file
#include "vgui_qt_adaptor.h"


//---------------------------------------------------------------------------
vgui_qt_menu::vgui_qt_menu(const vgui_menu& menu, QWidget * parent )
  : QMenu(parent)
{
   connect(this, SIGNAL(triggered(QAction*)), this, SLOT(upon_trigger(QAction*)));

   for (unsigned int i=0; i < menu.size(); ++i)
   {
      if (menu[i].is_separator())
      {
         this->addSeparator();
      }
      else if (menu[i].is_command())
      {
         QAction* action = this->addAction(menu[i].name.c_str());
         action->setShortcut(vgui_key_to_qt(menu[i].short_cut.key,
                                            menu[i].short_cut.mod));
         commands_[action] = menu[i].cmnd;
      }
      else if (menu[i].is_submenu())
      {
         vgui_qt_menu* qm = new vgui_qt_menu(*(menu[i].menu));
         qm->setTitle(menu[i].name.c_str());
         this->addMenu((QMenu*)qm);
      }
   }
}


//---------------------------------------------------------------------------


void vgui_qt_menu::upon_trigger(QAction * action) const
{
  vcl_map<QAction*, vgui_command_sptr>::const_iterator i = commands_.find(action);
  if (i != commands_.end() && bool(i->second))
    i->second->execute();
}


//---------------------------------------------------------------------------
//: Convert a vgui keypress into a QT key press
QKeySequence vgui_key_to_qt(vgui_key key, vgui_modifier mod)
{
  int keypress = 0;
  keypress += vgui_qt_adaptor::translate(mod);
  keypress += vgui_qt_adaptor::translate(key);

  return QKeySequence( keypress );
}
