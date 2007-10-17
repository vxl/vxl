#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_shell_tableau.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_tableau.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>

#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/nodes/SoRotationXYZ.h>

// This is a very simple example of how to use the
// basic bgui3d_tableau to render an Open Inventor
// scene graph with animation in vgui.
//
// The following is loosly based on
// "Hello, Cone" Using Engines (Example 2-2)
// provided in "The Inventor Mentor" 

// Construct a simple scene
void buildScene(SoGroup *root)
{
  // Add a camera
  SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
  root->addChild(myCamera);

  // Add a light
  root->addChild(new SoDirectionalLight);

  SoRotationXYZ *myRotXYZ = new SoRotationXYZ;
  root->addChild(myRotXYZ);
  myRotXYZ->axis = SoRotationXYZ::X;
  SoElapsedTime *myCounter = new SoElapsedTime;
  myRotXYZ->angle.connectFrom(&myCounter->timeOut);

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
  vgui_window* win = vgui::produce_window(400, 400, "Basic 3D Tableau with Animation");
  win->get_adaptor()->set_tableau( shell );
  win->show();

  // Enable idle event handling for animation
  tab3d->enable_idle();

  // Run the program
  return vgui::run(); 
}
