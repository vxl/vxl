#ifndef VGUI_QT_MENU_H_
#define VGUI_QT_MENU_H_
//:
// \file
// \brief vgui_menu wrapped into a QT popupmenu
//
// \author
//  Joris Schouteden, ESAT, K.U.Leuven
//
// \verbatim
//  Modifications
//   24.03.2000 JS  Initial Version
//   02.05.2007 Christoph_John@gmx.de ported to QT 4.2.2
// \endverbatim
//-----------------------------------------------------------------------------

#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vbl/vbl_smart_ptr.h>

#include <Qt3Support/q3popupmenu.h>
#include <Qt/qobject.h>

typedef vbl_smart_ptr<vgui_command> vgui_command_sptr;

//: QT implementation of vgui_menu.
class vgui_qt_menu : public Q3PopupMenu
{
  Q_OBJECT
 public:
  vgui_qt_menu(const vgui_menu& menuke);
 ~vgui_qt_menu() { delete[] commands_; };

 public slots:
  void  upon_activated(int id);

 private:
  vgui_command_sptr*  commands_;
};

#endif // VGUI_QT_MENU_H_
