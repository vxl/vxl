// This is core/vgui/impl/qt/vgui_qt_dialog_impl.cxx
#include "vgui_qt_dialog_impl.h"

#include <vcl_vector.h>
#include <vcl_iostream.h>

#include <vgui/internals/vgui_dialog_impl.h>
#include <vgui/internals/vgui_simple_field.h>

#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qvalidator.h>
#include <qstring.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qcolordialog.h>

static bool is_modal = true;

//-----------------------------------------------------------------------------
vgui_qt_dialog_impl::vgui_qt_dialog_impl(const char* name)
:  QDialog(0, name, is_modal), vgui_dialog_impl(name)
{
   setCaption(name);
}


//-----------------------------------------------------------------------------
bool vgui_qt_dialog_impl::ask()
{
   QPushButton *ok, *cancel;
   ok = new QPushButton( ok_button_text_.c_str(), this );
   connect( ok, SIGNAL(clicked()), SLOT(accept()) );
   cancel = new QPushButton( cancel_button_text_.c_str(), this );
   connect( cancel, SIGNAL(clicked()), SLOT(reject()) );

   ok->setMinimumSize(cancel->width(), cancel->height());
   ok->setMaximumSize(cancel->width(), cancel->height());

   QVBoxLayout* total = new QVBoxLayout(this, 10, -1, "totallayout");
   QVBoxLayout* layout = new QVBoxLayout(total, -1, "vboxlayout");
   QHBoxLayout* lower = new QHBoxLayout(total, -1, "buttonslayout");

   lower->addStretch(1);
   lower->addWidget(ok, 0);
   lower->addStretch(1);
   lower->addWidget(cancel, 0);
   lower->addStretch(1);

   for (vcl_vector<element>::iterator ei = elements.begin();
        ei != elements.end(); ++ei)
   {
      element l = (*ei);
      QWidget* widget = static_cast<QWidget*>(l.widget);
      if (!widget) vcl_cerr << "No QWidget defined for element type " << l.type << vcl_endl;
      else
      {
         if (l.type == int_elem    ||
             l.type == long_elem   ||
             l.type == float_elem  ||
             l.type == double_elem ||
             l.type == string_elem ||
             l.type == bool_elem   ||
             l.type == choice_elem)
         {
            QHBoxLayout* hbox = new QHBoxLayout(layout, -1);
            QLabel* label = new QLabel(widget, l.field->label.c_str(), this);
            hbox->addWidget(label, 0);
            hbox->addStretch(1);
            hbox->addWidget(widget, 0);
         }
         else if (l.type == text_msg)
         {
            layout->addWidget(widget, 0);
         }
         else if (l.type == file_bsr)
         {
            layout->addWidget(widget, 0);
         }
         else if (l.type == inline_file_bsr)
         {
            layout->addWidget(widget, 0);
         }
         else if (l.type == color_csr)
         {
            layout->addWidget(widget, 0);
         }
         else if (l.type == inline_color_csr)
         {
            layout->addWidget(widget, 0);
         }
      }
   }

   bool result = exec();

   if (result == true)
   {
      for (vcl_vector<element>::iterator ei = elements.begin();
           ei != elements.end(); ei++)
      {
         element l = (*ei);
         if (l.type == long_elem ||
             l.type == float_elem ||
             l.type == double_elem ||
             l.type == string_elem)
         {
            QLineEdit* input = static_cast<QLineEdit*>(l.widget);
            l.field->update_value(input->text().ascii());
         }
         else if (l.type == int_elem)
         {
            QSpinBox* input = static_cast<QSpinBox*>(l.widget);
            l.field->update_value(input->text().ascii());
         }
         else if (l.type == bool_elem)
         {
            QCheckBox* button = static_cast<QCheckBox*>(l.widget);
            vgui_bool_field* field = static_cast<vgui_bool_field*>(l.field);
            field->var = button->isChecked();
         }
         else if (l.type == choice_elem)
         {
            QComboBox* box = static_cast<QComboBox*>(l.widget);
            vgui_int_field *field = static_cast<vgui_int_field*>(l.field);
            field->var = box->currentItem();
         }
         else if ((l.type == inline_file_bsr) || (l.type == file_bsr))
         {
            vgui_qt_filebrowser_impl* fileb = static_cast<vgui_qt_filebrowser_impl*>(l.widget);
            l.field->update_value(fileb->file());
         }
         else if ((l.type == inline_color_csr) || (l.type == color_csr))
         {
            vgui_qt_colorchooser_impl* fileb = static_cast<vgui_qt_colorchooser_impl*>(l.widget);
            l.field->update_value(fileb->color());
         }
      }
   }

   delete ok;
   delete cancel;
   return result;
}


//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::bool_field_widget(const char* txt, bool& v)
{
   QCheckBox* widget = new QCheckBox(this);
   widget->setChecked(v);
   return widget;
}

//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::int_field_widget(const char* txt, int& v)
{
   QSpinBox* widget = new QSpinBox(-65000, 65000, 1, this);
   widget->setValue(v);
   return widget;
}


//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::long_field_widget(const char* txt, long& v)
{
   QString s; s.setNum(v);
   QLineEdit* widget = new QLineEdit(s, this);
   widget->setValidator(new QIntValidator(widget));
   return widget;
}


//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::float_field_widget(const char* txt, float& v)
{
   QString s; s.setNum(v);
   QLineEdit* widget = new QLineEdit(s, this);
   widget->setValidator(new QDoubleValidator(widget));
   return widget;
}


//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::double_field_widget(const char* txt, double& v)
{
   QString s; s.setNum(v);
   QLineEdit* widget = new QLineEdit(s, this);
   widget->setValidator(new QDoubleValidator(widget));
   return widget;
}


//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::string_field_widget(const char* txt, vcl_string& v)
{
   QLineEdit* widget = new QLineEdit(v.c_str(), this);
   return widget;
}


//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::choice_field_widget(const char* txt, const vcl_vector<vcl_string>& labels, int& v)
{
   QComboBox* widget = new QComboBox(this);
   for (vcl_vector<vcl_string>::const_iterator si = labels.begin(); si != labels.end(); si++)
   {
      widget->insertItem((*si).c_str());
   }
   widget->setCurrentItem(v);
   return widget;
}


//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::text_message_widget(const char* txt)
{
   QLabel* widget = new QLabel(txt, this, "text_message");
   return widget;
}


//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::file_browser_widget(const char* txt, vcl_string& v, vcl_string& s)
{
   vgui_qt_filebrowser_impl* widget = new vgui_qt_filebrowser_impl(this, txt, v, s);
   return widget;
}


//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::inline_file_browser_widget(const char * txt,vcl_string& v, vcl_string& s)
{
   vgui_qt_filebrowser_impl* widget = new vgui_qt_filebrowser_impl(this, txt, v, s);
   return widget;
}


//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::color_chooser_widget(const char * txt,vcl_string& val)//, vcl_string& s)
{
   vgui_qt_colorchooser_impl* widget = new vgui_qt_colorchooser_impl(this, txt, val);
   return widget;
}


//-----------------------------------------------------------------------------
void* vgui_qt_dialog_impl::inline_color_chooser_widget(const char * txt,vcl_string& val)//, vcl_string& s)
{
   vgui_qt_colorchooser_impl* widget = new vgui_qt_colorchooser_impl(this, txt, val);
   return widget;
}


//-----------------------------------------------------------------------------
vgui_qt_filebrowser_impl::vgui_qt_filebrowser_impl(QWidget* parent, const char* txt, vcl_string& v, vcl_string& s)
: QHGroupBox(txt, parent), filter_(v)
{
   edit_ = new QLineEdit(s.c_str(), this);
   QPushButton* push = new QPushButton("Browse", this, "go_browse");
   connect(push, SIGNAL(clicked()), this, SLOT(get_a_file()));
}


//-----------------------------------------------------------------------------
void vgui_qt_filebrowser_impl::get_a_file()
{
   QString r = QFileDialog::getSaveFileName(edit_->text(), filter_.c_str());
   if (!r.isEmpty()) edit_->setText(r);
}


//-----------------------------------------------------------------------------
vgui_qt_colorchooser_impl::vgui_qt_colorchooser_impl(QWidget* parent, const char* txt, vcl_string& val)
: QHGroupBox(txt, parent), value_(val)
{
   frame_ = new QFrame(this, "farm");
   frame_->setLineWidth(2);
   frame_->setFrameStyle(QFrame::Sunken | QFrame::Panel);
   frame_->setBackgroundColor(QColor(val.c_str()));
   QPushButton* push = new QPushButton("Pick", this, "go_browse");
   connect(push, SIGNAL(clicked()), this, SLOT(get_a_color()));
}


//-----------------------------------------------------------------------------
void vgui_qt_colorchooser_impl::get_a_color()
{
   QColor c = QColorDialog::getColor(QColor(value_.c_str()));
   if (c.isValid())
   {
      value_ = c.name().ascii();
      frame_->setBackgroundColor(c);
   }
}
