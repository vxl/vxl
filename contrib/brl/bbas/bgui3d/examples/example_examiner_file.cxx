#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>

#include <bgui3d/bgui3d_file_io.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>

#include <Inventor/nodes/SoSeparator.h>

// This is a very simple example of how to use read
// a scene from a file and display it in an examiner_tableau


int main(int argc, char** argv)
{
  if (argc <2) {
    vcl_cerr << "Please specify the path to scene file (IV or VRML)\n";
    return -1;
  }
  vcl_string filename(argv[1]);
  // initialize vgui
  vgui::init(argc, argv);

  // initialize bgui_3d
  bgui3d_init();

  // read the file into a scene graph
  SoNode* root = bgui3d_import_file(filename);
  root->ref();

  // wrap the scenegraph in an examiner tableau
  bgui3d_examiner_tableau_new tab3d(root);
  root->unref();

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 400, 400);
}
