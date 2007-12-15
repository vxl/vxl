#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>

#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>

// This is a very simple example of how to use the
// bgui3d_examiner_tableau to render an Open Inventor
// scene graph with multiple roots and the same camera
// in a vgui_grid_tableau.


int main(int argc, char** argv)
{
  // initialize vgui
  vgui::init(argc, argv);

  // initialize bgui_3d
  bgui3d_init();

  SoPerspectiveCamera* camera = new SoPerspectiveCamera;
  // create the scene graph root
  SoSeparator *root1 = new SoSeparator;
  root1->ref();

  SoSeparator *root2 = new SoSeparator;
  root2->ref();

  root1->addChild( camera );
  root2->addChild( camera );

  SoCone* cone = new SoCone;
  root1->addChild( cone );

  SoCylinder* cylinder = new SoCylinder;
  root2->addChild( cylinder );

  // wrap each scene graph in an examiner tableau
  bgui3d_examiner_tableau_new tab3d_1(root1);
  bgui3d_examiner_tableau_new tab3d_2(root2);
  root1->unref();
  root2->unref();

  //Put the viewers into a grid
  vgui_grid_tableau_sptr grid = new vgui_grid_tableau(2,1);
  grid->add_at(tab3d_1, 0,0);
  grid->add_at(tab3d_2, 1,0);

  // Put the grid into a shell tableau at the top the hierarchy
  vgui_shell_tableau_new shell(grid);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 800, 400);
}
