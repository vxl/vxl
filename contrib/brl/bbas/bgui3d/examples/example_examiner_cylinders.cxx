#include <iostream>
#include <cmath>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vgl/vgl_vector_3d.h>

#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoMaterial.h>

#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/SbLinear.h>

// Add sphere
void addSphere(SoGroup *root)
{
  SoSeparator* sep = new SoSeparator;
  root->addChild(sep);
  SoSphere *sph = new SoSphere;
  sph->radius = 10.0;

  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->transparency.setValue(0.5);

  sep->addChild(myMaterial);
  sep->addChild(sph);
}

// Add a cylinder
void addCylinder(SoGroup *root, vgl_vector_3d<double> p)
{
  SoSeparator* sep = new SoSeparator;
  root->addChild(sep);
  SoCylinder *cyl = new SoCylinder;
  cyl->radius = 1.0;
  cyl->height = 20.0;

  SoRotation *rot = new SoRotation;
  p /= p.length();
  SbVec3f axis(float(p.z()), 0.0f, -float(p.x()));
  rot->rotation.setValue(axis, float(std::acos(p.y())));

  SoMaterial *myMaterial = new SoMaterial;
//  myMaterial->diffuseColor.setValue(p.x(), p.y(), p.z());
  myMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);

  sep->addChild(rot);
  sep->addChild(myMaterial);
  sep->addChild(cyl);
}

// Construct a simple scene
void buildScene(SoGroup *root)
{
  // Add camera
  SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
  myCamera->position = SbVec3f(0, 0, 100);
  myCamera->nearDistance = 0.5f;
  myCamera->farDistance = 400.0f;
  myCamera->focalDistance = 100.0f;
  myCamera->heightAngle = (float)vnl_math::pi_over_2;
  root->addChild(myCamera);

  addSphere(root);
  // xy-plane
  addCylinder(root, vgl_vector_3d<double>( 1.0,0.0,0.0));
  addCylinder(root, vgl_vector_3d<double>( 1.0,1.0,0.0));
  addCylinder(root, vgl_vector_3d<double>( 0.0,1.0,0.0));
  addCylinder(root, vgl_vector_3d<double>(-1.0,1.0,0.0));
  // xz-plane
//  addCylinder(root, vgl_vector_3d<double>( 1.0,0.0,0.0));
  addCylinder(root, vgl_vector_3d<double>( 1.0,0.0,1.0));
  addCylinder(root, vgl_vector_3d<double>( 0.0,0.0,1.0));
  addCylinder(root, vgl_vector_3d<double>(-1.0,0.0,1.0));
  // yz-plane
//  addCylinder(root, vgl_vector_3d<double>(0.0, 1.0,0.0));
  addCylinder(root, vgl_vector_3d<double>(0.0, 1.0,1.0));
//  addCylinder(root, vgl_vector_3d<double>(0.0, 0.0,1.0));
  addCylinder(root, vgl_vector_3d<double>(0.0,-1.0,1.0));
  // intermediate planes
  addCylinder(root, vgl_vector_3d<double>( 1.0, 1.0, 1.0));
  addCylinder(root, vgl_vector_3d<double>(-1.0, 1.0, 1.0));
  addCylinder(root, vgl_vector_3d<double>( 1.0,-1.0, 1.0));
  addCylinder(root, vgl_vector_3d<double>( 1.0, 1.0,-1.0));

  myCamera->viewAll(root, SbViewportRegion() );
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
  bgui3d_examiner_tableau_new tab3d(root);
  root->unref();

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 400, 400);
}
