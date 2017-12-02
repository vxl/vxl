// This is core/vgui/impl/qt/vgui_qt_dialog_impl.h
#ifndef VGUI_QT_DIALOG_IMPL_H_
#define VGUI_QT_DIALOG_IMPL_H_
//:
// \file
// \brief QT Dialog wrapper
// \author Joris Schouteden, ESAT, K.U.Leuven
//
// \verbatim
//  Modifications
//   27.03.2000 JS  Initial Version, adapted from vgui_Fl_dialog_impl
//   26.10.2000 JS  Added file browser and text message
//   09.11.2000 JS  Added colour browser
//   14.11.2005 Chanop Silpa-Anan  adapted to QT 3.3.5 for X11/Mac
// \endverbatim

#include <vgui/internals/vgui_dialog_impl.h>
#include <qdialog.h>
#include <qobject.h>

//: QT implementation of vgui_dialog_impl.
class vgui_qt_dialog_impl :
   public QDialog,
   public vgui_dialog_impl
{
   Q_OBJECT
 public:
    vgui_qt_dialog_impl(const char* name);
   ~vgui_qt_dialog_impl() {}

   void* bool_field_widget(const char*, bool&);
   void* int_field_widget(const char*, int&);
   void* long_field_widget(const char*, long&);
   void* float_field_widget(const char*, float&);
   void* double_field_widget(const char*, double&);
   void* string_field_widget(const char*, std::string&);
   void* choice_field_widget(const char*, const std::vector<std::string>&, int&);

   void* text_message_widget(const char*);
   void* file_browser_widget(const char*, std::string&, std::string&);
   void* inline_file_browser_widget(const char *,std::string&, std::string&);
   void* color_chooser_widget(const char *,std::string&);//, std::string&);
   void* inline_color_chooser_widget(const char *,std::string&);//, std::string&);
   void* inline_tableau_widget(const vgui_tableau_sptr tab,
                               unsigned int width, unsigned int height);
   void  modal(bool m);

   bool  ask();
};

#include <qhgroupbox.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qframe.h>

class vgui_qt_filebrowser_impl : public QHGroupBox
{
   Q_OBJECT
 public:
   vgui_qt_filebrowser_impl(QWidget* parent, const char*, std::string&, std::string&);
  ~vgui_qt_filebrowser_impl() {}

   std::string  file() const { return std::string(edit_->text().latin1()); }

 public slots:
   void get_a_file();

 private:
   std::string& filter_;
   QLineEdit*  edit_;
};

class vgui_qt_colorchooser_impl : public QHGroupBox
{
   Q_OBJECT
 public:
   vgui_qt_colorchooser_impl(QWidget* parent, const char*, std::string&);
  ~vgui_qt_colorchooser_impl() {}

  std::string  color() const { return value_; }

 public slots:
   void get_a_color();

 private:
   std::string& value_;
   QFrame*     frame_;
};


class vgui_qt_tableau_impl : public QWidget
{
   Q_OBJECT
 public:
   vgui_qt_tableau_impl(QWidget* parent, const vgui_tableau_sptr tab,
                        unsigned int width, unsigned int height);
  ~vgui_qt_tableau_impl() {}
};

#endif // VGUI_QT_DIALOG_IMPL_H_
