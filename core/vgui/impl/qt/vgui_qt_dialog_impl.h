// This is core/vgui/impl/qt/vgui_qt_dialog_impl.h
#ifndef VGUI_QT_DIALOG_IMPL_H_
#define VGUI_QT_DIALOG_IMPL_H_
//:
// \file
// \brief QT Dialog wrapper
// \author Joris Schouteden
//
// \verbatim
//  Modifications
//   27.03.2000 JS  Initial Version, adapted from vgui_Fl_dialog_impl
//   26.10.2000 JS  Added file browser and text message
//   09.11.2000 JS  Added colour browser
// \endverbatim

#include <vgui/internals/vgui_dialog_impl.h>
#include <qdialog.h>

//: QT implementation of vgui_dialog_impl.
class vgui_qt_dialog_impl :
   public QDialog,
   public vgui_dialog_impl
{
   Q_OBJECT
 public:
    vgui_qt_dialog_impl(const char* name);
   ~vgui_qt_dialog_impl() { };

   void* bool_field_widget(const char*, bool&);
   void* int_field_widget(const char*, int&);
   void* long_field_widget(const char*, long&);
   void* float_field_widget(const char*, float&);
   void* double_field_widget(const char*, double&);
   void* string_field_widget(const char*, vcl_string&);
   void* choice_field_widget(const char*, const vcl_vector<vcl_string>&, int&);

   void* text_message_widget(const char*);
   void* file_browser_widget(const char*, vcl_string&, vcl_string&);
   void* inline_file_browser_widget(const char *,vcl_string&, vcl_string&);
   void* color_chooser_widget(const char *,vcl_string&);//, vcl_string&);
   void* inline_color_chooser_widget(const char *,vcl_string&);//, vcl_string&);

   void  modal(const bool);

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
   vgui_qt_filebrowser_impl(QWidget* parent, const char*, vcl_string&, vcl_string&);
  ~vgui_qt_filebrowser_impl() { };

   vcl_string  file() const { return vcl_string(edit_->text().latin1()); };

 public slots:
   void get_a_file();

 private:
   vcl_string& filter_;
   QLineEdit*  edit_;
};

class vgui_qt_colorchooser_impl : public QHGroupBox
{
   Q_OBJECT
 public:
   vgui_qt_colorchooser_impl(QWidget* parent, const char*, vcl_string&);
  ~vgui_qt_colorchooser_impl() { };

  vcl_string  color() const { return value_; };

 public slots:
   void get_a_color();

 private:
   vcl_string& value_;
   QFrame*     frame_;
};

#endif // VGUI_QT_DIALOG_IMPL_H_
