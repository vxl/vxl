#ifndef bwm_observer_mgr_h_
#define bwm_observer_mgr_h_

#include <vcl_vector.h>

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

  static bwm_observer_mgr* instance();

  virtual ~bwm_observer_mgr() {}

  static bwm_observer_cam* BWM_MASTER_OBSERVER;

  vcl_vector<bwm_observer_rat_cam*> observers_rat_cam();

  void add(bwm_observer* o){ observers_.push_back(o); }

  void remove(bwm_observer* observer);

  //: attach the observable to all observers
  void attach(bwm_observable_sptr obs);

  // Correspondence methods
  void collect_corr();
  void set_corr(bwm_corr_sptr corr);
  void save_world_pt_corr(vgl_point_3d<double> world_pt) 
  { if (corr_mode_ == WORLD_TO_IMAGE) corr_world_pt_ = world_pt;
  else vcl_cerr << "First set correspondence mode to World to Image!!" << vcl_endl;
  }
  bool obs_in_corr(bwm_observer_cam *obs);
  void save_corr(vcl_ostream& s);
  void update_corr(bwm_observer_cam* obs,vgl_point_2d<double> old_pt,vgl_point_2d<double> new_pt);
  void delete_last_corr();
  void delete_all_corr();

  BWM_CORR_MODE corr_mode() { return corr_mode_; }
  void set_corr_mode(BWM_CORR_MODE mode) 
  { if (mode > 1) vcl_cerr << "Invalid Corr. Mode" << vcl_endl; 
  else corr_mode_ = mode;
  }

  void print_observers();
  
private:
  bwm_observer_mgr() {corr_mode_ = IMAGE_TO_IMAGE;}

  static bwm_observer_mgr* instance_;
  
  vcl_vector<bwm_observer* > observers_;

  BWM_CORR_MODE corr_mode_;
  vcl_vector<bwm_corr_sptr> corr_list_;
  vgl_point_3d<double> corr_world_pt_;
};

#endif
