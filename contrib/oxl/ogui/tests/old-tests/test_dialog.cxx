//:
// \file
// \author Philip C. Pritchett, Oxford RRG
// \date   05 Oct 99
//-----------------------------------------------------------------------------

#include <vbl/vbl_bool_ostream.h>

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>

static void test_dialog(const void*)
{
  vcl_cerr << "making dialog\n";

  static int int_value = 2;
  static long long_value = 3;
  static float float_value = 3.1;
  static double double_value = 4.2;
  static vcl_string string_value = "dialog test";
  static bool bool_value = true;

  static int choice_value = 1;
  vcl_vector<vcl_string> labels;
  labels.push_back(vcl_string("fltk"));
  labels.push_back(vcl_string("motif"));
  labels.push_back(vcl_string("gtk"));
  labels.push_back(vcl_string("glut"));
  labels.push_back(vcl_string("glX"));

  vgui_dialog mydialog("My dialog");

  mydialog.message("This is a message");
  mydialog.field("int value", int_value);
  mydialog.field("long value", long_value);
  mydialog.field("float value", float_value);
  mydialog.field("double value", double_value);
  mydialog.field("vcl_string value", string_value);
  mydialog.checkbox("bool value", bool_value);
  mydialog.choice("choice value", labels, choice_value);

  if (mydialog.ask()) {
    vcl_cerr << "int_value : " << int_value << vcl_endl
             << "long_value : " << long_value << vcl_endl
             << "float_value : " << float_value << vcl_endl
             << "double_value : " << double_value << vcl_endl
             << "string_value : " << string_value << vcl_endl
             << "bool_value : " << vbl_bool_ostream::true_false(bool_value) << vcl_endl
             << "choice_value : " << choice_value << ' ' << labels[choice_value] << vcl_endl;
  }
}


vgui_menu create_menus()
{
  vgui_menu test;
  test.add("Dialog",(test_dialog));

  vgui_menu bar;
  bar.add("Test",test);

  return bar;
}


int main(int argc, char ** argv)
{
  if (argc < 2) return 1;

  vgui::init(argc,argv);

  vgui_image_tableau image(argv[1]);
  vgui_viewer2D viewer(&image);

#if 0
  vgui_shell_tableau shell(&viewer);
  return vgui::run(&shell, 512, 512, create_menus());
#endif // 0
  return vgui::run(&viewer, 512, 512, create_menus(), "vgui");
}
