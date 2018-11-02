// This is brl/bseg/sdet/sdet_image_mesh.h
#ifndef sdet_image_mesh_h_
#define sdet_image_mesh_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for triangulating a images based on detected line segments
//
//  This algorithm triangulates an image into a 3-d mesh based on
//  detected line segments. The line segments are fit to edge detection
//  contour chains.
//
// \author
//  J.L. Mundy - March 21, 2011
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_line_segment_2d.h>
#include <sdet/sdet_image_mesh_params.h>
#include <imesh/imesh_mesh.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>

class sdet_image_mesh : public sdet_image_mesh_params
{
 public:
  // === Constructors/destructor ===
  sdet_image_mesh(sdet_image_mesh_params& imp);
  ~sdet_image_mesh() override;

  // === Process methods ===
  void set_image(vil_image_resource_sptr const& resource);
  bool compute_mesh();

  // === Accessors ===
  imesh_mesh& get_mesh() { return mesh_; }
  bool mesh_valid() const { return mesh_valid_; }

 protected:
  // === protected methods ===
  bool step_boundary(vgl_line_segment_2d<double> const& parent,
                     vgl_line_segment_2d<double>& child0,
                     vgl_line_segment_2d<double>& child1);
  void set_anchor_points(imesh_mesh& mesh, vil_image_view<float> dt_img);
  bool compute_line_segments(vil_image_resource_sptr const& resc,
                             std::vector<vgl_line_segment_2d<double> >& segs);

  // === members ===
  bool mesh_valid_;      //: process state flag
  vil_image_resource_sptr resc_;
  imesh_mesh mesh_;
};

#endif // sdet_image_mesh_h_
