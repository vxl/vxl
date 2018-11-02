#include <iostream>
#include <cstdio>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_math.h> // for pi_over_2

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>

#include <bgui3d/bgui3d_tableau.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoFaceSet.h>


#define WINDOW_SIZE 600
#define NUM_VERTICES 100000

int ***data;
int m_w, m_h, m_d;

void addSphere(SoGroup *root)
{
  SoSeparator *sep = new SoSeparator;
  //Add material
  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(1.0f, 1.0f, 0.0f);
  myMaterial->shininess = .75f;
  sep->addChild(myMaterial);

  SoSphere* cyl = new SoSphere;
  cyl->radius = 5.0f;
  sep->addChild(cyl);
  root->addChild(sep);
}

#define NUM_PLANES 7
#define PLANE_DIST 20
#define PLANE_DIM 200

void addSampleViewingPlanes(SoGroup *root)
{
  SoSeparator *sep = new SoSeparator;

  //Add material
  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(1.0f, 1.0f, 0.0f);
  sep->addChild(myMaterial);

  //forming vertices, norms, face vertex numbers
  static float vertices[NUM_PLANES*4][3];
  int numvertices[NUM_PLANES];
  static float norms[NUM_PLANES][3];
  for (int i=0; i < NUM_PLANES; i++)
  {
    int index = i * 4;
    int z_coord = i * PLANE_DIST;

    vertices[index  ][0] = 0;
    vertices[index  ][1] = 0;
    vertices[index  ][2] = z_coord;

    vertices[index+1][0] = 0;
    vertices[index+1][1] = PLANE_DIM;
    vertices[index+1][2] = z_coord;

    vertices[index+2][0] = PLANE_DIM;
    vertices[index+2][1] = PLANE_DIM;
    vertices[index+2][2] = z_coord;

    vertices[index+3][0] = PLANE_DIM;
    vertices[index+3][1] = 0;
    vertices[index+3][2] = z_coord;

    norms[i][0] = 0;
    norms[i][1] = 0;
    norms[i][2] = 1;

    numvertices[i] = 4;
  }

  //adding normals and normal bindings
  SoNormal *myNormals = new SoNormal;
  myNormals->vector.setValues(0, NUM_PLANES, norms);
  sep->addChild(myNormals);
  SoNormalBinding *myNormalBinding = new SoNormalBinding;
  myNormalBinding->value = SoNormalBinding::PER_FACE;
  sep->addChild(myNormalBinding);

  //adding coordinates for vertices
  SoCoordinate3 *myCoords = new SoCoordinate3;
  myCoords->point.setValues(0, NUM_PLANES*4, vertices);
  sep->addChild(myCoords);

  //define the planes (as face sets)
  SoFaceSet *myFaceSet = new SoFaceSet;
  myFaceSet->numVertices.setValues(0, NUM_PLANES, numvertices);
  sep->addChild(myFaceSet);

  root->addChild(sep);
}

void addPointSet(SoGroup *root)
{
  SoSeparator *sep_pos = new SoSeparator;
  SoSeparator *sep_neg = new SoSeparator;

  //Add material for positive-valued points
  SoMaterial *myMaterial_pos = new SoMaterial;
  myMaterial_pos->diffuseColor.setValue(1.0f, 0.0f, 0.0f);
  myMaterial_pos->shininess = .28f;
  sep_pos->addChild(myMaterial_pos);

  //Add material for negative-valued points
  SoMaterial *myMaterial_neg = new SoMaterial;
  myMaterial_neg->diffuseColor.setValue(0.0f, 0.0f, 1.0f);
  myMaterial_neg->shininess = .28f;
  sep_neg->addChild(myMaterial_neg);

  //Add drawing style
  SoDrawStyle *myDrawStyle = new SoDrawStyle;
  myDrawStyle->style = SoDrawStyle::FILLED;
  myDrawStyle->pointSize = 5.0;
  sep_pos->addChild(myDrawStyle);
  sep_neg->addChild(myDrawStyle);

  //Form vertices
  static float pos_vertices[NUM_VERTICES][3];
  int pos_pixel_num = 0;
  static float neg_vertices[NUM_VERTICES][3];
  int neg_pixel_num = 0;
  for (int k=0; k<m_d; k++)
  {
    for (int j=0; j<m_h; j++)
    {
      for (int i=0; i<m_w; i++)
      {
        if (data[k][j][i] > 0)
        {
          pos_vertices[pos_pixel_num][0] = i;
          pos_vertices[pos_pixel_num][1] = j;
          pos_vertices[pos_pixel_num][2] = k;
          pos_pixel_num++;
        }
        else if (data[k][j][i] < 0)
        {
          neg_vertices[neg_pixel_num][0] = i;
          neg_vertices[neg_pixel_num][1] = j;
          neg_vertices[neg_pixel_num][2] = k;
          neg_pixel_num++;
        }
      }
    }
  }
  //Add coordinates from formed vertices
  SoCoordinate3 *myCoords_pos = new SoCoordinate3;
  myCoords_pos->point.setValues(0, pos_pixel_num, pos_vertices);
  sep_pos->addChild(myCoords_pos);

  SoCoordinate3 *myCoords_neg = new SoCoordinate3;
  myCoords_neg->point.setValues(0, neg_pixel_num, neg_vertices);
  sep_neg->addChild(myCoords_neg);
  //Add point set
  SoPointSet *myPointSet_pos = new SoPointSet;
  myPointSet_pos->startIndex = 0;
  myPointSet_pos->numPoints = pos_pixel_num;
  sep_pos->addChild(myPointSet_pos);

  SoPointSet *myPointSet_neg = new SoPointSet;
  myPointSet_neg->startIndex = 0;
  myPointSet_neg->numPoints = neg_pixel_num;
  sep_neg->addChild(myPointSet_neg);

  root->addChild(sep_pos);
  root->addChild(sep_neg);
}

void addPlaneBoundaries(SoGroup *root)
{
  SoSeparator *sep = new SoSeparator;

  //Add material
  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(0.0f, 1.0f, 0.0f);
  myMaterial->shininess = .5f;
  sep->addChild(myMaterial);

  //Add drawing style
  SoDrawStyle *myDrawStyle = new SoDrawStyle;
  myDrawStyle->style = SoDrawStyle::FILLED;
  myDrawStyle->pointSize = 2.0;
  sep->addChild(myDrawStyle);

  //forming vertices, norms, face vertex numbers
  static float vertices[NUM_PLANES*5][3];
  int numvertices[NUM_PLANES];
  static float norms[NUM_PLANES][3];
  for (int i=0; i < NUM_PLANES; i++)
  {
    int index = i * 5;
    int z_coord = i * PLANE_DIST;

    vertices[index  ][0] = 0;
    vertices[index  ][1] = 0;
    vertices[index  ][2] = z_coord;

    vertices[index+1][0] = 0;
    vertices[index+1][1] = PLANE_DIM;
    vertices[index+1][2] = z_coord;

    vertices[index+2][0] = PLANE_DIM;
    vertices[index+2][1] = PLANE_DIM;
    vertices[index+2][2] = z_coord;

    vertices[index+3][0] = PLANE_DIM;
    vertices[index+3][1] = 0;
    vertices[index+3][2] = z_coord;

    vertices[index+4][0] = 0;
    vertices[index+4][1] = 0;
    vertices[index+4][2] = z_coord;

    norms[i][0] = 0;
    norms[i][1] = 0;
    norms[i][2] = 1;

    numvertices[i] = 5;
  }

  //adding coordinates for vertices
  SoCoordinate3 *myCoords = new SoCoordinate3;
  myCoords->point.setValues(0, NUM_PLANES*5, vertices);
  sep->addChild(myCoords);

  SoLineSet *myLineSet = new SoLineSet;
  myLineSet->numVertices.setValues(0, NUM_PLANES, numvertices);
  myLineSet->startIndex.setValue(0);
  sep->addChild(myLineSet);

  root->addChild(sep);
}

void addLineSet(SoGroup *root)
{
  SoSeparator *sep = new SoSeparator;

  //Add material
  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->ambientColor.setValue(.33f, .22f, .27f);
  myMaterial->diffuseColor.setValue(.78f, .57f, .11f);
  myMaterial->specularColor.setValue(.99f, .94f, .81f);
  myMaterial->shininess = .28f;
  sep->addChild(myMaterial);

  //Add drawing style
  SoDrawStyle *myDrawStyle = new SoDrawStyle;
  myDrawStyle->style = SoDrawStyle::FILLED;
  myDrawStyle->pointSize = 2.0;
  sep->addChild(myDrawStyle);

  static float coord_vertices[6][3];
  coord_vertices[0][0] = 0;
  coord_vertices[0][1] = 0;
  coord_vertices[0][2] = 0;

  coord_vertices[1][0] = 200;
  coord_vertices[1][1] = 0;
  coord_vertices[1][2] = 0;

  coord_vertices[2][0] = 0;
  coord_vertices[2][1] = 0;
  coord_vertices[2][2] = 0;

  coord_vertices[3][0] = 0;
  coord_vertices[3][1] = 200;
  coord_vertices[3][2] = 0;

  coord_vertices[4][0] = 0;
  coord_vertices[4][1] = 0;
  coord_vertices[4][2] = 0;

  coord_vertices[5][0] = 0;
  coord_vertices[5][1] = 0;
  coord_vertices[5][2] = 200;

  SoCoordinate3 *myCoords = new SoCoordinate3;
  myCoords->point.setValues(0, 6, coord_vertices);
  sep->addChild(myCoords);

  SoLineSet *myLineSet = new SoLineSet;
  myLineSet->startIndex.setValue(0);
  SoMFInt32 num_vertices;
  num_vertices.set1Value(0, 2);
  num_vertices.set1Value(1, 2);
  num_vertices.set1Value(2, 2);
  myLineSet->numVertices = num_vertices;
  sep->addChild(myLineSet);

  root->addChild(sep);
}

void buildScene(SoGroup *root)
{
  //Add camera
  SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
  myCamera->position = SbVec3f(0, 0, 300);
//    myCamera->orientation.setValue(SbVec3f(0, 0, -1), 0.0f);
  myCamera->nearDistance = 0.5f;
  myCamera->farDistance = 400.0f;
  myCamera->focalDistance = 300.0f;
  myCamera->heightAngle = float(vnl_math::pi_over_2);
  root->addChild(myCamera);

  //Add light
  root->addChild(new SoDirectionalLight);

  addPlaneBoundaries(root);
  addSphere(root);
  addPointSet(root);

  //SbViewportRegion vpr(WINDOW_SIZE, WINDOW_SIZE);
  //myCamera->viewAll(root, vpr);
}

void read_raw_data_file()
{
  std::FILE *fp;
  fp = std::fopen("E:\\MyDocs\\Temp\\filter_x=2.hdr", "r");
  std::fscanf(fp, "%d %d %d\n", &m_w, &m_h, &m_d);
  std::printf("width: %d\nheight: %d\ndepth: %d\n", m_w, m_h, m_d);
  std::fclose(fp);
  data = (int ***)malloc(sizeof(int)*m_d);
  for (int k = 0; k < m_d; k++)
    data[k] = (int **)malloc(sizeof(int)*m_h);

  for (int k = 0; k < m_d; k++){
    for (int j = 0; j < m_h; j++){
      data[k][j] = (int *)malloc(sizeof(int)*m_w);}}

  int x;
  fp = std::fopen("E:\\MyDocs\\Temp\\filter_x=2.txt", "r");
  for (int k = 0; k < m_d; k++)
  {
    for (int j = 0; j < m_h; j++)
    {
      for (int i = 0; i < m_w; i++) {
        std::fscanf(fp, "%d", &x);
        data[k][j][i] = x;
      }
    }
  }
  std::fclose(fp);
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

    read_raw_data_file();
  buildScene(root);

  // wrap the scene graph in a bgui3d tableau
  bgui3d_examiner_tableau_new tab3d(root);
  root->unref();

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  vgui_window* win = vgui::produce_window(WINDOW_SIZE, WINDOW_SIZE, "Can is working");
  win->get_adaptor()->set_tableau( shell );
  win->show();

    // Enable idle event handling for animation
  //tab3d->enable_idle();

  // Run the program
  return vgui::run();
//    return vgui::run(shell, 400, 400);
}
