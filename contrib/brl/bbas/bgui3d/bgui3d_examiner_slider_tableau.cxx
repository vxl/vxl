// This is brl/bbas/bgui3d/bgui3d_examiner_slider_tableau.cxx
#include <iostream>
#include <algorithm>
#include "bgui3d_examiner_slider_tableau.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h> // for vnl_math::pi_over_2

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/nodes/SoOrthographicCamera.h>


bgui3d_examiner_slider_tableau::bgui3d_examiner_slider_tableau(SoNode * scene_root)
    : bgui3d_examiner_tableau(scene_root), last_viewport_sz_(-1, -1)
{
   interaction_type_ = CAMERA;

   // COLOR BAR AND SLIDERS ROOT
   SoSeparator *slider_root_ = new SoSeparator;
   sliderCamera_ = new SoOrthographicCamera;
   sliderCamera_->position.setValue(SbVec3f(0.0, 0.0, 10.0));
   sliderCamera_->nearDistance = 8.0;
   sliderCamera_->farDistance = 12.0;
   sliderCamera_->focalDistance = 10.0;
   sliderCamera_->height.setValue(1.0) ;

   slider_transform = new SoTransform;
   SoDirectionalLight* slider_light = new SoDirectionalLight;
   slider_light->direction.setValue(SbVec3f(0., 0., 1.0));
   slider_light->color.setValue(SbVec3f(1.0, 1.0, 1.0));

   // COLOR BAR
   slider_height_ = 310;
   slider_width_ = 25;
   slider_image_ = new SoImage;
   slider_image_->vertAlignment = SoImage::TOP;
   slider_image_->horAlignment = SoImage::LEFT;
   slider_image_->height = slider_height_;
   slider_image_->width = slider_width_;

   ((SoGroup*)(scene_root_))->addChild(slider_root_);
   slider_root_->addChild(sliderCamera_);
   slider_root_->addChild(slider_light);
   slider_root_->addChild(slider_transform);
   slider_root_->addChild(slider_image_);

   // MIN SLIDER
   this->min = 0;
   SoSeparator *min_root = new SoSeparator;
   min_transform = new SoTransform;
   min_transform->rotation.setValue(SbVec3f(0.f, 0.f, 1.f), float(vnl_math::pi_over_2));
   min_transform->translation.setValue(SbVec3f(0.1f, 0.f, 0.f));
   min_mark_ = new SoCone;
   min_mark_ ->bottomRadius.setValue(0.05f);
   min_mark_ ->height.setValue(0.1f);

   ((SoGroup*)(slider_root_))->addChild(min_root);
   min_root->addChild(min_transform);
   min_root->addChild(min_mark_ );

   // MAX SLIDER
   this->max = 255;
   SoSeparator *max_root = new SoSeparator;
   max_transform = new SoTransform;
   max_transform->rotation.setValue(SbVec3f(0, 0, 1), float(vnl_math::pi_over_2));
   max_transform->translation.setValue(SbVec3f(0.1f, 0.f, 0.f));
   max_mark_ = new SoCone;
   max_mark_ ->bottomRadius.setValue(0.05f);
   max_mark_ ->height.setValue(0.1f);

   ((SoGroup*)(slider_root_))->addChild(max_root);
   max_root->addChild(max_transform);
   max_root->addChild(max_mark_ );

   // POSITION COLOR BAR AND LOAD PIXEL VALUES
   loadSliderImage();
}

//: Populate the colormap array and set the image
void bgui3d_examiner_slider_tableau::loadSliderImage()
{
    float slider_offset = 255.0f/slider_height_;
    unsigned char * img = new unsigned char[slider_width_*slider_height_*4];
    int k=0;

    float curr_offset = 0;
    for (int j=0; j < slider_height_; j++) {
        k = (int)curr_offset;
        for (int i=0; i<slider_width_*4; i+=4) {
            img[j*slider_width_*4+i] =
            img[j*slider_width_*4+i+1] =
            img[j*slider_width_*4+i+2] =
            img[j*slider_width_*4+i+3] = (unsigned char)(255 - k);
        }
        curr_offset += slider_offset;
    }
    slider_image_->image.setValue(SbVec2s(slider_width_,slider_height_), 4, img);
}

//: Position the color map bar in the top left corner of screen and scale it screen height.
void
bgui3d_examiner_slider_tableau::positionSlider()
{
#if 1
   // set up the view volume of the color map camera
   SbViewportRegion v = get_viewport_region();
   SbVec2s viewport = v.getViewportSizePixels();

   if (viewport != last_viewport_sz_)
   {
     float aspect = float(viewport[0]) / float(viewport[1]);
     float factor = 300.f / float(viewport[1]);
     float wsx =1, wsy = 1;
     if ( aspect > 1.0f )
       wsx *= aspect;
     else {
       wsy /= aspect;
       factor /= aspect;
     }

     slider_transform->translation.setValue(SbVec3f(-0.5f * wsx, 0.5f*wsy, .0f));

     float wslider_width = wsx * slider_width_ / float( viewport[0]);
     // unused variable: float wslider_height = wsy * slider_height_ / float(viewport[1]);

     float min_height = wsy * slider_height_ / viewport[1] * this->min / 255;
     min_transform->translation.setValue(SbVec3f(wslider_width, -min_height, 0.f));
     min_transform->scaleFactor.setValue( SbVec3f(factor, factor, 1.0f));

     float max_height = wsy * slider_height_ / float( viewport[1]) * this->max/255.0f;
     max_transform->translation.setValue(SbVec3f(wslider_width, -max_height, 0.f));
     max_transform->scaleFactor.setValue( SbVec3f(factor, factor, 1.0f));
    }
#endif // 1
}

//: Render the scene graph (called on draw events)
bool
bgui3d_examiner_slider_tableau::render()
{
  positionSlider();

  // call the super class method
  return bgui3d_examiner_tableau::render();
}

//: it only handles the slider events and leaves the rest to the parent class
bool
bgui3d_examiner_slider_tableau::handle(const vgui_event& e)
{
  // MOUSE MOTION
  if ( e.type == vgui_MOUSE_MOTION )
  {
    const SbVec2s viewport_size(get_viewport_region().getViewportSizePixels());
    const SbVec2s viewport_origin(get_viewport_region().getViewportOriginPixels());
    const SbVec2s curr_pos = SbVec2s(e.wx, e.wy) - viewport_origin;
    const SbVec2f curr_pos_norm((float) curr_pos[0] / (float) std::max((int)(viewport_size[0] - 1), 1),
                              (float) curr_pos[1] / (float) std::max((int)(viewport_size[1] - 1), 1));
    const SbVec2f last_pos_norm = last_pos_;
    // unused variable: float aspect_ratio = get_viewport_region().getViewportAspectRatio();

    // MOUSE DOWN HANDLING
    // unused variable: float factor = slider_height_/255.f;
    switch (last_down_button_)
    {
      case vgui_LEFT:
        if (e.wx >= 0 && e.wx <= slider_width_*2) { // WON'T ROTATE WHEN YOUR MOUSE IS DOWN ON THE LEFT COLOR MAP REGION
          int mouse_pos = static_cast<int>((viewport_size[1] - e.wy)*255.0/slider_height_);
          if (mouse_pos >= this->min-10 && mouse_pos <= this->min + 10) {
            transfer_callback(true, mouse_pos, this->max);
            return true;
          }
          else if (mouse_pos >= this->max -10 && mouse_pos <= this->max + 10)
          {
            transfer_callback(true, this->min, mouse_pos);
            return true;
          }
        }
        else if ( e.modifier != vgui_CTRL ) {
          spin(curr_pos_norm, last_pos_norm);
          update_log( curr_pos_norm );
          last_process_ = DRAG;
        }
        break;
      default:
        break;
    }

    last_timestamp_ = e.timestamp;
  }

  set_clipping_planes();
  return bgui3d_examiner_tableau::handle(e);
}

//: Called by biov_transfer_function if a change is made
void
bgui3d_examiner_slider_tableau::transfer_callback(const bool & remap, const int & n_min,
                                                  const int & n_max)
{
  if (remap && n_min<=n_max && n_min>=0 && n_max <=255) {
    this->min = n_min;
    this->max = n_max;

    if (this->min > this->max)
    {
      int tmp = this->min;
      this->min = this->max;
      this->max = tmp;
    }

    loadSliderImage();
  }
}
