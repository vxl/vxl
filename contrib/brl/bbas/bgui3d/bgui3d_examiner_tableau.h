 // This is brl/bbas/bgui3d/bgui3d_examiner_tableau.h
#ifndef bgui3d_examiner_tableau_h_
#define bgui3d_examiner_tableau_h_
//:
// \file
// \brief  Examiner viewer tableau for 3D scenes
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   May 25, 2004
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bgui3d_fullviewer_tableau.h"
#include "bgui3d_examiner_tableau_sptr.h"
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoSwitch.h>

class SbSphereSheetProjector;
class SbRotation;
class SoTimerSensor;
class SoCone;

enum Process
{
  SPIN=0,
  DRAG,
  PAN,
  ZOOM,
  IDLE,
  SEEK
};

struct Log
{
  int size;
  SbVec2f pos1;
  SbVec2f pos2;
  SbVec2f pos3;
};

//:  Examiner viewer tableau for 3D scenes
class bgui3d_examiner_tableau : public bgui3d_fullviewer_tableau
{
 public:
  enum SeekDistance
  {
    SEEK_FAR,
    SEEK_HALF,
    SEEK_NEAR,
    SEEK_ZERO
  };

  //: Constructor
  bgui3d_examiner_tableau(SoNode * scene_root = NULL);

  //: Destructor
  virtual ~bgui3d_examiner_tableau();

  //: Handle vgui events
  virtual bool handle(const vgui_event& event);

  //: Return the type name of this tableau
  std::string type_name() const;

  //: Builds a popup menu
  virtual void get_popup(const vgui_popup_params&, vgui_menu &m);

  //: Render the scene graph (called on draw events)
  virtual bool render();

  //: Pans the camera
  void pan(const SbVec2f & currpos, const SbVec2f & prevpos, const float aspect_ratio);

  //: Zooms the camera
  void zoom( float aspectratio );

  //: Spins the camera around the scene
  void spin(const SbVec2f& currpos, const SbVec2f &prevpos);

  //: Reorients the camera to the specified rotation
  void reorient_camera(const SbRotation & rot);

  //: Updates position log
  void update_log(SbVec2f pos);

  //: Resets position log
  void reset_log();

  //: Seeks for the object under a mouse click and zooms to it
  void seek_to_point(SbVec2s pos);

  //: What to do if events are idle, if enabled
  bool idle();

  //: Set the visibility of the axis cross
  void set_axis_visible(bool state);

  //: Return true if the axis cross is visible
  bool axis_visible() const;

  void set_seek_distance( SeekDistance seek );

  //void seeksensorCB(void * data, SoSensor * s);

 protected:
  // these are made protected (instead of private) so that child classes
  // can access them
  vgui_button last_down_button_;
  SbVec2f last_pos_;
  Process last_process_;
  int last_timestamp_;

  //: draw the axis
  void draw_axis_cross();

  //: Draw an arrow for the axis representation
  void draw_arrow();

  //: Determine the scale of the scene
  void find_scale();

 private:
  SbSphereSheetProjector* spin_projector_;
  int spin_sample_counter_;
  SbRotation spin_increment_;
  Log log_;
  SeekDistance seek_distance_;
  SoTimerSensor* seekSensor_;

  // The scale of the scene
  float scale_;

  bool axis_visible_;

 public:
  SbRotation fromRot_;
  SbRotation toRot_;
  SbVec3f fromPos_;
  SbVec3f toPos_;
};


//: Create a smart pointer to a bgui3d_examiner_tableau.
struct bgui3d_examiner_tableau_new : public bgui3d_examiner_tableau_sptr
{
  typedef bgui3d_examiner_tableau_sptr base;
  bgui3d_examiner_tableau_new(SoNode* scene_root = NULL)
   : base(new bgui3d_examiner_tableau(scene_root)) {}
};


#endif // bgui3d_examiner_tableau_h_
