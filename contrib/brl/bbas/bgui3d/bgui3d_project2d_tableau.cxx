// This is brl/bbas/bgui3d/bgui3d_project2d_tableau.cxx
#include "bgui3d_project2d_tableau.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_det.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vpgl/vpgl_perspective_camera.h>
#include "bgui3d_algo.h"

#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/SbLinear.h>


//: Default Constructor - don't use this, use bgui3d_project2d_tableau_new.
bgui3d_project2d_tableau::bgui3d_project2d_tableau()
 : bgui3d_tableau(NULL), draw_headlight_(true)
{
  // Use the identity camera
  this->set_camera(vpgl_proj_camera<double>());
}


//: Constructor - don't use this, use bgui3d_project2d_tableau_new.
bgui3d_project2d_tableau::bgui3d_project2d_tableau( const vpgl_proj_camera<double>& camera,
                                                   SoNode * scene_root )
: bgui3d_tableau(scene_root), draw_headlight_(true)
{
  this->set_camera(camera);
}


//: Destructor
bgui3d_project2d_tableau::~bgui3d_project2d_tableau()
{
}


vcl_string bgui3d_project2d_tableau::type_name() const
{
  return "bgui3d_project2d_tableau";
}


//: Set the scene camera
// creates a graphics camera from a vpgl camera (either perspective or affine)
bool
bgui3d_project2d_tableau::set_camera(const vpgl_proj_camera<double>& cam)
{
  vnl_double_3x4 camera = cam.get_matrix();

  vnl_double_3x3 K; // internal parameters
  vnl_double_3x3 R; // camera rotation
  vnl_double_3 t; // camera translation

  // Code for affine camera
  if ( camera[2][0] == 0 && camera[2][1] ==  0 && camera[2][2] == 0 )
  {
    vnl_double_3x4 ncamera = camera;
    ncamera /= ncamera[2][3];

    // Decompose the camera as:
    // | a b c | | 1 0 0 0 | |   r1  0 |
    // |   d e |*| 0 1 0 0 |*|   r2  0 |
    // |     1 | | 0 0 0 1 | |   r3  0 |
    //                       | 0 0 0 1 |
    K.fill( 0.0 );
    vnl_double_3 r1, r2, r3, a1;
    r2[0] = ncamera[1][0]; r2[1] = ncamera[1][1]; r2[2] = ncamera[1][2];
    K[1][1] = vcl_sqrt( r2[0]*r2[0]+r2[1]*r2[1]+r2[2]*r2[2] );
    r2 /= vcl_sqrt( r2[0]*r2[0]+r2[1]*r2[1]+r2[2]*r2[2] );
    a1[0] = ncamera[0][0]; a1[1] = ncamera[0][1]; a1[2] = ncamera[0][2];
    r3 = vnl_cross_3d( a1, r2 );
    r3 /= vcl_sqrt( r3[0]*r3[0]+r3[1]*r3[1]+r3[2]*r3[2] );
    r1 = vnl_cross_3d( r2, r3 );
    K[0][0] = a1[0]*r1[0] + a1[1]*r1[1] + a1[2]*r1[2];
    if ( K[0][0] < 0 ) {
      K[0][0] *= -1;
      r1 *= -1;
    }
    K[0][1] = a1[0]*r2[0] + a1[1]*r2[1] + a1[2]*r2[2];
    if ( K[1][1] < 0 ) {
      K[1][1] *= -1;
      r2 *= -1;
      K[0][1] *= -1;
    }
    K[0][2] = ncamera[0][3];
    K[1][2] = ncamera[1][3];
    K[2][2] = 1.0;

    // Check recomposition
    vnl_double_3x4 mcamera( 0.0 );
    vnl_double_4x4 rcamera( 0.0 );
    mcamera[0][0] = mcamera[1][1] = mcamera[2][3] = 1;
    rcamera[0][0] = r1[0]; rcamera[0][1] = r1[1]; rcamera[0][2] = r1[2];
    rcamera[1][0] = r2[0]; rcamera[1][1] = r2[1]; rcamera[1][2] = r2[2];
    rcamera[2][0] = r3[0]; rcamera[2][1] = r3[1]; rcamera[2][2] = r3[2];
    rcamera[3][3] = 1.0;
    assert( (K*mcamera*rcamera - ncamera).fro_norm() > 1e-4 );
    // if this fails email tpollard@dam.brown.edu for help

    t.fill( 0.0 );
    R[0][0] = r1[0];  R[0][1] = r1[1];  R[0][2] = r1[2];
    R[1][0] = r2[0];  R[1][1] = r2[1];  R[1][2] = r2[2];
    R[2][0] = r3[0];  R[2][1] = r3[1];  R[2][2] = r3[2];
  }
  // Code for perspective camera:
  else{
    // make the camera right-handed
    vnl_double_3x4 cam = camera;
    if (vnl_det(vnl_double_3x3(cam.extract(3,3))) < 0)
      cam *= -1.0;
    if (!bgui3d_decompose_camera(cam, K, R, t)){
      vcl_cerr << "decomposition error\n\n";
      return false;
    }
  }

  // The model matrix is the cameras rotation and translation
  vnl_double_4x4 M(0.0);
  M.update(R);
  M.set_column(3, t);
  M(3,3) = 1.0;

  // set M to model_matrix_ for OpenGL use
  vnl_matrix_ref<double> mm(4,4,model_matrix_);
  mm = M.transpose();

  // The inverse of the model matrix
  vnl_double_4x4 Mi(0.0);
  Mi.update(R.transpose());
  Mi.set_column(3, -R.transpose()*t);
  Mi(3,3) = 1.0;

  // Apply the inverse of the model matrix to the camera
  camera_z_ = camera * Mi;

  // The resulting left 3x3 submatrix must be upper triangle
  // check this condition and force it to be exactly true.
  assert(vcl_fabs(camera_z_(1,0))<1e-10);
  assert(vcl_fabs(camera_z_(2,0))<1e-10);
  assert(vcl_fabs(camera_z_(2,1))<1e-10);
  camera_z_(1,0) = camera_z_(2,0) = camera_z_(2,1) = 0.0;

  return true;
}


//: Get the scene camera
// creates a vpgl camera (either perspective or affine) from the graphics camera
vcl_auto_ptr<vpgl_proj_camera<double> >
bgui3d_project2d_tableau::camera() const
{
  vnl_matrix<double> mm(4,4,16,model_matrix_);
  vgl_rotation_3d<double> R(mm.extract(3,3).transpose());
  vgl_point_3d<double> t(-mm[3][0], -mm[3][1], -mm[3][2]);
  t = R.inverse()*t;
  if (camera_z_[2][2] != 0){
    vpgl_calibration_matrix<double> K(camera_z_.extract(3,3));
    return vcl_auto_ptr<vpgl_proj_camera<double> >
        ( new vpgl_perspective_camera<double>(K,t,R) );
  }
  else
    // FIXME - construct a vpgl_affine_camera
    return vcl_auto_ptr<vpgl_proj_camera<double> >
      ( new vpgl_proj_camera<double>(camera_z_*mm.transpose()) );
}


//: Draw a headlight in the direction of the camera
static void draw_headlight()
{
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  GLfloat light0_pos[4]   = {  0.0, 0.0, -1.0, 0.0 };
  GLfloat light0_diff[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat light0_amb[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat light0_spec[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

  glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
  glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_amb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diff);
  glLightfv(GL_LIGHT0, GL_SPECULAR,  light0_spec);

  glEnable(GL_LIGHT0);
}


//: Handle vgui events
bool
bgui3d_project2d_tableau::handle(const vgui_event& e)
{
  // Handle draw events
  if ( e.type == vgui_DRAW || e.type == vgui_DRAW_OVERLAY ){
    // Store state, but don't rely on GL stacks since they might be full
    double P[16] , M[16];
    glGetDoublev(GL_PROJECTION_MATRIX, P);
    glGetDoublev(GL_MODELVIEW_MATRIX, M);

    if (this->setup_projection()) {
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      if (draw_headlight_)
        draw_headlight();
      else
        glDisable(GL_LIGHT0);
      glLoadMatrixd(model_matrix_);
      if ( e.type == vgui_DRAW )
        return this->render();
      if ( e.type == vgui_DRAW_OVERLAY )
        return this->render_overlay();
    }

    // Restore the OpenGL state
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(P);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(M);
  }

  return bgui3d_tableau::handle(e);
}


//: setup the OpenGL projection matrix
bool
bgui3d_project2d_tableau::setup_projection()
{
  // Get current modelview matrix state and combine with projection state
  // This moves the 2D panning and zooming into the projection stage
  // The modelview matrix will now be used for 3D orientation and translation
  double M[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, M);
  glMatrixMode(GL_PROJECTION);
  glMultMatrixd(M);

  // Compute a bounding box around the world to determine near and far
  // clipping planes
  SoGetBoundingBoxAction sbba(get_viewport_region());
  sbba.apply(scene_root_);

  SbXfBox3f xbox = sbba.getXfBoundingBox();
  SbMatrix mat;
  mat.makeIdentity();
  for (int i=0; i<4; ++i)
    for (int j=0; j<4; ++j)
      mat[i][j] = float(model_matrix_[4*i+j]);
  xbox.transform(mat);
  mat = xbox.getTransform();
  SbBox3f box = xbox.project();

  double nearval = box.getMin()[2];
  double farval =  box.getMax()[2];

  // Compute the missing 3rd row of the matrix such that
  // nearval and farval map to -1 and 1 in depth

  // Assume that the camera has been rotated to point down the
  // -z axis.  Thus the bottom row of the camera is [0 0 W1 W2]
  assert(!camera_z_(2,0) && !camera_z_(2,1));
  // actually map between -1+epsilon and 1-epsilon to be safe from clipping
  double epsilon = 0.01;
  double denom = (farval - nearval)/(1.0-epsilon);
  double w1 = camera_z_[2][2];
  double w2 = camera_z_[2][3];
  double z2 = -(w1*(farval+nearval) + 2*w2)/denom;
  double z3 = (2*w1*farval*nearval + w2*(farval+nearval))/denom;

  double P[4][4] = {
    { camera_z_(0,0),               0,   0,               0},
    { camera_z_(0,1),  camera_z_(1,1),   0,               0},
    { camera_z_(0,2),  camera_z_(1,2),  z2,  camera_z_(2,2)},
    { camera_z_(0,3),  camera_z_(1,3),  z3,  camera_z_(2,3)}
  };

  // Compose this projection with the panning and zooming from vgui
  glMultMatrixd((double *)P);

  return true;
}


//----------------------------------------------------------------------------
//: A vgui command used to toggle animation
class bgui3d_headlight_command : public vgui_command
{
 public:
  bgui3d_headlight_command(bgui3d_project2d_tableau* tab) : bgui3d_project2d_tab(tab) {}
  void execute()
  {
    bool headlight = bgui3d_project2d_tab->is_headlight();
    bgui3d_project2d_tab->set_headlight(!headlight);
  }

  bgui3d_project2d_tableau *bgui3d_project2d_tab;
};

//----------------------------------------------------------------------------
//: Builds a popup menu
void bgui3d_project2d_tableau::get_popup(const vgui_popup_params& params,
                                         vgui_menu &menu)
{
  vcl_string headlight_item;
  if ( this->is_headlight() )
    headlight_item = "Disable Headlight";
  else
    headlight_item = "Enable Headlight";

  menu.add(headlight_item, new bgui3d_headlight_command(this));
}
