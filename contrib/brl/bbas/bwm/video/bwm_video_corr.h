#ifndef bwm_video_corr_h_
#define bwm_video_corr_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A subclass of bwm_corr to handle video sequences
//
// \verbatim
//  Modifications
//   Original December 26, 2007
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include "dll.h"
class bwm_observer_video;

class bwm_video_corr : public vbl_ref_count
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  //: Constructor - default
  bwm_video_corr(): observer_(0), world_pt_valid_(false), id_(unique_id_++){}

  //: Constructor - from data
  bwm_video_corr(bwm_observer_video* obs, unsigned frame_index,
                 vgl_point_2d<double> const& pt): observer_(obs),
    world_pt_valid_(false), id_(unique_id_++)
    {matches_[frame_index]=pt;}


  //: Constructor - don't copy the ref count
  bwm_video_corr(bwm_video_corr const& c):vbl_ref_count(){};

  //: Destructor
  ~bwm_video_corr() {}

  //: Accessors 
  //: the video observer generating the correspondences 
  void set_observer(bwm_observer_video* obs){observer_ = obs;}
  bwm_observer_video* observer(){return observer_;}

  //: get the matching image location in a given frame
  bool match(unsigned frame, vgl_point_2d<double> &pt);

  //: is there a match for the specified frame
  bool match(unsigned frame);

  //: add a match
  bool add(unsigned frame, vgl_point_2d<double> const& pt);
  //: remove a match 
 void remove(unsigned frame)
   { matches_.erase(frame);}

  //: Number of matches assigned, i.e. number of matched video frames.
 unsigned num_matches()
   { return matches_.size();}

 unsigned id(){return id_;}

  //: the lowest frame number for which there is a match
 unsigned min_frame();

  //: the highest frame number for which there is a match
 unsigned max_frame();

 //: the frame closest to the specified frame where there is a match
 bool nearest_frame(unsigned frame, unsigned& near_frame);

 //: methods that apply if a 3-d world point is defined
 //: access
 void set_world_pt(vgl_point_3d<double> const& world_pt)
   {world_pt_ = world_pt; world_pt_valid_ = true;}

  vgl_point_3d<double> world_pt() const { return world_pt_;}
                    
  bool world_pt_valid() {return world_pt_valid_;}

  //: Write the correspondence as xml
  void x_write(vcl_ostream &os);
 protected:

  // INTERNALS-----------------------------------------------------------------
 unsigned id_;

  // Data Members--------------------------------------------------------------
 private:
 BWM_VIDEO_DLL_DATA static unsigned unique_id_;
  bwm_observer_video* observer_;
  vcl_map<unsigned, vgl_point_2d<double> > matches_;//match in each frame
  bool world_pt_valid_;
  vgl_point_3d<double> world_pt_;
};
#include <bwm/video/bwm_video_corr_sptr.h>
#endif
