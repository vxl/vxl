#ifndef bwm_observable_textured_mesh_h_
#define bwm_observable_textured_mesh_h_
// :
// \file

#include "bwm_observable_mesh.h"

#include <bmsh3d/bmsh3d_textured_mesh_mc.h>
#include <bmsh3d/bmsh3d_textured_face_mc.h>

#include <vcl_iosfwd.h>

class bwm_observable_textured_mesh : public bwm_observable_mesh
{
public:

  vcl_string type_name() const {return "bwm_observable_textured_mesh"; }

  // : constructors
  bwm_observable_textured_mesh()
    : object_(0) /*, base_(0)*/ {}

  bwm_observable_textured_mesh(bmsh3d_textured_mesh_mc* object)
    : object_(object) {}

  virtual ~bwm_observable_textured_mesh();

  vcl_string tex_map_uri()
  {
    bmsh3d_textured_face_mc* face =
      static_cast<bmsh3d_textured_face_mc *>(object_->facemap(0) ); return face->tex_map_uri();
  }

  void save_gml(vcl_ostream & os, int obj_count, vpgl_lvcs* lvcs);

  void save_kml(vcl_ostream & os, int obj_count, vpgl_lvcs* lvcs, double ground_height, double x_offset,
                double y_offset );

  void save_kml_collada(vcl_ostream & os, vpgl_lvcs* lvcs, vcl_string geometry_id, vcl_string geometry_position_id,
                        vcl_string geometry_position_array_id, vcl_string geometry_uv_id,
                        vcl_string geometry_uv_array_id, vcl_string geometry_vertex_id, vcl_string material_name);

  void save_x3d(vcl_ostream & os, vpgl_lvcs* lvcs);

private:

  bmsh3d_textured_mesh_mc* object_;
};

#endif
