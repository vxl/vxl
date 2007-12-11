// This is brl/bbas/bgui3d/bgui3d_examiner_tableau.cxx
#include "bgui3d_examiner_tableau.h"
//:
// \file

#include <vcl_cassert.h>
#include <vcl_algorithm.h>

#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_gl.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/projectors/SbSphereSheetProjector.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>

#include <Inventor/SbLinear.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>


static void seeksensorCB(void * data, SoSensor * s);

// Bitmap representations of an "X", a "Y" and a "Z" for the axis cross.
static GLubyte xbmp[] = { 0x11,0x11,0x0a,0x04,0x0a,0x11,0x11 };
static GLubyte ybmp[] = { 0x04,0x04,0x04,0x04,0x0a,0x11,0x11 };
static GLubyte zbmp[] = { 0x1f,0x10,0x08,0x04,0x02,0x01,0x1f };

int count = 0;
SbVec3f d;

//: Constructor
bgui3d_examiner_tableau::bgui3d_examiner_tableau(SoNode * scene_root)
 : bgui3d_fullviewer_tableau(scene_root),
   last_down_button_(vgui_BUTTON_NULL), last_timestamp_(0), seek_distance_(SEEK_HALF),
   scale_(1.0f),
   axis_visible_(true)
{
   interaction_type_ = CAMERA;
   spin_projector_ = new SbSphereSheetProjector(SbSphere(SbVec3f(0, 0, 0), 0.8f));
   SbViewVolume volume;
   volume.ortho(-1, 1, -1, 1, -1, 1);
   spin_projector_->setViewVolume( volume );
   _seekSensor = new SoTimerSensor( seeksensorCB, this );
   find_scale();
}


//: Destructor
bgui3d_examiner_tableau::~bgui3d_examiner_tableau()
{
  //: Ming: no need to delete the SbSphereSheetProjector object
  ///delete spin_projector_;
}


vcl_string bgui3d_examiner_tableau::type_name() const {return "bgui3d_examiner_tableau";}

//: Handle the events coming in
//   Left mouse = spin
//   Middle mouse = pan
//   Middle mouse with control = zoom (dolly)
//   's' and then click = seeks 2/3 of the distance to the object
//   'i' toggles idle events
//   'h' puts camera to the current view
//   'h' with control sets the home view to the current view
//   'v' views the entire scene
//   'n' go to the next camera
//   'p' go the the previous camera
//   The cursors pan
//   The cursors with 'control' spin
//   The up and down cursors with shift zoom (dolly)


bool bgui3d_examiner_tableau::handle(const vgui_event& e)
{
  // to deal with multiple tableaux in a grid
  if ( e.type == vgui_LEAVE )
    reset_log();
  if ( e.type == vgui_ENTER )
    post_idle_request();

  // ALWAYS DO KEYPRESSES
  if ( e.type == vgui_KEY_PRESS )
  {
    //float aspect_ratio = get_viewport_region().getViewportAspectRatio();
    SbVec2f middle(0.5f, 0.5f);
    SbVec2f left(0.4f, 0.5f);
    SbVec2f right(0.6f, 0.5f);
    SbVec2f up(0.5f, 0.6f);
    SbVec2f down(0.5f, 0.4f);
    switch (e.key)
    {
     case 'v':
      find_scale();
      view_all();
      break;

     case 'h':
      if (e.modifier == vgui_CTRL)
        save_home_position();
      else
        reset_to_home_position();
      break;

     case 'n': {
      int next = camera_group_->whichChild.getValue()+1;
      if (next < camera_group_->getChildren()->getLength()){
        camera_group_->whichChild.setValue(next);
        set_camera((SoCamera*)(*camera_group_->getChildren())[next]);
      }
      break; }

     case 'p': {
      int prev = camera_group_->whichChild.getValue()-1;
      if (prev>=0){
        camera_group_->whichChild.setValue(prev);
        set_camera((SoCamera*)(*camera_group_->getChildren())[prev]);
      }
      break; }

     case vgui_CURSOR_LEFT:
      if (e.modifier == vgui_CTRL)
        spin(left, middle);
      else if (e.modifier == vgui_MODIFIER_NULL) // rotate left
      {
        SbRotation camrot = scene_camera_->orientation.getValue();
        SbVec3f upvec(0, 1, 0); // init to default up vector
        camrot.multVec(upvec, upvec);
        SbRotation rotation(upvec, 0.025f );
        scene_camera_->orientation = camrot*rotation;
      }
      break;

     case vgui_CURSOR_RIGHT:
      if (e.modifier == vgui_CTRL)
        spin(right, middle);
      else if (e.modifier == vgui_MODIFIER_NULL) // rotate right
      {
        SbRotation camrot = scene_camera_->orientation.getValue();
        SbVec3f upvec(0, 1, 0); // init to default up vector
        camrot.multVec(upvec, upvec);
        SbRotation rotation(upvec, -0.025f );
        scene_camera_->orientation = camrot*rotation;
      }
      break;

     case vgui_CURSOR_UP:
      if (e.modifier == vgui_CTRL)
        spin(up, middle);
      else if (e.modifier == vgui_MODIFIER_NULL) // move forward
      {
        SbRotation camrot = scene_camera_->orientation.getValue();
        SbVec3f lookat(0, 0, -1); // init to default view direction vector
        camrot.multVec(lookat, lookat);
        lookat *= scale_*0.025;
        SbVec3f pos = scene_camera_->position.getValue();
        pos += lookat;
        scene_camera_->position = pos;
        float foc = scene_camera_->focalDistance.getValue();
        foc -= lookat.length();
        scene_camera_->focalDistance = foc;
      }
      else if (e.modifier == vgui_SHIFT)
        zoom(0.025f*scale_);
      break;

     case vgui_CURSOR_DOWN:
      if (e.modifier == vgui_CTRL)
        spin(down, middle);
      else if (e.modifier == vgui_MODIFIER_NULL) // move backward
      {
        SbRotation camrot = scene_camera_->orientation.getValue();
        SbVec3f lookat(0, 0, -1); // init to default view direction vector
        camrot.multVec(lookat, lookat);
        SbVec3f pos = scene_camera_->position.getValue();
        pos -= lookat;
        scene_camera_->position = pos;
        float foc = scene_camera_->focalDistance.getValue();
        foc += lookat.length();
        scene_camera_->focalDistance = foc;
      }
      else if (e.modifier == vgui_SHIFT)
        zoom(-0.025f*scale_);
      break;
     default:
      break;
    }
  }

  // ONLY IF CAMERA INTERACTION MODE
  if ( interaction_type_ == CAMERA )
  {
    const SbVec2s viewport_size(get_viewport_region().getViewportSizePixels());
    const SbVec2s viewport_origin(get_viewport_region().getViewportOriginPixels());
    const SbVec2s curr_pos = SbVec2s(e.wx, e.wy) - viewport_origin;
    float aspect_ratio = get_viewport_region().getViewportAspectRatio();

    if ( e.type == vgui_KEY_PRESS && e.key == 's' )
    {
      last_process_ = SEEK;
    }
    // SEEK
    else if ( last_process_ == SEEK && e.type == vgui_MOUSE_DOWN )
    {
      reset_log();
      seek_to_point(curr_pos);
      last_process_ = IDLE;
    }
    // MOUSE DOWN
    else if ( e.type == vgui_MOUSE_DOWN )
    {
      reset_log();
      last_down_button_ = e.button;
      if ( e.modifier == vgui_CTRL)
      {
        interaction_type_ = SCENEGRAPH;
        bool b = bgui3d_fullviewer_tableau::handle(e);
        interaction_type_ = CAMERA;
        return b;
      }
    }

    // MOUSE UP
    else if ( e.type == vgui_MOUSE_UP )
    {
      last_down_button_ = vgui_BUTTON_NULL;
      last_process_ = IDLE;
      if ( e.timestamp - last_timestamp_ > 100 )
        reset_log();

      if ( e.modifier == vgui_CTRL)
      {
        interaction_type_ = SCENEGRAPH;
        bool b = bgui3d_fullviewer_tableau::handle(e);
        interaction_type_ = CAMERA;
        return b;
      }
    }

    // MOUSE MOTION
    else if ( e.type == vgui_MOUSE_MOTION )
    {
      if ( e.modifier == vgui_CTRL && last_down_button_ == vgui_LEFT)
      {
        interaction_type_ = SCENEGRAPH;
        ///bool b =
        bgui3d_fullviewer_tableau::handle(e);
        interaction_type_ = CAMERA;
        //return b;
      }
      const SbVec2f last_pos_norm = last_pos_;
      const SbVec2s curr_pos = SbVec2s(e.wx, e.wy) - viewport_origin;
      const SbVec2f curr_pos_norm((float) curr_pos[0] / (float) vcl_max((int)(viewport_size[0] - 1), 1),
                                (float) curr_pos[1] / (float) vcl_max((int)(viewport_size[1] - 1), 1));

      last_pos_ = curr_pos_norm;

      // MOUSE DOWN HANDLING
      switch (last_down_button_)
      {
       case vgui_LEFT:
        if ( e.modifier != vgui_CTRL )
        {
          spin(curr_pos_norm, last_pos_norm);
          update_log( curr_pos_norm );

          last_process_ = DRAG;
        }
        break;

       case vgui_MIDDLE:
        if (e.modifier == vgui_CTRL)
        {
          zoom( (curr_pos_norm[1] - last_pos_norm[1]) * scale_);
          last_process_ = ZOOM;
        }
        else
        {
          pan(curr_pos_norm, last_pos_norm, aspect_ratio);
          last_process_ = PAN;
        }
        break;

       default:
        break;
      }

      last_timestamp_ = e.timestamp;
    }
    if ( idle_enabled_ )
    {
      idle();
    }
  }
  set_clipping_planes();
  return bgui3d_fullviewer_tableau::handle(e);
}


//: Determine the scale of the scene
void bgui3d_examiner_tableau::find_scale()
{
  SoGetBoundingBoxAction action( get_viewport_region() );
  action.apply( scene_root_ );
  SbBox3f box = action.getBoundingBox();

  float dx, dy, dz;
  box.getSize(dx,dy,dz);

  scale_ = dx;
  if (dy > scale_) scale_ = dy;
  if (dz > scale_) scale_ = dz;
}


//: When idle, spin the scene based on the log
bool bgui3d_examiner_tableau::idle()
{
  if ( idle_enabled_ && last_process_ == IDLE && log_.size >2  )
  {
    SbVec2f p = log_.pos3 + (log_.pos1 - log_.pos3)/5.0;
    spin(p, log_.pos3);
    request_render();
  }
  return bgui3d_fullviewer_tableau::idle();
}

//----------------------------------------------------------------------------
//: A vgui command used to toggle axis visibility
class bgui3d_axis_visible_command : public vgui_command
{
 public:
  bgui3d_axis_visible_command(bgui3d_examiner_tableau* tab)
   : bgui3d_examiner_tab(tab) {}
  void execute()
  {
    bool visible = bgui3d_examiner_tab->axis_visible();
    bgui3d_examiner_tab->set_axis_visible(!visible);
  }

  bgui3d_examiner_tableau *bgui3d_examiner_tab;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//: A vgui command used to toggle interaction type
class bgui3d_seek_distance_command : public vgui_command
{
 public:
  bgui3d_seek_distance_command(bgui3d_examiner_tableau* tab,
                               bgui3d_examiner_tableau::SeekDistance seek)
                              : bgui3d_examiner_tab(tab), seek_distance(seek) {}
  void execute()
  {
    bgui3d_examiner_tab->set_seek_distance(seek_distance);
  }

  bgui3d_examiner_tableau *bgui3d_examiner_tab;
  bgui3d_examiner_tableau::SeekDistance seek_distance;
};

//: Builds a popup menu
void bgui3d_examiner_tableau::get_popup(const vgui_popup_params& params,
                                            vgui_menu &menu)
{
  bgui3d_fullviewer_tableau::get_popup(params, menu);

  vcl_string axis_item;
  if ( this->axis_visible() )
    axis_item = "Disable Axis";
  else
    axis_item = "Enable Axis";

  menu.add(axis_item, new bgui3d_axis_visible_command(this));

  vcl_string check_on = "[x]";
  vcl_string check_off = "[ ]";

  vgui_menu seek_menu;
  SeekDistance seek = seek_distance_;

  seek_menu.add(((seek==SEEK_FAR)?check_on:check_off) + " 3/4 ",
              new bgui3d_seek_distance_command(this,SEEK_FAR));
  seek_menu.add(((seek==SEEK_HALF)?check_on:check_off) + " 1/2",
                  new bgui3d_seek_distance_command(this,SEEK_HALF));
  seek_menu.add(((seek==SEEK_NEAR)?check_on:check_off) + " 1/4",
                  new bgui3d_seek_distance_command(this,SEEK_NEAR));
  seek_menu.add(((seek==SEEK_ZERO)?check_on:check_off) + " 0 (focus)",
                  new bgui3d_seek_distance_command(this,SEEK_ZERO));

  menu.add( "Seek Distance", seek_menu );
}

//: Pan the camera
void bgui3d_examiner_tableau::pan(const SbVec2f& currpos, const SbVec2f &prevpos, const float aspect_ratio)
{
  if (scene_camera_ == NULL) return; // can happen for empty scenegraph

  if (currpos == prevpos) return; // useless invocation

  SbViewVolume vv = scene_camera_->getViewVolume(aspect_ratio);
  SbPlane panningplane = vv.getPlane(scene_camera_->focalDistance.getValue());

  vv = scene_camera_->getViewVolume(aspect_ratio);
  SbLine line;
  vv.projectPointToLine(currpos, line);
  SbVec3f current_planept;
  panningplane.intersect(line, current_planept);
  vv.projectPointToLine(prevpos, line);
  SbVec3f old_planept;
  panningplane.intersect(line, old_planept);

  scene_camera_->position = scene_camera_->position.getValue() - (current_planept - old_planept);
}


//: Zoom (actually dolly) the camera
void bgui3d_examiner_tableau::zoom(float diffvalue)
{
 if (scene_camera_ == NULL) return; // can happen for empty scenegraph

  SbVec3f direction;
  scene_camera_->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
  scene_camera_->position = diffvalue*direction + scene_camera_->position.getValue();
  scene_camera_->focalDistance = -diffvalue*direction.length() + scene_camera_->focalDistance.getValue();
}

//: Spin the scene based on the current mouse position and the last mouse position
void bgui3d_examiner_tableau::spin(const SbVec2f& currpos, const SbVec2f &prevpos)
{
  spin_projector_->project(prevpos);
  SbRotation r;
  spin_projector_->projectAndGetRotation(currpos, r);
  r.invert();
  reorient_camera(r);
}

//: Reorient the camera based on specified rotation
void bgui3d_examiner_tableau::reorient_camera(const SbRotation & rot)
{
  SoCamera * cam = scene_camera_;
  if (cam == NULL) return;

  // Find global coordinates of focal point.
  SbVec3f direction;
  cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
  SbVec3f focalpoint = cam->position.getValue() +
          cam->focalDistance.getValue() * direction;

  // Set new orientation value by accumulating the new rotation.
  cam->orientation = rot * cam->orientation.getValue();
  SbVec3f lookat(0, 0, -1); // init to default view direction vector

  cam->orientation.getValue().multVec(lookat, lookat);

  // Reposition camera so we are still pointing at the same old focal point.
  cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
  SbVec3f distance = cam->focalDistance.getValue() * direction;

  cam->position = focalpoint - distance;
}


//: Update the log so that it can keep track of where the mouse has been
void bgui3d_examiner_tableau::update_log(SbVec2f pos)
{
  log_.size++;
  log_.pos3 = log_.pos2;
  log_.pos2 = log_.pos1;
  log_.pos1 = pos;
}

//: Reset the log
void bgui3d_examiner_tableau::reset_log()
{
  log_.size = 0;
}


static void
seeksensorCB(void * data, SoSensor * s)
{
  SbTime currenttime = SbTime::getTimeOfDay();

  bgui3d_examiner_tableau * thisp = (bgui3d_examiner_tableau *)data;
  SoTimerSensor * sensor = (SoTimerSensor *)s;

  float t = float((currenttime - sensor->getBaseTime()).getValue()) / 1.0f;
  if ((t > 1.0f) || (t + sensor->getInterval().getValue() > 1.0f)) t = 1.0f;
  SbBool end = (t == 1.0f);

  t = (float) ((1.0 - vcl_cos(M_PI*t)) * 0.5);

  thisp->camera_node()->position = thisp->_fromPos + (thisp->_toPos - thisp->_fromPos) * t;
  thisp->camera_node()->orientation = SbRotation::slerp( thisp->_fromRot, thisp->_toRot, t);
  if ( end )
    s->unschedule();
}

// Seek to a specified point on the screen
void bgui3d_examiner_tableau::seek_to_point( SbVec2s pos )
{
  if (! scene_camera_)
    return;

  // SoRayPickAction needs the the viewport to have origin (0,0)
  SbViewportRegion v = get_viewport_region();
  v.setViewportPixels( SbVec2s(0,0), v.getViewportSizePixels() );

  SoRayPickAction rpaction( v );
  rpaction.setPoint( pos );
  rpaction.setRadius( 1 );
  rpaction.apply( user_scene_root_ );

  SoPickedPoint * picked = rpaction.getPickedPoint();
  if (!picked)
    return;
  float factor;
  switch (seek_distance_)
  {
   case SEEK_FAR:
    factor = 0.75f;
    break;
   case SEEK_HALF:
    factor = 0.5f;
    break;
   case SEEK_NEAR:
   default:
    factor = 0.25f;
    break;
   case SEEK_ZERO:
    factor = 0.0f;
    break;
  }

  SbVec3f hitpoint = picked->getPoint();
  SbVec3f cameraposition = scene_camera_->position.getValue();
  SbVec3f diff = hitpoint - cameraposition;
  _fromPos = cameraposition;
  _toPos = cameraposition += factor*diff;

  SbRotation camrot = scene_camera_->orientation.getValue();
  SbVec3f lookat(0, 0, -1); // init to default view direction vector
  camrot.multVec(lookat, lookat);
  SbRotation rot(lookat, diff);

   _fromRot = camrot;
  _toRot = camrot*rot ;

  scene_camera_->focalDistance = diff.length()*(1.0-factor);
  _seekSensor->setBaseTime( SbTime::getTimeOfDay() );
  _seekSensor->schedule();
}


//: Set the visibility of the axis cross
void
bgui3d_examiner_tableau::set_axis_visible(bool state)
{
  axis_visible_ = state;
}


//: Return true if the axis cross is visible
bool
bgui3d_examiner_tableau::axis_visible() const
{
  return axis_visible_;
}


//: Changes the distance the viewer goes when seeking
void
bgui3d_examiner_tableau::set_seek_distance( SeekDistance seek )
{
  seek_distance_ = seek;
}


//: Render the scene graph (called on draw events)
bool
bgui3d_examiner_tableau::render()
{
  // call the super class method
  bool result = bgui3d_fullviewer_tableau::render();
  if (!result)
    return false;

  if (axis_visible_)
    this->draw_axis_cross();

  return true;
}


void
bgui3d_examiner_tableau::draw_axis_cross()
{
  // FIXME: convert this to a superimposition scenegraph instead of
  // OpenGL calls. 20020603 mortene.

  // Store GL state.
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  GLfloat depthrange[2];
  glGetFloatv(GL_DEPTH_RANGE, depthrange);
  GLdouble projectionmatrix[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projectionmatrix);

  glDepthFunc(GL_ALWAYS);
  glDepthMask(GL_TRUE);
  glDepthRange(0, 0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_BLEND); // Kills transparency.

  // Set the viewport in the OpenGL canvas. Dimensions are calculated
  // as a percentage of the total canvas size.
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  const int view_x = vp[2];
  const int view_y = vp[3];
  const int pixelarea = int(0.25f * vcl_min(view_x, view_y));
  // lower right of canvas
  glViewport(vp[0]+vp[2] - pixelarea, vp[1], pixelarea, pixelarea);

  // Set up the projection matrix.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  const float NEARVAL = 0.1f;
  const float FARVAL = 10.0f;
  const float dim = NEARVAL * float(vcl_tan(M_PI / 8.0)); // FOV is 45 (45/360 = 1/8)
  glFrustum(-dim, dim, -dim, dim, NEARVAL, FARVAL);

  // Set up the model matrix.
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  SbMatrix mx;
  SoCamera * cam = this->camera_node();

  // If there is no camera (like for an empty scene, for instance),
  // just use an identity rotation.
  if (cam) { mx = cam->orientation.getValue(); }
  else { mx = SbMatrix::identity(); }

  mx = mx.inverse();
  mx[3][2] = -3.5; // Translate away from the projection point (along z axis).
  glLoadMatrixf((float *)mx);

  // Find unit vector end points.
  SbMatrix px;
  glGetFloatv(GL_PROJECTION_MATRIX, (float *)px);
  SbMatrix comb = mx.multRight(px);

  SbVec3f xpos;
  comb.multVecMatrix(SbVec3f(1,0,0), xpos);
  xpos[0] = (1 + xpos[0]) * view_x/2;
  xpos[1] = (1 + xpos[1]) * view_y/2;
  SbVec3f ypos;
  comb.multVecMatrix(SbVec3f(0,1,0), ypos);
  ypos[0] = (1 + ypos[0]) * view_x/2;
  ypos[1] = (1 + ypos[1]) * view_y/2;
  SbVec3f zpos;
  comb.multVecMatrix(SbVec3f(0,0,1), zpos);
  zpos[0] = (1 + zpos[0]) * view_x/2;
  zpos[1] = (1 + zpos[1]) * view_y/2;

  // Render the cross.
  {
    glLineWidth(2.0);

    enum { XAXIS, YAXIS, ZAXIS };
    int idx[3] = { XAXIS, YAXIS, ZAXIS };
    float val[3] = { xpos[2], ypos[2], zpos[2] };

    // Bubble sort.. :-}
    if (val[0] < val[1]) { vcl_swap(val[0], val[1]); vcl_swap(idx[0], idx[1]); }
    if (val[1] < val[2]) { vcl_swap(val[1], val[2]); vcl_swap(idx[1], idx[2]); }
    if (val[0] < val[1]) { vcl_swap(val[0], val[1]); vcl_swap(idx[0], idx[1]); }
    assert((val[0] >= val[1]) && (val[1] >= val[2])); // Just checking..

    for (int i=0; i < 3; i++) {
      glPushMatrix();
      if (idx[i] == XAXIS) {                       // X axis.
        glColor3f(0.500f, 0.125f, 0.125f);
      } else if (idx[i] == YAXIS) {                // Y axis.
        glRotatef(90, 0, 0, 1);
        glColor3f(0.125f, 0.500f, 0.125f);
      } else {                                     // Z axis.
        glRotatef(-90, 0, 1, 0);
        glColor3f(0.125f, 0.125f, 0.500f);
      }
      this->draw_arrow();
      glPopMatrix();
    }
  }

  // Render axis notation letters ("X", "Y", "Z").
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, view_x, 0, view_y, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLint unpack;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glColor3fv(SbVec3f(0.8f, 0.8f, 0.0f).getValue());

  glRasterPos2d(xpos[0], xpos[1]);
  glBitmap(8, 7, 0, 0, 0, 0, xbmp);
  glRasterPos2d(ypos[0], ypos[1]);
  glBitmap(8, 7, 0, 0, 0, 0, ybmp);
  glRasterPos2d(zpos[0], zpos[1]);
  glBitmap(8, 7, 0, 0, 0, 0, zbmp);

  glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);
  glPopMatrix();

  // Reset original state.

  // FIXME: are these 3 lines really necessary, as we push
  // GL_ALL_ATTRIB_BITS at the start? 20000604 mortene.
  glDepthRange(depthrange[0], depthrange[1]);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(projectionmatrix);

  glPopAttrib();
}

//: Draw an arrow for the axis representation directly through OpenGL.
void
bgui3d_examiner_tableau::draw_arrow()
{
  glBegin(GL_LINES);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glEnd();
  glDisable(GL_CULL_FACE);
  glBegin(GL_TRIANGLES);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, +0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, -0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, +0.5f / 4.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, -0.5f / 4.0f);
  glEnd();
  glBegin(GL_QUADS);
  glVertex3f(1.0f - 1.0f / 3.0f, +0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, +0.5f / 4.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, -0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, -0.5f / 4.0f);
  glEnd();
}

