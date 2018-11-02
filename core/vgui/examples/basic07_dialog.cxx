// This example makes a dialog box and attaches it to a menu item.
// When the user has added values to the dialog box and clicked on OK
// their list of choices will be printed to cerr.

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_bool_ostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

// Make a vgui.dialog:
static void test_dialog()
{
  static int int_value = 2;
  static long long_value = 3;
  static float float_value = 3.1f;
  static double double_value = 4.2;
  static std::string string_value = "dialog test";
  static bool bool_value = true;
  static std::string inline_file_value = "/tmp/myfile_inline.txt";
  static std::string file_value = "/tmp/myfile.txt";
  static std::string regexp = "*.txt";
  static std::string inline_color_value = "blue";
  static std::string color_value = "red";

  static int choice_value = 1;
  std::vector<std::string> labels;
  labels.push_back(std::string("fltk"));
  labels.push_back(std::string("motif"));
  labels.push_back(std::string("gtk"));
  labels.push_back(std::string("glut"));
  labels.push_back(std::string("glX"));

  vgui_dialog mydialog("My dialog");
  mydialog.field("int value", int_value);
  mydialog.field("long value", long_value);
  mydialog.field("float value", float_value);
  mydialog.field("double value", double_value);
  mydialog.field("string value", string_value);
  mydialog.checkbox("bool value", bool_value);
  mydialog.choice("choice value", labels, choice_value);
  mydialog.inline_file("inline file browser", regexp, inline_file_value);
  mydialog.file("file browser", regexp, file_value);
  mydialog.inline_color("inline color value", inline_color_value);
  mydialog.color("color value", color_value);

  if (mydialog.ask())
  {
    std::cerr << "OK pressed.\n";
    std::cerr << "int_value : " << int_value << std::endl;
    std::cerr << "long_value : " << long_value << std::endl;
    std::cerr << "float_value : " << float_value << std::endl;
    std::cerr << "double_value : " << double_value << std::endl;
    std::cerr << "string_value : " << string_value << std::endl;
    std::cerr << "bool_value : " << vbl_bool_ostream::true_false(bool_value) << std::endl;
    std::cerr << "choice_value : " << choice_value << " " << labels[choice_value] << std::endl;
    std::cerr << "inline_file_value: " << inline_file_value << std::endl;
    std::cerr << "file_value: " << file_value << std::endl;
    std::cerr << "inline_color_value: " << inline_color_value << std::endl;
    std::cerr << "color_value: " << color_value << std::endl;
  } else {
    std::cerr << "Cancel pressed.\n";
    std::cerr << "int_value : " << int_value << std::endl;
    std::cerr << "long_value : " << long_value << std::endl;
    std::cerr << "float_value : " << float_value << std::endl;
    std::cerr << "double_value : " << double_value << std::endl;
    std::cerr << "string_value : " << string_value << std::endl;
    std::cerr << "bool_value : " << vbl_bool_ostream::true_false(bool_value) << std::endl;
    std::cerr << "choice_value : " << choice_value << " " << labels[choice_value] << std::endl;
    std::cerr << "inline_file_value: " << inline_file_value << std::endl;
    std::cerr << "file_value: " << file_value << std::endl;
    std::cerr << "inline_color_value: " << inline_color_value << std::endl;
    std::cerr << "color_value: " << color_value << std::endl;
  }
}

static void test_dialog2()
{
  vgui_dialog mydialog("My dialog2");
  vgui_image_tableau_new image("az32_10.tif");
  vgui_viewer2D_tableau_new viewer(image);
  mydialog.inline_tableau(viewer, 512, 512);

  mydialog.message("A picture");

  std::string button_txt("close");
  mydialog.set_ok_button(button_txt.c_str());
  mydialog.set_cancel_button(nullptr);
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
  vgui_viewer2D_tableau_new viewer(image);
  vgui_shell_tableau_new shell(viewer);

  // Create a window with a menu, add the tableau and show it on screen:
  return vgui::run(shell, image->width(), image->height(), create_menus());
}
