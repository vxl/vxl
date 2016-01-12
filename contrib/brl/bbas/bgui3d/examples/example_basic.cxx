#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_tableau.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>

// This is a very simple example of how to use the
// basic bgui3d_tableau to render an Open Inventor
// scene graph in vgui.
//
// The following is loosly based on
// Basic "Hello, Cone" Program (Example 2-1)
// provided in "The Inventor Mentor"

// Construct a simple scene
void buildScene(SoGroup *root)
{
  // Add a camera
  SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
  root->addChild(myCamera);

  // Add a light
  root->addChild(new SoDirectionalLight);

  // Add a red material
  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(1.0f, 0.0f, 0.0f);  // Red
  root->addChild(myMaterial);

  // Add a cone
  root->addChild(new SoCone);

  SbViewportRegion vpr(400,400);
  myCamera->viewAll(root, vpr);
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

  // wrap the scene graph in a bgui3d tableau
  bgui3d_tableau_new tab3d(root);
  root->unref();

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 400, 400);
}
