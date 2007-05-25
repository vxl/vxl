#ifndef vgui_qt_menu_h_
#define vgui_qt_menu_h_
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
//   23.05.2007 Matt Leotta  converted to QT3 compatibility functions to native QT4
// \endverbatim
//-----------------------------------------------------------------------------

#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vcl_map.h>

#include <qmenu.h>
#include <Qt/qobject.h>

typedef vbl_smart_ptr<vgui_command> vgui_command_sptr;

//: QT implementation of vgui_menu.
class vgui_qt_menu : public QMenu
{
  Q_OBJECT
 public:
  vgui_qt_menu(const vgui_menu& menu, QWidget * parent = 0 );
 ~vgui_qt_menu() {}

 public slots:
  void upon_trigger(QAction * action) const;

 private:
  vcl_map<QAction*, vgui_command_sptr> commands_;
};

//: Convert a vgui keypress into a QT key press
QKeySequence vgui_key_to_qt(vgui_key key, vgui_modifier = vgui_MODIFIER_NULL);

#endif // vgui_qt_menu_h_
