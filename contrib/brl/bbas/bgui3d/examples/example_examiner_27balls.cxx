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

///#include <Inventor/nodes/SoRotationXYZ.h>
///#include <Inventor/draggers/SoTranslate1Dragger.h>

// Construct a simple scene
void buildScene(SoGroup *root)
{
  // Add a camera
  SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
  root->addChild(myCamera);

  int num = 5;  
  float fnum = (float)num; 
  
  SoSeparator* sep = new SoSeparator;
  root->addChild( sep );
  SoCoordinate3* coords = new SoCoordinate3;
  coords->point.set1Value(0, -1, -1, -1 );
  coords->point.set1Value(1, -2, -2, -2 );
  SoLineSet* face = new SoLineSet;
  sep->addChild( coords );
  sep->addChild( face );
   

  // 27 BALLS EXAMPLE
  for(int x = 0; x<num; ++x)
  {
    for(int y =0; y<num; ++y)
    {
      for(int z = 0; z<num; ++z)
      {
       
        SoSeparator* sep = new SoSeparator;
        root->addChild(sep);
        SoSphere* cyl = new SoSphere;
        cyl->radius = (x+y+z)/(float)num;
        SoMaterial *myMaterial = new SoMaterial;
        SoTranslation *trans = new SoTranslation;
        myMaterial->diffuseColor.setValue(x/fnum, y/fnum, z/fnum);
        trans->translation.setValue(x*num,y*num,z*num);
        sep->addChild(myMaterial);
        sep->addChild(trans);
        sep->addChild(cyl);

      }
    }
  }
  
  SoHandleBoxDragger* dragger = new SoHandleBoxDragger;  
  sep->addChild( dragger );

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

  
    /*SoSeparator * group = new SoSeparator;

    SoTranslation * trans = new SoTranslation;
    group->addChild(trans);
    trans->translation.setValue(0, 0, 0.0f);

    SoRotationXYZ * rotate = new SoRotationXYZ;
    group->addChild(rotate);
    rotate->axis = SoRotationXYZ::Z;
    rotate->angle = 1 * M_PI / 180.0f;

    group->addChild(new SoTranslate1Dragger);
    
    root->addChild (group);*/


  // wrap the scene graph in a bgui3d tableau
  bgui3d_examiner_tableau_new tab3d(root);
  root->unref();

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 400, 400);
}
