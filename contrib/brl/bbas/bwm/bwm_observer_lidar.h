#ifndef bwm_observer_lidar_h_
#define bwm_observer_lidar_h_

#include "bwm_observer_cam.h"
#include "bwm_observable.h"
#include "bwm_observable_sptr.h"
#include "algo/bwm_lidar_algo.h"
#include <vcl_map.h>

#include <vgui/vgui_observer.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_3d_sptr.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>

#include <vnl/vnl_matrix.h>
#include <bgeo/bgeo_lvcs.h>
#include <vil/vil_image_view.h>
#include <vil/file_formats/vil_geotiff_header.h>

class bwm_observer_lidar: public bwm_observer_cam
{
public:
  //constructors & destructor
  bwm_observer_lidar() {}

  bwm_observer_lidar(bgui_image_tableau_sptr const& img, 
    vil_image_resource_sptr res1, 
    vil_image_resource_sptr res2);

  virtual ~bwm_observer_lidar() {}

  virtual vcl_string type_name() const { return "bwm_observer_lidar"; }
  //: Handle all events sent to this tableau.
  // bool handle(const vgui_event& e);
  // {return vgui_easy2D_tableau::handle(e);}


  void update(vgui_message const& msg){}
  void handle_update(vgui_message const& msg, 
    bwm_observable_sptr observable){} 

  void update_all(){}
  void translate(vgl_vector_3d<double> T){}

  //: connect the the given face as an inner face to the selected face
  void connect_inner_face(vsol_polygon_2d_sptr poly){}

  //: create the interior of an objects, as a smaller copy of the outer 
  // object
  void create_interior(){}

  //: Select a polygon before you call this method. If it is a multiface 
  // object, it deletes the object where the selected polygon belongs to
  void delete_object(){}

  //: Deletes the whole set of objects created so far
  void delete_all(){}

  //: Special case of backprojecting onto the projection plane
  void backproj_poly(vsol_polygon_2d_sptr poly2d, 
    vsol_polygon_3d_sptr& poly3d);

  void proj_poly(vsol_polygon_3d_sptr poly3d, 
    vsol_polygon_2d_sptr& poly2d);

  void backproj_point(vsol_point_2d_sptr p2d,  vsol_point_3d_sptr &p3d);

  void set_lvcs(double lat, double lon, double elev);

  void set_ground_plane(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

  float ground_elev() { return ground_elev_; }

  bool get_point_cloud(const float x1, const float y1, 
    const float x2, const float y2, 
    bgeo_lvcs& lvcs, vcl_vector<vsol_point_3d_sptr>& points);

  bool save_meshed_point_cloud(const float x1, const float y1, 
    const float x2, const float y2, vcl_string file);

  void get_img_to_wgs(const unsigned i, const unsigned j, 
    double& lon, double& lat, double& elev);

  void label_lidar(lidar_labeling_params params);

private:

  vbl_smart_ptr<bgeo_lvcs> lvcs_;

  vnl_matrix<double> trans_matrix_;

  vcl_vector<vcl_vector<double> > tiepoints;

  vil_image_resource_sptr first_ret_;

  vil_image_resource_sptr second_ret_;

  vil_image_view<float> img_view_;

  float ground_elev_;

  double scale_;

  void set_trans_matrix(vil_geotiff_header* gtif, 
    vcl_vector<vcl_vector<double> > tiepoints);

  void proj_point(vsol_point_3d_sptr p3d,  vsol_point_2d_sptr& p2d);

  void img_to_wgs(const unsigned i, const unsigned j, 
    double& lon, double& lat);

};

#endif
