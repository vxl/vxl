// This is oxl/ogui/examples/kym/dialog-example.cxx

//:
// \file
// This example makes a dialog box and attaches it to a menu item.

#include <vbl/vbl_bool_ostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_shell_tableau.h>

// Make a vgui.dialog:
static void test_dialog()
{
  static int int_value = 2;
  static long long_value = 3;
  static float float_value = 3.1f;
  static double double_value = 4.2;
  static vcl_string string_value = "dialog test";
  static bool bool_value = true;
  static vcl_string file_value = "/tmp/myfile.txt";
  static vcl_string regexp = "*.txt";
   static vcl_string color_value = "blue";

  static int choice_value = 1;
  vcl_vector<vcl_string> labels;
  labels.push_back(vcl_string("fltk"));
  labels.push_back(vcl_string("motif"));
  labels.push_back(vcl_string("gtk"));
  labels.push_back(vcl_string("glut"));
  labels.push_back(vcl_string("glX"));

  vgui_dialog mydialog("My dialog");
  mydialog.field("int value", int_value);
  mydialog.field("long value", long_value);
  mydialog.field("float value", float_value);
  mydialog.field("double value", double_value);
  mydialog.field("string value", string_value);
  mydialog.checkbox("bool value", bool_value);
  mydialog.choice("choice value", labels, choice_value);
  mydialog.inline_file("file browser", regexp, file_value);
  mydialog.inline_color("color value", color_value);

  if (mydialog.ask())
  {
    vcl_cerr << "int_value : " << int_value << vcl_endl;
    vcl_cerr << "long_value : " << long_value << vcl_endl;
    vcl_cerr << "float_value : " << float_value << vcl_endl;
    vcl_cerr << "double_value : " << double_value << vcl_endl;
    vcl_cerr << "string_value : " << string_value << vcl_endl;
    vcl_cerr << "bool_value : " << vbl_bool_ostream::true_false(bool_value) << vcl_endl;
    vcl_cerr << "choice_value : " << choice_value << " " << labels[choice_value] << vcl_endl;
    vcl_cerr << "file_value: " << file_value << vcl_endl;
    vcl_cerr << "color_value: " << color_value << vcl_endl;
  }
}

static void test_dialog2()
{
  vgui_dialog mydialog("My dialog2");
  vgui_image_tableau_new image("az32_10.tif");
  vgui_viewer2D_new viewer(image);
  mydialog.inline_tableau(viewer, 512, 512);

  mydialog.message("A picture");

  vcl_string button_txt("close");
  mydialog.set_ok_button(button_txt.c_str());
  mydialog.set_cancel_button(0);
  mydialog.ask();
}

// Create a vgui.menu with an item which shows the dialog box:
vgui_menu create_menus()
{
  vgui_menu test;
  test.add("Dialog", test_dialog);
  test.add("Dialog2", test_dialog2);

  vgui_menu bar;
  bar.add("Test",test);

  return bar;
}

int main(int argc, char ** argv)
{
  vgui::init(argc,argv);

  vgui_image_tableau_new image(argc>1 ? argv[1] : "az32_10.tif");
  vgui_viewer2D_new viewer(image);
  vgui_shell_tableau_new shell(viewer);

  // Create a window with a menu, add the tableau and show it on screen:
  return vgui::run(shell, image->width(), image->height(), create_menus());
}

