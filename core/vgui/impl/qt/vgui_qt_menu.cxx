#include <iostream>
#include "vgui_qt_menu.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//---------------------------------------------------------------------------
vgui_qt_menu::vgui_qt_menu(const vgui_menu& menu)
: QPopupMenu()
{
   connect(this, SIGNAL(activated(int)), this, SLOT(upon_activated(int)));

   //int i = 1;
   commands_ = new vgui_command_sptr[menu.size()];
   for (unsigned int i=0; i < menu.size(); ++i)
   {
      if (menu[i].is_separator())
      {
         insertSeparator();
      }
      else if (menu[i].is_command())
      {
         insertItem(menu[i].name.c_str(), i);
         commands_[i] = menu[i].cmnd;
      }
      else if (menu[i].is_submenu())
      {
         vgui_qt_menu* qm = new vgui_qt_menu(*(menu[i].menu));
         insertItem(menu[i].name.c_str(), (QPopupMenu*)qm, i);
      }
   }
}


//---------------------------------------------------------------------------
void vgui_qt_menu::upon_activated(int id)
{
#ifdef DEBUG
   std::cerr << "upon_activated " << id << std::endl;
#endif
   vgui_command_sptr c = commands_[id];
   c->execute();
}
