#ifndef bwm_observer_mgr_h_
#define bwm_observer_mgr_h_
//:
// \file

#include <vcl_vector.h>
#include <vcl_iosfwd.h>

#include "bwm_observer.h"
#include "bwm_observer_cam.h"
#include "bwm_observable.h"
#include "bwm_observer_rat_cam.h"
#include "bwm_corr.h"
#include "bwm_corr_sptr.h"

class bwm_observer_mgr
{
 public:
  typedef enum {IMAGE_TO_IMAGE, WORLD_TO_IMAGE} BWM_CORR_MODE;
  typedef enum {SINGLE_PT_CORR, MULTIPLE_CORRS} BWM_N_CORRS;

  static bwm_observer_mgr* instance();

  virtual ~bwm_observer_mgr() {}

  static bwm_observer_cam* BWM_MASTER_OBSERVER;

  static bwm_observer_cam* BWM_EO_OBSERVER;

  static bwm_observer_cam* BWM_OTHER_MODE_OBSERVER;

  //: cleans up the correspondences and initializes the system for a new site
  void clear();

  vcl_vector<bwm_observer_cam*> observers_cam();
  vcl_vector<bwm_observer_rat_cam*> observers_rat_cam();
  

  void add(bwm_observer* o);

  void remove(bwm_observer* observer);

  //: attach the observable to all observers
  void attach(bwm_observable_sptr obs);

  bool comp_avg_camera_center(vgl_point_3d<double> &cam_center);

  // Correspondence methods
  void collect_corr();
  void set_corr(bwm_corr_sptr corr);
  bool obs_in_corr(bwm_observer_cam *obs);
  //implement me!
  bool match(bwm_observer_cam *obs, vgl_point_2d<double> pt);
  void save_corr(vcl_ostream& s);
  void save_corr_XML();
  void update_corr(bwm_observer_cam* obs,vgl_point_2d<double> old_pt,vgl_point_2d<double> new_pt);
  void delete_last_corr();
  void delete_all_corr();
  BWM_CORR_MODE corr_mode() { return corr_mode_; }
  BWM_N_CORRS n_corrs() { return n_corrs_; }
  void set_corr_mode();
  void set_corr_mode(BWM_CORR_MODE mode){corr_mode_ = mode;}
  void set_n_corrs(BWM_N_CORRS n){n_corrs_ = n;}
  void move_to_corr();

  //: picking up corr points are controlled by starting and stopping it
  void start_corr() { start_corr_ = true; }
  void stop_corr() { start_corr_ = false; }
  //: returns true if the correspondence picking started by the main corr menu
  bool in_corr_picking() { return start_corr_; }

  void print_observers();

  vcl_vector<bwm_corr_sptr> correspondences()
    {return corr_list_;}

  //: Given a set of image-to-image correpondences, solve for the 3-d world point and adjust the cameras
  void adjust_camera_offsets();

 private:
  bwm_observer_mgr() : start_corr_(false)
  {
    corr_mode_ = IMAGE_TO_IMAGE;
    n_corrs_ = SINGLE_PT_CORR;
  }

  static bwm_observer_mgr* instance_;

  vcl_vector<bwm_observer* > observers_;

  bool start_corr_;
  BWM_CORR_MODE corr_mode_;
  BWM_N_CORRS n_corrs_;
  vcl_vector<bwm_corr_sptr> corr_list_;
#if 0
  bool world_point_valid_;
  vgl_point_3d<double> corr_world_pt_;
#endif
};

#endif
