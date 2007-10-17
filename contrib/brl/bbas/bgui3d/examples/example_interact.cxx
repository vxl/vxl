#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vcl_cmath.h>

#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/draggers/SoHandleBoxDragger.h>

#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoDragPointDragger.h>


// Print instructions on stdout.
void show_instructions (void)
{
  (void)fprintf(stdout, "\nThis example program shows off the dragpoint dragger.\n");
  (void)fprintf(stdout, "\nQuick instructions:\n\n");
  (void)fprintf(stdout, "  - Use left mouse botton to rotate, mid mouse button to pan.\n");
  (void)fprintf(stdout, "  - Pick and drag plane or stick with CTRL + left mouse button.\n");
  (void)fprintf(stdout, "\n");
}

// Construct a transparent sphere with position controlled by the
// translation field of a SoDragPointDragger.

SoSeparator* make_transparent_sphere (SbColor color, SoSFVec3f * draggerfield)
{
  SoSeparator * sub = new SoSeparator;

  SoMaterial * mat = new SoMaterial;
  sub->addChild(mat);
  mat->transparency = 0.1;
  mat->diffuseColor.setValue(color);
  mat->emissiveColor.setValue(color / 2);

  SoTranslation * trans = new SoTranslation;
  sub->addChild(trans);
  trans->translation.connectFrom(draggerfield);

  SoPickStyle * pickstyle = new SoPickStyle;
  sub->addChild(pickstyle);
  pickstyle->style = SoPickStyle::UNPICKABLE;

  SoTranslation * offset = new SoTranslation;
  sub->addChild(offset);
  offset->translation.setValue(0, -3.5, 0);

  SoSphere * sphere = new SoSphere;
  sub->addChild(sphere);
  sphere->radius = 3;

  return sub;
}

int main(int argc, char** argv)
{
  // initialize bgui_3d
  bgui3d_init();

  show_instructions ();

  // create the scene graph root
  SoSeparator *root = new SoSeparator;
  root->ref();

  //###################################################################

  const SbVec3f startpos[] = {
    SbVec3f(0, 0, 0), SbVec3f(5, 0, -1), SbVec3f(2, 5, -2)
  };
  const SbColor colors[] = {
    SbColor(1, 0, 0), SbColor(0, 1, 0), SbColor(0, 0, 1)
  };

  for (int i = 0; i < 3; i++) {
    SoDragPointDragger* dragger = new SoDragPointDragger;
    root->addChild(dragger);
    dragger->translation.setValue(startpos[i]);
    root->addChild(make_transparent_sphere(colors[i], &dragger->translation));
  }

  //###################################################################

  //Run the specified cmd-line process with visualization in GUI Window.  
  //force option "--mfc-use-gl" to use gl in initializing vgui.    
  vcl_cout << "Starting bgui3d window...\n";
  int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = "--mfc-use-gl";
  vgui::init (my_argc, my_argv);
  delete []my_argv;
  
  //Wrap the scene graph in an examiner tableau
  bgui3d_examiner_tableau_new tab3d (root);
  root->unref ();
  //Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell (tab3d);
  //Create a window, add the tableau and show it on screen.
  int result = vgui::run (shell, 800, 600);
  
  return result;
}
