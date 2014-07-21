//This is brl/bbas/volm/conf/volm_conf_query.h
#ifndef volm_conf_query_h_
#define volm_conf_query_h_
//:
// \file A class to represent to an annotated query image through an 2d overhead map and estimated camera parameters
// Given one camera estimate, that is tilt, roll, focal, the query stores the coordinates (nearest points to camera center of labeled objects), orientation, height etc.
// and can be visualized by a 2-d overhead image
// Note that the coordinates are relative to camera center and represented under cylindrical coordinate system.  Also sky object is not considered in this matcher
//
// \author Yi DOng
// \date July 16, 2014
// \verbatim
//   Modifications
//    none yet
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_polygon.h>
#include <vsl/vsl_binary_io.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_category_io.h>
#include <volm/conf/volm_conf_object.h>

class volm_conf_query;
typedef vbl_smart_ptr<volm_conf_query> volm_conf_query_sptr;

class volm_conf_query : public vbl_ref_count
{
public:
  //: default constructor
  volm_conf_query();
  //: constructor from depth map scene
  volm_conf_query(volm_camera_space_sptr cam_space, depth_map_scene_sptr depth_scene);

  //: destructor
  ~volm_conf_query() {}

  //: Accessors
  vcl_vector<vpgl_perspective_camera<double> >& cameras()  { return cameras_; }
  vcl_vector<vcl_string>& camera_strings() { return camera_strings_; }
  vcl_vector<cam_angles>& camera_angles()  { return camera_angles_; }
  depth_map_scene_sptr depth_scene() const { return dm_; }
  unsigned ni() const { return ni_; }
  unsigned nj() const { return nj_; }
  unsigned ncam() const { return ncam_; }
  unsigned nobj() const { return nobj_; }
  unsigned nref() const { return nref_; }
  vcl_vector<vcl_string>& ref_obj_name() { return ref_obj_name_; }
  vcl_vector<vcl_map<vcl_string, vcl_vector<vgl_point_3d<float> > > >& obj_3d_polygons() { return obj_3d_polygons_; }
  vcl_map<vcl_string, unsigned char>& obj_land_ids() { return obj_land_ids_; }
  vcl_vector<vcl_map<vcl_string, volm_conf_object_sptr> >& conf_objects() { return conf_objects_; }

  //: some visualization method...

  //: IO ??

private:
  //: image size
  unsigned ni_;
  unsigned nj_;
  //: depth map scene
  depth_map_scene_sptr   dm_;
  //: number of cameras
  unsigned ncam_;
  //: number of objects
  unsigned nobj_;
  //: number of reference objects
  unsigned nref_;
  //: vector of cameras that satisfies ground plane constraint among all possible camera calibrations
  vcl_vector<vpgl_perspective_camera<double> > cameras_;
  vcl_vector<vcl_string> camera_strings_;
  vcl_vector<cam_angles> camera_angles_;
  //: list of reference objects specified in depth scene
  vcl_vector<vcl_string> ref_obj_name_;
  //: list of 3-d polygons generated from 2-d labeled polygon.  Map key is the object name
  vcl_vector<vcl_map<vcl_string, vcl_vector<vgl_point_3d<float> > > > obj_3d_polygons_;
  //: list of land type generated from 2-d labeled polygon.    Map key is the object name
  vcl_map<vcl_string, unsigned char> obj_land_ids_;
  //: list of configurational objects for each camera
  vcl_vector<vcl_map<vcl_string, volm_conf_object_sptr> > conf_objects_;

  //: construct reference objects list
  bool parse_ref_object(depth_map_scene_sptr depth_scene);
  //: construct cameras
  bool create_perspective_cameras(volm_camera_space_sptr cam_space);
  //: construct 3-d polygons from depth map scene for each camera
  bool create_3d_polygons(depth_map_scene_sptr depth_scene);
  //: construct the configurational objects for each camera
  bool create_conf_object();

};

#endif // volm_conf_query