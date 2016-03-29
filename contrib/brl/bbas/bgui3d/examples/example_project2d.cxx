#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_composite_tableau.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_project2d_tableau.h>

#include <vnl/vnl_math.h> // for pi_over_4
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_rotation_matrix.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>

#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <vgui/vgui_deck_tableau.h>

// This is a very simple example of how to use the
// basic bgui3d_project2d_tableau to render an Open Inventor
// scene graph with a 3x4 camera matrix onto an image plane
// compatible with vgui_viewer2d_tableau.
//

// Construct a simple scene
void buildScene(SoGroup *root)
{
  {
    SoSeparator *group = new SoSeparator;

    SoTransform *myTransform = new SoTransform;
    myTransform->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
    myTransform->translation.setValue(0.0f, 0.0f, 20.0f);
    group->addChild(myTransform);

    // Add a red material
    SoMaterial *myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(1.0f, 0.0f, 0.0f);  // Red
    group->addChild(myMaterial);

    // Add a cone
    group->addChild(new SoCone);

    root->addChild(group);
  }
  {
    SoSeparator *group = new SoSeparator;

    SoTransform *myTransform = new SoTransform;
    myTransform->scaleFactor.setValue(0.5f, 0.5f, 0.5f);
    myTransform->translation.setValue(1.0f, 1.0f, 20.0f);
    group->addChild(myTransform);

    // Add a red material
    SoMaterial *myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(0.0f, 1.0f, 0.0f);  // Green
    group->addChild(myMaterial);

    // Add a cone
    group->addChild(new SoCone);
    root->addChild(group);
  }
}


// make the example image
vil_image_resource_sptr
make_image()
{
  vil_image_view<vxl_byte> image(400, 400, 3);
  for (int i=0; i<400; ++i) {
    for (int j=0; j<400; ++j) {
      image(i,j,0) = (vxl_byte)(((i+j)%2)*255);
      image(i,j,1) = (vxl_byte)(((i/2+j/2)%2)*255);
      image(i,j,2) = (vxl_byte)(((i/4+j/4)%2)*255);
    }
  }
  return vil_new_image_resource_of_view(image);
}


// Construct the camera
vnl_double_3x4 make_camera()
{
  // The calibration matrix
  vnl_double_3x3 K;
  K[0][0] = 2000.0;  K[0][1] = 0.0;      K[0][2] = 200.0;
  K[1][0] = 0.0;     K[1][1] = 2000.0;   K[1][2] = 200.0;
  K[2][0] = 0.0;     K[2][1] = 0.0;      K[2][2] = 1.0;

  // The rotation about the x axis
  double angle = vnl_math::pi_over_4;
  vnl_double_3x3 R = vnl_rotation_matrix(angle*(vnl_double_3(1.0, 0.0, 0.0).normalize()));

  // The translation
  vnl_double_3 t(0.0, 10.0, 10.0);

  R = R.transpose();

  vnl_double_3x4 C;
  C.update(R.as_ref()); // left 3x3 part
  C.set_column(3, -R*t);
  std::cout << "Camera =\n" <<K*C << std::endl;

  return K*C;
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

  vnl_double_3x4 camera = make_camera();

#if 0
  // wrap the scene graph in a bgui3d tableau
  bgui3d_project2d_tableau_new tab3d(camera, root);
#endif // 0

  bgui3d_project2d_tableau_sptr proj_tab = bgui3d_project2d_tableau_new(camera,root);
  bgui3d_examiner_tableau_sptr exam_tab = bgui3d_examiner_tableau_new(root);
  exam_tab->set_camera(camera);
  exam_tab->save_home_position();

  vgui_deck_tableau_sptr tab3d = vgui_deck_tableau_new();
  tab3d->add(exam_tab);
  tab3d->add(proj_tab);

  root->unref();

  // add an image tableau
  vgui_image_tableau_new img_tab(make_image());

  // add a composite tableau
  vgui_composite_tableau_new comp(img_tab, tab3d);

  // add a 2D viewer
  vgui_viewer2D_tableau_new viewer(comp);

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(viewer);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 400, 400);
}
