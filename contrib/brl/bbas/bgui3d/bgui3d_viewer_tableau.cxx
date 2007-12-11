// This is brl/bbas/bgui3d/bgui3d_viewer_tableau.cxx
#include "bgui3d_viewer_tableau.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include "bgui3d_algo.h"
#include <vgui/vgui_gl.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_det.h>

#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/SoDB.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/VRMLnodes/SoVRMLViewpoint.h>
#include <Inventor/SoSceneManager.h>

#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/nodes/SoText2.h>

#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/sensors/SoAlarmSensor.h>

//: Constructor
bgui3d_viewer_tableau::bgui3d_viewer_tableau(SoNode * scene_root)
 : bgui3d_tableau(NULL),
   user_scene_root_(NULL),
   camera_group_(NULL),
   scene_camera_(NULL),
   stored_camera_(NULL),
   headlight_(NULL)
{
  this->set_scene_root(scene_root);
}


//: Destructor
bgui3d_viewer_tableau::~bgui3d_viewer_tableau()
{
  if (scene_camera_)
    scene_camera_->unref();
  if (headlight_)
    headlight_->unref();
}


void
bgui3d_viewer_tableau::set_scene_root(SoNode* scene_root)
{
  if (!scene_root)
    bgui3d_tableau::set_scene_root(scene_root);

  user_scene_root_ = scene_root;

  // Build the super scene graph
  SoSeparator *super_root = new SoSeparator;
  super_root->setName("bgui3d_viewer_root");

  // Create a headlight if one does not exist
  // By inserting this before any scenegraph camera, the
  // light will always be pointing in the correct direction.
  if (!headlight_){
    headlight_ = new SoDirectionalLight;
    headlight_->direction.setValue(1, -1, -10);
    headlight_->setName("headlight");
    headlight_->ref();
  }
  super_root->addChild(headlight_);

  // add the text and its translation to shift it to the top right
  SoTranslation* trans = new SoTranslation;
  trans->translation = SbVec3f(-.98f,.93f,0);
  super_root->addChild( trans );

  _text = new SoText2;
  _text->string.deleteValues(0, -1); //empty the string
  super_root->addChild( _text );

  //: Make a group of all the cameras in the scene
  camera_group_ = new SoSwitch;
  camera_group_->setName("bgui3dCameraGroup");
  super_root->addChild(camera_group_);

  // Make the default Camera
  SoCamera* camera = (SoCamera*)SoPerspectiveCamera::getClassTypeId().createInstance();
  camera->setName("Examiner_Camera");
  camera_group_->addChild(camera);
  camera->nearDistance = 0.5f;
  camera->farDistance = 1000.0f;

  // Set the camera to view the whole scene
  camera->viewAll(scene_root, get_viewport_region());

  // find existing VRML viewpoints in the scene and make cameras
  this->collect_vrml_cameras(scene_root);

  // find and used the first user scene camera (if it exists)
  vcl_vector<SoCamera*> user_cams = find_cameras(user_scene_root_);
  if (!user_cams.empty()){
    camera_group_->whichChild.setValue(-1);
    this->set_camera(user_cams[0]);
  }
  else{
    // if not, use the first examiner camera
    assert(camera_group_->getChildren()->getLength() > 0);

    if (camera_group_->whichChild.getValue() < 0)
      camera_group_->whichChild.setValue(0);
    int cam_idx = camera_group_->whichChild.getValue();

    SoChildList* list = camera_group_->getChildren();
    this->set_camera((SoCamera*)(*list)[cam_idx]);
  }

  // The users scene should be added last
  super_root->addChild(scene_root);

  bgui3d_tableau::set_scene_root(super_root);

  save_home_position();
}


//--------------Camera Methods--------------------

//: Set the camera viewing the scene
// \note this does not add the camera to a scene graph
void
bgui3d_viewer_tableau::set_camera(SoCamera *camera)
{
  if (scene_camera_){
    scene_camera_->unref();
  }

  scene_camera_ = camera;

  if (scene_camera_) {
    scene_camera_->ref();
    SoType cam_type = scene_camera_->getTypeId();
    if ( cam_type == SoOrthographicCamera::getClassTypeId() )
      camera_type_ = ORTHOGONAL;
    else
      camera_type_ = PERSPECTIVE;
  }
}


//: Set the scene camera
// Generate an SoCamera from a camera matrix and use it
// \note Only handles perspective cameras for now
bool
bgui3d_viewer_tableau::set_camera(const vpgl_proj_camera<double>& camera)
{
  SoPerspectiveCamera* new_cam = new SoPerspectiveCamera;
  new_cam->ref();
  new_cam->setName("Camera_Matrix");

  vnl_double_3x3 K, R;
  vnl_double_3 t;

  vnl_double_3x4 cam = camera.get_matrix();
  if (vnl_det(vnl_double_3x3(cam.extract(3,3))) < 0)
    cam *= -1.0;
  if ( bgui3d_decompose_camera(cam, K, R, t) ){
    new_cam->aspectRatio = K[0][2]/K[1][2];
    new_cam->heightAngle = 2.0*vcl_atan(K[1][2]/K[1][1]);

    vnl_double_3 C = -R.transpose()*t;
    new_cam->position.setValue( float(C[0]), float(C[1]), float(C[2]) );

    // the identity camera requires a 180 degree rotation about the
    // x-axis to project to images with the origin in the upper left
    R.scale_row(1,-1);
    R.scale_row(2,-1);

    // create a rotation matrix
    SbMatrix rot = SbMatrix::identity();

    for (int i=0; i<3; ++i)
      for ( int j=0; j<3; ++j)
        rot[j][i] = R[j][i];

    new_cam->orientation.setValue(SbRotation(rot));

    new_cam->nearDistance = 1.0f;
    new_cam->farDistance = 1000.0f;
  }

  if ( user_scene_root_ ){
    camera_group_->addChild(new_cam);
    int num_cameras = camera_group_->getChildren()->getLength();
    this->select_camera(num_cameras -1);
    this->set_camera(new_cam);

    this->set_clipping_planes();
  }

  new_cam->unref();

  return true;
}


//: Get the scene camera.
// Creates a vpgl camera (either perspective or affine) from the active SoCamera
vcl_auto_ptr<vpgl_proj_camera<double> >
bgui3d_viewer_tableau::camera() const
{
  if (!scene_camera_)
    return vcl_auto_ptr<vpgl_proj_camera<double> >(NULL);

  const SbVec3f& t_vec = scene_camera_->position.getValue();
  vnl_double_3 t(t_vec[0], t_vec[1], t_vec[2]);


  float q1,q2,q3,q4;
  scene_camera_->orientation.getValue().getValue(q1,q2,q3,q4);
  // inverse and rotate 180 degrees around Z
  vgl_rotation_3d<double> R(vnl_quaternion<double>(q2,-q1,q4,q3));


  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  unsigned width = vp[2];
  unsigned height = vp[3];

  switch (camera_type_)
  {
   case PERSPECTIVE: {
    SoPerspectiveCamera* cam = (SoPerspectiveCamera*)scene_camera_;
    double f = 1.0/(vcl_tan(cam->heightAngle.getValue()/2.0));
    double sx = 1.0, sy = 1.0;
    if (width < height)
      sy = double(width)/height;
    else
      sx = double(height)/width;
    vgl_point_2d<double> p(0, 0);
    vpgl_calibration_matrix<double> K(f,p,sx,sy);
    vgl_point_3d<double> c(t[0],t[1],t[2]);
    return vcl_auto_ptr<vpgl_proj_camera<double> >
           ( new vpgl_perspective_camera<double>(K,c,R) );
    }
   case ORTHOGONAL: {
    SoOrthographicCamera* cam = (SoOrthographicCamera*)scene_camera_;
    //double h = cam->height.getValue();
    vcl_cerr << "WARNING: not implemented yet\n";
    return vcl_auto_ptr<vpgl_proj_camera<double> >(NULL);
    }
  }
  return vcl_auto_ptr<vpgl_proj_camera<double> >(NULL);
}


//: Select the active camera by index.
// A negative index selects the first user scene camera
void
bgui3d_viewer_tableau::select_camera(int camera_index)
{
  int num_cameras = camera_group_->getChildren()->getLength();

  if (camera_index >= 0 && camera_index < num_cameras){
    if (camera_index != camera_group_->whichChild.getValue()){
      camera_group_->whichChild.setValue(camera_index);
      SoChildList* list = camera_group_->getChildren();
      this->set_camera((SoCamera*)(*list)[camera_index]);
    }
  }
  else{
    vcl_vector<SoCamera*> user_cams = find_cameras(user_scene_root_);
    if (!user_cams.empty()){
      camera_group_->whichChild.setValue(-1);
      this->set_camera(user_cams[0]);
    }
  }
}


//: Return the camera viewing the scene
SoCamera*
bgui3d_viewer_tableau::camera_node() const
{
  // FIX ME
  return scene_camera_;
}


//: Set the camera type (Perspective or Orthogonal)
void
bgui3d_viewer_tableau::set_camera_type(camera_type_enum type)
{
  SoType ptype = SoPerspectiveCamera::getClassTypeId();
  SoType otype = SoOrthographicCamera::getClassTypeId();
  SoCamera* newCamera;
  if ( camera_type_ != type )
  {
    if ( camera_type_ == PERSPECTIVE && type == ORTHOGONAL )
    {
      newCamera = (SoCamera *)otype.createInstance();
      convertPerspective2Ortho((SoPerspectiveCamera*)scene_camera_, (SoOrthographicCamera*)newCamera);
    }
    else //if ( camera_type_ == ORTHOGONAL && type == PERSPECTIVE )
    {
      newCamera = (SoCamera *)ptype.createInstance();
      convertOrtho2Perspective((SoOrthographicCamera*)scene_camera_, (SoPerspectiveCamera*)newCamera);
    }

    newCamera->ref();
    vcl_vector<SoGroup *> cameraparents = get_parents_of_node(this->scene_camera_);
    for (vcl_vector<SoGroup *>::iterator cp = cameraparents.begin(); cp != cameraparents.end(); ++cp)
    {
      (*cp)->replaceChild((*cp)->findChild(this->scene_camera_), newCamera);
    }

    this->set_camera(newCamera);

    camera_group_->whichChild.setValue(camera_group_->findChild(this->scene_camera_));
    newCamera->unref();
  }
}


//: Return the camera type (Perspective or Orthogonal)
bgui3d_viewer_tableau::camera_type_enum
bgui3d_viewer_tableau::camera_type() const
{
  return camera_type_;
}


//: Toggle the camera type between Perspective and Orthogonal
void
bgui3d_viewer_tableau::toggle_camera_type()
{
  if (camera_type_ == ORTHOGONAL)
    set_camera_type(PERSPECTIVE);
  else
    set_camera_type(ORTHOGONAL);
}


//: Adjust the camera to view the entire scene
void
bgui3d_viewer_tableau::view_all()
{
    scene_camera_->viewAll( user_scene_root_, get_viewport_region() );
}


//: Save the current camera as the home position
void
bgui3d_viewer_tableau::save_home_position()
{
  if (! this->scene_camera_) return; // probably a scene-less viewer

  // We use SoType::createInstance() to store a copy of the camera,
  // not just assuming it's either a perspective or an orthographic
  // camera.

  SoType t = this->scene_camera_->getTypeId();
  assert(t.isDerivedFrom(SoNode::getClassTypeId()));
  assert(t.canCreateInstance());

  if (this->stored_camera_)
    this->stored_camera_->unref();

  this->stored_camera_ = (SoNode *)t.createInstance();
  this->stored_camera_->ref();
  this->stored_camera_->copyContents(this->scene_camera_, false);
}


//: Restore the saved home position of the camera
void
bgui3d_viewer_tableau::reset_to_home_position()
{
  if (! this->scene_camera_) { return; } // probably a scene-less viewer
  if (! this->stored_camera_) { return; }

  SoType t = this->scene_camera_->getTypeId();
  SoType s = this->stored_camera_->getTypeId();

  // most common case
  if (t == s) {
    this->scene_camera_->copyContents(this->stored_camera_, false);
  }
  // handle common case #1
  else if (t == SoOrthographicCamera::getClassTypeId() &&
           s == SoPerspectiveCamera::getClassTypeId()) {
    this->convertPerspective2Ortho((SoPerspectiveCamera *)this->stored_camera_,
                                   (SoOrthographicCamera *)this->scene_camera_);
    camera_type_ = ORTHOGONAL;
  }
  // handle common case #2
  else if (t == SoPerspectiveCamera::getClassTypeId() &&
           s == SoOrthographicCamera::getClassTypeId()) {
    this->convertOrtho2Perspective((SoOrthographicCamera *)this->stored_camera_,
                                   (SoPerspectiveCamera *)this->scene_camera_);
    camera_type_ = PERSPECTIVE;
  }
  // otherwise, cameras have changed in ways we don't understand since
  // the last saveHomePosition() invokation, and so we're just going
  // to ignore the reset request
}

//-------------------------------------------------

//-------------Headlight Methods-------------------

//: Activate a headlight
void
bgui3d_viewer_tableau::set_headlight(bool enable)
{
  headlight_->on = enable;
}


//: Is the headlight active
bool
bgui3d_viewer_tableau::is_headlight() const
{
  return headlight_->on.getValue() != 0;
}


//: Return the headlight
SoDirectionalLight*
bgui3d_viewer_tableau::headlight() const
{
  return headlight_;
}

//-------------------------------------------------

//-------------Text2 Methods-------------------

static void setTextCallback( void *data, SoSensor *sensor )
{
  ((SoText2*)data)->string.deleteValues(0, 1);
}

void bgui3d_viewer_tableau::setText( const vcl_string& string )
{
  int numStrings = _text->string.getNum();
  _text->string.set1Value( numStrings, string.c_str() );
  SoAlarmSensor* alarm = new SoAlarmSensor( setTextCallback, _text );
  alarm->setTimeFromNow( 7.0 );
  alarm->schedule();
}

//---------------------------------------------------

//: convert camera to perspective
void
bgui3d_viewer_tableau::convertOrtho2Perspective(const SoOrthographicCamera * in,
                                                SoPerspectiveCamera * out)
{
  out->aspectRatio.setValue(in->aspectRatio.getValue());
  out->focalDistance.setValue(in->focalDistance.getValue());
  out->orientation.setValue(in->orientation.getValue());
  out->position.setValue(in->position.getValue());
  out->viewportMapping.setValue(in->viewportMapping.getValue());
  out->setName(in->getName());

  float focaldist = in->focalDistance.getValue();

  // focalDistance==0.0f happens for empty scenes.
  if (focaldist != 0.0f) {
    out->heightAngle = 2.0f * (float)vcl_atan(in->height.getValue() / 2.0 / focaldist);
  }
  else {
    // 45?is the default value of this field in SoPerspectiveCamera.
    out->heightAngle = (float)(M_PI / 4.0);
  }
}


//: convert camera to orthographic
void
bgui3d_viewer_tableau::convertPerspective2Ortho(const SoPerspectiveCamera * in,
                                                SoOrthographicCamera * out)
{
  out->aspectRatio.setValue(in->aspectRatio.getValue());
  out->focalDistance.setValue(in->focalDistance.getValue());
  out->orientation.setValue(in->orientation.getValue());
  out->position.setValue(in->position.getValue());
  out->viewportMapping.setValue(in->viewportMapping.getValue());
  out->setName(in->getName());

  float focaldist = in->focalDistance.getValue();

  out->height = 2.0f * focaldist * (float)vcl_tan(in->heightAngle.getValue() / 2.0);
}

void
bgui3d_viewer_tableau::set_clipping_planes()
{
  SoGetBoundingBoxAction action( get_viewport_region() );

  action.apply( scene_root_ );

  SbXfBox3f xbox = action.getXfBoundingBox();

  SbMatrix cammat;

  SoSearchAction searchaction;
  searchaction.reset();
  searchaction.setSearchingAll(TRUE);
  searchaction.setInterest(SoSearchAction::FIRST);
  searchaction.setNode(scene_camera_);
  searchaction.apply(scene_root_);

  SoGetMatrixAction matrixaction(get_viewport_region());
  SbMatrix inverse = SbMatrix::identity();
  if (searchaction.getPath()) {
    matrixaction.apply(searchaction.getPath());
    inverse = matrixaction.getInverse();
  }

  xbox.transform(inverse);

  SbMatrix mat;
  mat.setTranslate(- scene_camera_->position.getValue());
  xbox.transform(mat);
  mat = scene_camera_->orientation.getValue().inverse();
  xbox.transform(mat);
  SbBox3f box = xbox.project();

  // Bounding box was calculated in camera space, so we need to "flip"
  // the box (because camera is pointing in the (0,0,-1) direction
  // from origo.
  float nearval = -box.getMax()[2];
  float farval = -box.getMin()[2];

  // FIXME: what if we have a weird scale transform in the scenegraph?
  // Could we end up with nearval > farval then? Investigate, then
  // either use an assert() (if it can't happen) or an SoWinSwap()
  // (to handle it). 20020116 mortene.

  // Check if scene is completely behind us.
  if (farval <= 0.0f) { return; }

  if ( scene_camera_->isOfType(SoPerspectiveCamera::getClassTypeId())) {
    // Disallow negative and small near clipping plane distance.

    float nearlimit; // the smallest value allowed for nearval
    //if (this->autoclipstrategy == SoWinViewer::CONSTANT_NEAR_PLANE) {
      //nearlimit = this->autoclipvalue;
    //}
    //else {
      //assert(this->autoclipstrategy == SoWinViewer::VARIABLE_NEAR_PLANE);
      // From glFrustum() documentation: Depth-buffer precision is
      // affected by the values specified for znear and zfar. The
      // greater the ratio of zfar to znear is, the less effective the
      // depth buffer will be at distinguishing between surfaces that
      // are near each other. If r = far/near, roughly log (2) r bits
      // of depth buffer precision are lost. Because r approaches
      // infinity as znear approaches zero, you should never set znear
      // to zero.

      GLint depthbits[1];
      glGetIntegerv(GL_DEPTH_BITS, depthbits);

      int use_bits = (int) (float(depthbits[0]) * (0.4f));
      float r = (float) vcl_pow(2.0, (double) use_bits);
      nearlimit = farval / r;
    //}

    if (nearlimit >= farval) {
      // (The "5000" magic constant was found by fiddling around a bit
      // on an OpenGL implementation with a 16-bit depth-buffer
      // resolution, adjusting to find something that would work well
      // with both a very "stretched" / deep scene and a more compact
      // single-model one.)
      nearlimit = farval / 5000.0f;
    }

    // adjust the near plane if the the value is too small.
    if (nearval < nearlimit) {
      nearval = nearlimit;
    }

#if 0
    if (this->autoclipcb) {
      SbVec2f nearfar;
      nearfar[0] = nearval;
      nearfar[1] = farval;

      nearfar = this->autoclipcb(this->autoclipuserdata, nearfar);

      nearval = nearfar[0];
      farval = nearfar[1];
    }
#endif // 0
  }
  // Some slack around the bounding box, in case the scene fits
  // exactly inside it. This is done to minimize the chance of
  // artifacts caused by the limitation of the z-buffer
  // resolution. One common artifact if this is not done is that the
  // near clipping plane cuts into the corners of the model as it's
  // rotated.
  const float SLACK = 0.001f;

  // FrustumCamera can be found in the SmallChange CVS module. We
  // should not change the nearDistance for this camera, as this will
  // modify the frustum.
  if (scene_camera_->getTypeId().getName() == "FrustumCamera") {
    nearval = scene_camera_->nearDistance.getValue();
    farval *= (1.0f + SLACK);
    if (farval <= nearval) {
      // nothing is visible, so just set farval to som value > nearval.
      farval = nearval + 10.0f;
    }
    scene_camera_->farDistance = farval;
  }
  else {
    scene_camera_->nearDistance = nearval * (1.0f - SLACK);
    scene_camera_->farDistance = farval * (1.0f + SLACK);
  }
}


vcl_vector<SoGroup*>
bgui3d_viewer_tableau::get_parents_of_node(SoNode * node)
{
  SbBool oldsearch = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(TRUE);

  assert(node && "get_parent_of_node() called with null argument");

  SoSearchAction search;
  search.setSearchingAll(TRUE);
  search.setNode(node);
  search.setInterest(SoSearchAction::ALL);
  search.apply(this->scene_root());
  SoPathList & pl = search.getPaths();

  vcl_vector<SoGroup*> parents;
  for (int i = 0; i < pl.getLength(); ++i) {
    SoFullPath * p = (SoFullPath*) pl[i];
    if (p->getLength() > 0)
      parents.push_back((SoGroup*)p->getNodeFromTail(1));
  }
  SoBaseKit::setSearchingChildren(oldsearch);
  return parents;
}


vcl_vector<SoCamera*>
bgui3d_viewer_tableau::find_cameras(SoNode* root) const
{
  assert(camera_group_);
  SoSearchAction sa;

  // Search for existing cameras
  sa.setType(SoCamera::getClassTypeId());
  sa.setInterest(SoSearchAction::ALL);
  sa.setSearchingAll(TRUE);
  sa.apply(root);
  SoPathList & pl = sa.getPaths();

  vcl_vector<SoCamera*> cameras;
  for (int i = 0; i < pl.getLength(); ++i) {
    SoFullPath * p = (SoFullPath*) pl[i];
    if (p->getTail()->isOfType(SoCamera::getClassTypeId())) {
      SoCamera * camera = (SoCamera*) p->getTail();
      cameras.push_back(camera);
    }
  }
  sa.reset();

  return cameras;
}


//: Find the VRML viewpoint nodes in the scenegraph and make camera.
// The cameras are added to the camera group (outside the user scene)
void bgui3d_viewer_tableau::collect_vrml_cameras(SoNode* root) const
{
  assert(camera_group_);
  SoSearchAction sa;

  // Search for VRML viewpoints and create cameras
  sa.setType(SoVRMLViewpoint::getClassTypeId());
  sa.setInterest(SoSearchAction::ALL);
  sa.setSearchingAll(TRUE);
  sa.apply(root);
  SoPathList & pl2 = sa.getPaths();

  for (int i = 0; i < pl2.getLength(); ++i) {
    SoFullPath * p = (SoFullPath*) pl2[i];
    if (p->getTail()->isOfType(SoVRMLViewpoint::getClassTypeId())) {
      SoVRMLViewpoint * vp = (SoVRMLViewpoint*) p->getTail();
      SoPerspectiveCamera * camera = (SoPerspectiveCamera *)
          SoPerspectiveCamera::getClassTypeId().createInstance();
      camera->nearDistance = 0.5f;
      camera->farDistance = 5000.0f;
      camera->position = vp->position;
      camera->orientation = vp->orientation;
      camera->heightAngle = vp->fieldOfView;
      camera->setName(vp->description.getValue());
      camera_group_->addChild( camera );
    }
  }
  sa.reset();
}
