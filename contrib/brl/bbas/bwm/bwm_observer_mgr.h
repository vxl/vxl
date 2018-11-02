#ifndef bwm_observer_mgr_h_
#define bwm_observer_mgr_h_
//:
// \file

#include <vector>
#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_fwd.h>

#include "bwm_observer.h"
#include "bwm_observer_cam.h"
#include "bwm_observable.h"
#include "bwm_observable_sptr.h"
#include "bwm_observer_rat_cam.h"
#include "bwm_corr.h"
#include "bwm_corr_sptr.h"
#include "bwm_3d_corr_sptr.h"


class bwm_observer_mgr
{
 public:
  typedef enum {IMAGE_TO_IMAGE, WORLD_TO_IMAGE, WORLD_TO_WORLD} BWM_CORR_MODE;
  typedef enum {SINGLE_PT_CORR, MULTIPLE_CORRS} BWM_N_CORRS;
  typedef enum {FEATURE_CORR, TERRAIN_CORR} BWM_CORR_TYPE;

  static bwm_observer_mgr* instance();

  virtual ~bwm_observer_mgr() {}

  static bwm_observer_cam* BWM_MASTER_OBSERVER;

  static bwm_observer_cam* BWM_EO_OBSERVER;

  static bwm_observer_cam* BWM_OTHER_MODE_OBSERVER;

  //: cleans up the correspondences and initializes the system for a new site
  void clear();

  std::vector<bwm_observer_cam*> observers_cam() const;
  std::vector<bwm_observer_rat_cam*> observers_rat_cam() const;

  void add(bwm_observer* o);

  void remove(bwm_observer* observer);

  //: attach the observable to all observers
  void attach(bwm_observable_sptr obs);

  //: detach the observable from all observers
  void detach(bwm_observable_sptr obs);

  bool comp_avg_camera_center(vgl_point_3d<double> &cam_center);

  // Correspondence methods
  void collect_corr();
  void set_corr(bwm_corr_sptr corr);
  bool obs_in_corr(bwm_observer_cam *obs);

  //: returns the corresponding points on an observer
  std::vector<vgl_point_2d<double> > get_corr_points(bwm_observer_cam *obs);

  //implement me!
  bool match(bwm_observer_cam *obs, vgl_point_2d<double> pt);
  void save_corr(std::ostream& s);
  void save_corr_XML();
  void update_corr(bwm_observer_cam* obs,vgl_point_2d<double> old_pt,vgl_point_2d<double> new_pt);
  void delete_last_corr();
  void delete_all_corr();
  BWM_CORR_MODE corr_mode() const { return corr_mode_; }
  BWM_CORR_TYPE corr_type() const { return corr_type_; }
  BWM_N_CORRS n_corrs() const { return n_corrs_; }
  void set_corr_mode();
  void set_corr_mode(BWM_CORR_MODE mode){corr_mode_ = mode;}
  void set_n_corrs(BWM_N_CORRS n){n_corrs_ = n;}
  void move_to_corr();

  //: picking up corr points are controlled by starting and stopping it
  void start_corr() { start_corr_ = true; }
  void stop_corr() { start_corr_ = false; }
  //: returns true if the correspondence picking started by the main corr menu
  bool in_corr_picking() const { return start_corr_; }

  void print_observers();

  std::vector<bwm_corr_sptr> correspondences()
    {return corr_list_;}

  //: Given a set of image-to-image correspondences, solve for the 3-d world point and adjust the cameras
  void adjust_camera_offsets();

  void find_terrain_points(std::vector<vgl_point_3d<double> >& points);

  //============ site to site correspondence methods ==============
  // methods for selecting correspondences between multiple sites
  // observables provide the site string name. 3-d points are constructed
  // from multiple views either as 3-d polygon vertices or polygon centroids.

  //: get all selected soviews to find the corresponding vertices or polygons
  std::vector<bwm_observable_sptr>
    all_selected_observables(std::string const& soview_type) const;

  //: requires exactly two selected vertices each in a unique site
  // (NOT YET IMPLEMENTED)
  bool add_3d_corr_vertex();

  //: requires exactly two selected polygons each in a unique site.
  //  Corresponds centroids of the polygons
  bool add_3d_corr_centroid();


  //: save 3d_corrs as an ascii file
  void save_3d_corrs() const;
  static void save_3d_corrs(std::string const& path,
                            std::vector<bwm_3d_corr_sptr> const& corrs);

  //: load 3d_corrs from an ascii file
  void load_3d_corrs();
  static void load_3d_corrs(std::string const& path,
                            std::vector<bwm_3d_corr_sptr>& corrs);
 private:
  bwm_observer_mgr() : start_corr_(false)
  {
    corr_mode_ = IMAGE_TO_IMAGE;
    n_corrs_ = SINGLE_PT_CORR;
    corr_type_ = FEATURE_CORR;
  }

  static bwm_observer_mgr* instance_;

  std::vector<bwm_observer* > observers_;

  bool start_corr_;
  BWM_CORR_MODE corr_mode_;
  BWM_N_CORRS n_corrs_;
  BWM_CORR_TYPE corr_type_;
  std::vector<bwm_corr_sptr> corr_list_;
  std::vector<bwm_corr_sptr> terrain_corr_list_;
  std::vector<bwm_3d_corr_sptr> site_to_site_corr_list_;
#if 0
  bool world_point_valid_;
  vgl_point_3d<double> corr_world_pt_;
#endif
};

#endif
