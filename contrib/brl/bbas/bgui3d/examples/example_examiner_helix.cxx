#include <iostream>
#include <cmath>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoMaterial.h>

// This is a very simple example of how to use the
// bgui3d_examiner_tableau to render an Open Inventor
// scene graph with animation in vgui.

// Construct a simple scene
void buildScene(SoGroup *root)
{
  SoSeparator* group = new SoSeparator;

  for (int i=0; i<4; ++i) {
    SoCoordinate3* line_coords = new SoCoordinate3;
    SoMaterialBinding* material_bind = new SoMaterialBinding;
    SoMaterial* vertex_material = new SoMaterial;
    SoLineSet* line = new SoLineSet;
    material_bind->value = SoMaterialBinding::PER_VERTEX;

    int coord_num = 0;
    for ( float t=0.0f; t<=1.0f; t+=0.001f, ++coord_num) {
      line_coords->point.set1Value(coord_num, float(i*std::sin(t*10*(5-i))), float(i*std::cos(t*10*(5-i))), 4*t+i);
      vertex_material->diffuseColor.set1Value(coord_num, i*0.25f , 1.0f - t, t);
    }

    line->numVertices.setValue(coord_num);
    group->addChild(material_bind);
    group->addChild(line_coords);
    group->addChild(vertex_material);
    group->addChild(line);
  }

  // Add a cone
  root->addChild(group);
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
