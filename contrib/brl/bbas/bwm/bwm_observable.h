#ifndef bwm_observable_h_
#define bwm_observable_h_
//:
// \file

#include <string>
#include <iostream>
#include <map>
#include <vbl/vbl_ref_count.h>

#include "bwm_observable_sptr.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

#include <vgui/vgui_observable.h>

#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_line_3d_sptr.h>
#include <vpgl/vpgl_lvcs.h>


class bwm_observable : public vgui_observable, public vbl_ref_count
{
 public:

  // constructor
  bwm_observable(): path_("") {}

  bwm_observable(const bwm_observable&) : vgui_observable(), vbl_ref_count() {}

  virtual ~bwm_observable() {}

  virtual std::string type_name() const { return "bwm_observable"; }

  virtual int obj_type() const=0;

  // removes the object from 3D scene by notifying the observers of deletion
  virtual void remove()=0;

  void virtual set_object(vsol_polygon_3d_sptr poly3d)=0;

  virtual vgl_box_3d<double> bounding_box()=0;

  virtual unsigned num_faces() const=0;

  virtual void translate(vgl_vector_3d<double> T)=0;

  virtual bwm_observable_sptr transform(vgl_h_matrix_3d<double> T_)=0;

  virtual void send_update()=0;

  virtual void extrude(int face_id)=0;

  virtual void extrude(int face_id, double dist)=0;

  virtual vsol_polygon_3d_sptr extract_face(unsigned i)=0;

  virtual vsol_polygon_3d_sptr extract_bottom_face()=0;

  virtual vgl_plane_3d<double> get_plane(unsigned face_id)=0;

  virtual std::map<int, vsol_polygon_3d_sptr> extract_faces()=0;

  virtual std::map<int, vsol_line_3d_sptr> extract_edges()=0;

  virtual std::vector<vsol_point_3d_sptr> extract_vertices()=0;

  virtual std::map<int, vsol_polygon_3d_sptr> extract_inner_faces(int face_id)=0;

  virtual void divide_face(unsigned face_id,
                           vgl_point_3d<double> l1, vgl_point_3d<double> l2,
                           vgl_point_3d<double> p1, vgl_point_3d<double> l3,
                           vgl_point_3d<double> l4, vgl_point_3d<double> p2)=0;

  virtual void move_normal_dir(double dist)=0;
  virtual void move_extr_face(double dist)=0;
  virtual int find_closest_face(vgl_point_3d<double> point)=0;

  virtual void attach_inner_face(unsigned face_id, vsol_polygon_3d_sptr poly)=0;

  virtual void create_interior()=0;

  vgl_vector_3d<double> last_translation() const { return last_translation_; }

  virtual void label_roof(unsigned face_id)=0;

  virtual void label_wall(unsigned face_id)=0;

  virtual bwm_observable_sptr global_to_local(vpgl_lvcs* lvcs, double& min_z)=0;

  virtual void save(const char* filename, vpgl_lvcs* lvcs)=0;

  virtual void save(const char* filename)=0;

  std::string path() const { return path_; }
  void set_path(std::string const& path) { path_=path; }

  std::string site() const { return site_; }
  void set_site(std::string const& site) { site_=site; }

 protected:
  //: the observable file path
  std::string path_;
  //: the observable site name
  std::string site_;

  vgl_vector_3d<double> last_translation_;
};

#endif
