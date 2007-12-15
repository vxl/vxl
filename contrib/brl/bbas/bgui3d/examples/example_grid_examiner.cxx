#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>

#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoMaterial.h>


// This is a very simple example of how to use the
// bgui3d_examiner_tableau to render an Open Inventor
// scene graph with multiple views in a vgui_grid_tableau.

// Construct a simple scene
void buildScene(SoGroup *root)
{
  // Add a red material
  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(1.0f, 0.0f, 0.0f);  // Red
  root->addChild(myMaterial);

  // Add a cone
  root->addChild(new SoCone);
}


int main(int argc, char** argv)
{
  // initialize vgui
  vgui::init(argc, argv);

  // initialize bgui_3d
  bgui3d_init();

  // create the scene graph root
  SoSeparator *root = new SoSeparator;
  root->ref();
  buildScene(root);

  // wrap the scene graph in an examiner tableau
  bgui3d_examiner_tableau_new tab3d_1(root);
  bgui3d_examiner_tableau_new tab3d_2(root);
  root->unref();

  //Put the viewers into a grid
  vgui_grid_tableau_sptr grid = new vgui_grid_tableau(2,1);
  grid->add_at(tab3d_1, 0,0);
  grid->add_at(tab3d_2, 1,0);

  // Put the grid into a shell tableau at the top the hierarchy
  vgui_shell_tableau_new shell(grid);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 800, 400);
}
