#ifndef bwm_observable_h_
#define bwm_observable_h_

#include <vbl/vbl_ref_count.h>

#include "bwm_observable_sptr.h"

#include <vcl_string.h>
#include <vcl_map.h>

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

#include <vgui/vgui_observable.h>

#include <vsol/vsol_polygon_3d_sptr.h>

#include <bgeo/bgeo_lvcs.h>

class SoSeparator;

class bwm_observable : public vgui_observable, public vbl_ref_count
{
public:

  //: constructors
  bwm_observable() {}

  virtual ~bwm_observable() {}

  virtual vcl_string type_name() const {return "bwm_observable"; }

  // removes the object from 3D scene by notifiying the observers of deletion
  virtual void remove()=0;

  void virtual set_object(vsol_polygon_3d_sptr poly3d)=0;

  virtual vgl_box_3d<double> bounding_box()=0;

  virtual unsigned num_faces()=0;

  virtual void translate(vgl_vector_3d<double> T)=0;

  virtual bwm_observable_sptr transform(vgl_h_matrix_3d<double> T_)=0;

  virtual void extrude(int face_id)=0;

  virtual void extrude(int face_id, double dist)=0;

  virtual vsol_polygon_3d_sptr extract_face(unsigned i)=0;

  virtual vgl_homg_plane_3d<double> get_plane(unsigned face_id)=0;

  virtual vcl_map<int, vsol_polygon_3d_sptr> extract_faces()=0;

  virtual vcl_map<int, vsol_polygon_3d_sptr> extract_inner_faces(int face_id)=0;

  virtual void divide_face(unsigned face_id, 
    vgl_point_3d<double> l1, vgl_point_3d<double> l2,
    vgl_point_3d<double> p1, vgl_point_3d<double> l3, 
    vgl_point_3d<double> l4, vgl_point_3d<double> p2)=0;

  virtual void move_normal_dir(double dist)=0;
  virtual void move_extr_face(double dist)=0;
  virtual int find_closest_face(vgl_point_3d<double> point)=0;


  virtual void attach_inner_face(unsigned face_id, vsol_polygon_3d_sptr poly)=0;

  virtual void create_interior()=0;

  vgl_vector_3d<double> last_translation() { return last_translation_;}

  virtual void label_roof(unsigned face_id)=0;
  
  virtual void label_wall(unsigned face_id)=0;

  virtual SoSeparator* convert_coin3d(bool b_shape_hints, float transp, int colorcode)=0;

  virtual bwm_observable_sptr global_to_local(bgeo_lvcs* lvcs, double& min_z)=0;

  virtual void save(const char* filename, bgeo_lvcs* lvcs)=0;

  virtual void save(const char* filename)=0;

protected:

  vgl_vector_3d<double> last_translation_;
  
};

#endif
