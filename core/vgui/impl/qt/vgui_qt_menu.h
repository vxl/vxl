#ifndef _VGUI_QT_MENU_H_
#define _VGUI_QT_MENU_H_

// .NAME vgui_qt_menu - vgui_menu wrapped into a QT popupmenu
// .HEADER vxl package
// .LIBRARY vgui-qt
// .INCLUDE vgui/impl/qt/vgui_qt_menu.h
// .FILE vgui_qt_menu.cxx

// .SECTION Author
// Joris Schouteden

// .SECTION Modifications:
// 24.03.2000 JS  Initial Version

#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vbl/vbl_smart_ptr.h>

#include <qpopupmenu.h>

typedef vbl_smart_ptr<vgui_command> vgui_command_ref;

class vgui_qt_menu : public QPopupMenu
{
   Q_OBJECT
public:
   vgui_qt_menu(const vgui_menu& menuke);
  ~vgui_qt_menu() { delete[] commands_; };
  
public slots:
   void  upon_activated(int id);
   
private:
   vgui_command_ref*  commands_;
};

#endif // _VGUI_QT_MENU_H_
