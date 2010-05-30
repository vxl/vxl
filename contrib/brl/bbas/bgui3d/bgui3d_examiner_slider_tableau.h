// This is brl/bbas/bgui3d/bgui3d_examiner_slider_tableau.h
#ifndef bgui3d_examiner_slide_tableau_h_
#define bgui3d_examiner_slide_tableau_h_
//:
// \file
// \brief  Basic tableau that wraps Coin3D into VGUI and adds a slider and functionality to examine the scene
//
//         It basically adds the slider to the bgui_3d_examiner_tableau
//         class. The slider keeps a minimum and maximum value which is
//         in the range of [0-255]. Any program using this class can
//         investigate the min-max value by calling minValue() and maxValue()
//         public methods. An example to investigate this values could be
//         setting a SoTimerSensor (Coin3D) and getting the updated values
//         at regular time intervals and doing the required changes/processing
//         based on those values.
//
// \author Gamze Tunali (gamze@lems.brown.edu)
// \date   December 01, 2005
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bgui3d_examiner_tableau.h"
#include "bgui3d_examiner_slider_tableau_sptr.h"

#include <Inventor/nodes/SoImage.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoCone.h>

class bgui3d_examiner_slider_tableau: public bgui3d_examiner_tableau
{
 public:
  //: Constructor
  bgui3d_examiner_slider_tableau(SoNode * scene_root = NULL);

  //: Destructor
  virtual ~bgui3d_examiner_slider_tableau() {}

  bool handle(const vgui_event& e);

  int minValue() const { return min; }
  int maxValue() const { return max; }

 protected:
  //: Render the scene graph (called on draw events)
  virtual bool render();

  void transfer_callback(const bool & remap, const int & n_min,
                         const int & n_max);

 private:
  void loadSliderImage();
  void positionSlider();

  SbVec2s last_viewport_sz_;
  int min, max;
  SbViewportRegion * slider_vp_;
  SoImage * slider_image_;
  SoOrthographicCamera* sliderCamera_;
  SoTransform *slider_transform, *min_transform, *max_transform;
  int slider_height_, slider_width_;
  SoCone *min_mark_, *max_mark_;
};

//: Create a smart pointer to a bgui3d_examiner_tableau.
struct bgui3d_examiner_slider_tableau_new : public bgui3d_examiner_slider_tableau_sptr
{
  typedef bgui3d_examiner_slider_tableau_sptr base;
  bgui3d_examiner_slider_tableau_new(SoNode* scene_root = NULL)
   : base(new bgui3d_examiner_slider_tableau(scene_root)) { }
};

#endif // bgui3d_examiner_slide_tableau_h_
