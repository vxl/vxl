#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>

#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoMaterial.h>

#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/nodes/SoRotationXYZ.h>

// This is a very simple example of how to use the
// bgui3d_examiner_tableau to render an Open Inventor
// scene graph with animation in vgui.

// Construct a simple scene
void buildScene(SoGroup *root)
{
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
  bgui3d_examiner_tableau_new tab3d(root);
  root->unref();

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 400, 400);
}
