// This is core/vgui/impl/qt4/vgui_qt_dialog_impl.h
#ifndef vgui_qt_dialog_impl_h_
#define vgui_qt_dialog_impl_h_
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
//   02.05.2007 Christoph_John@gmx.de ported to QT 4.2.2
//   23.05.2007 Matt Leotta  converted to QT3 compatibility functions to native QT4
// \endverbatim

#include <vgui/internals/vgui_dialog_impl.h>

#include <QDialog>
#include <QObject>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>



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



class vgui_qt_filebrowser_impl : public QGroupBox
{
   Q_OBJECT
 public:
   vgui_qt_filebrowser_impl(QWidget* parent, const std::string& t, const std::string& f, const std::string& s);
  ~vgui_qt_filebrowser_impl() {}

  std::string  file() const { return std::string(edit_->text().toLatin1()); }

 public slots:
   void get_a_file();

 private:
   std::string title_;
   std::string filter_;
   QLineEdit*  edit_;
};

class vgui_qt_colorchooser_impl : public QGroupBox
{
   Q_OBJECT
 public:
   vgui_qt_colorchooser_impl(QWidget* parent, const char*, std::string&);
  ~vgui_qt_colorchooser_impl() {}

  std::string  color() const { return value_; }

 private:
  void update_color_string();

 public slots:
  void get_a_color();
  void change_red(int);
  void change_green(int);
  void change_blue(int);
  void change_alpha(int);

 private:
   std::string& value_;
   QColor      color_;
   QFrame*     frame_;
   QSpinBox   *rbox_, *gbox_, *bbox_, *abox_;
};


class vgui_qt_tableau_impl : public QWidget
{
   Q_OBJECT
 public:
   vgui_qt_tableau_impl(QWidget* parent, const vgui_tableau_sptr tab,
                        unsigned int width, unsigned int height);
  ~vgui_qt_tableau_impl() {}
};

#endif // vgui_qt_dialog_impl_h_
