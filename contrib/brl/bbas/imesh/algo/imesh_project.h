// This is brl/bbas/imesh/algo/imesh_project.h
#ifndef imesh_project_h_
#define imesh_project_h_
//:
// \file
// \brief Functions to project a mesh into an image
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date Nov. 8, 2005
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <imesh/imesh_mesh.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_lens_distortion.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_box_2d.h>

//: project the 3D vertices into 2D using the camera
void imesh_project_verts(const std::vector<vgl_point_3d<double> >& verts3d,
                         const vpgl_proj_camera<double>& camera,
                         std::vector<vgl_point_2d<double> >& verts2d);

//: project the 3D vertices into 2D using the camera
void imesh_project_verts(const imesh_vertex_array<3>& verts3d,
                         const vpgl_proj_camera<double>& camera,
                         std::vector<vgl_point_2d<double> >& verts2d);

//: project the 3D vertices into 2D vertices and depths using the camera
void imesh_project_verts(const std::vector<vgl_point_3d<double> >& verts3d,
                         const vpgl_proj_camera<double>& camera,
                         std::vector<vgl_point_2d<double> >& verts2d,
                         std::vector<double>& depths);

//: project the 3D mesh vertices into 2D vertices and depths using the camera
void imesh_project_verts(const imesh_vertex_array<3>& verts3d,
                         const vpgl_proj_camera<double>& camera,
                         std::vector<vgl_point_2d<double> >& verts2d,
                         std::vector<double>& depths);

//: distort the 2D vertices using the lens
void imesh_distort_verts(const std::vector<vgl_point_2d<double> >& in_verts,
                         const vpgl_lens_distortion<double>& lens,
                         std::vector<vgl_point_2d<double> >& out_verts);

//: project the mesh onto the image plane using the camera and lens distortion
//  Set each pixel of the image to true if the mesh projects onto it
void imesh_project(const imesh_mesh& mesh,
                   const vpgl_proj_camera<double>& camera,
                   const vpgl_lens_distortion<double>& lens,
                   vil_image_view<bool>& image);

//: project the front-facing triangles of the mesh onto the image plane
//  Using the camera and lens distortion
//  Set each pixel of the image to true if the mesh projects onto it
//  The optional \p bbox returns a 2D bounding box for the projection in the image
void imesh_project(const imesh_mesh& mesh,
                   const std::vector<vgl_vector_3d<double> >& normals,
                   const vpgl_proj_camera<double>& camera,
                   const vpgl_lens_distortion<double>& lens,
                   vil_image_view<bool>& image,
                   vgl_box_2d<unsigned int>* bbox = nullptr);

//: project the mesh onto the image plane using the camera
//  Set each pixel of the image to true if the mesh projects onto it
void imesh_project(const imesh_mesh& mesh,
                   const vpgl_proj_camera<double>& camera,
                   vil_image_view<bool>& image);

//: project the mesh onto the image plane using the camera
//  Set each pixel of the image to the depth to the mesh
void imesh_project_depth(const imesh_mesh& mesh,
                         const vpgl_proj_camera<double>& camera,
                         vil_image_view<double>& image);

//: Render a triangle defined by its vertices
void imesh_render_triangle_interp(const vgl_point_2d<double>& v1,
                                  const vgl_point_2d<double>& v2,
                                  const vgl_point_2d<double>& v3,
                                  const double& i1, const double& i2, const double& i3,
                                  vil_image_view<double>& image);

//: Render the faces of the mesh into the image by interpolating the values at the vertices
//  The minimum value is kept at each pixel (as in computing a depth map)
void imesh_render_triangles_interp(const imesh_regular_face_array<3>& tris,
                                   const std::vector<vgl_point_2d<double> >& img_verts,
                                   const std::vector<double>& vals,
                                   vil_image_view<double>& image);

//: Triangulates the faces and then calls imesh_render_triangles_interp
void imesh_render_faces_interp(const imesh_mesh& mesh,
                               const std::vector<vgl_point_2d<double> >& img_verts,
                               const std::vector<double>& vals,
                               vil_image_view<double>& image);

//: Compute the bounds of the projection of a set of image points
//  The returned bounds are the intersection of the input bounds
//  and the bounding box of the points
void imesh_projection_bounds(const std::vector<vgl_point_2d<double> >& img_pts,
                             vgl_box_2d<unsigned int>& bbox);

//: back project an image point onto the mesh using the camera
//  Returns the index of the intersected triangle, or -1 for no intersection
int imesh_project_onto_mesh(const imesh_mesh& mesh,
                            const std::vector<vgl_vector_3d<double> >& normals,
                            const std::vector<vgl_point_2d<double> >& verts2d,
                            const vpgl_perspective_camera<double>& camera,
                            const vgl_point_2d<double>& pt_2d,
                            vgl_point_3d<double>& pt_3d);

//: project a texture point onto a mesh face index with barycentric coordinates
//  Returns the index of the intersected triangle, or -1 for no intersection
int imesh_project_texture_to_barycentric(const imesh_mesh& mesh,
                                         const vgl_point_2d<double>& pt_2d,
                                         vgl_point_2d<double>& pt_uv);

//: project a texture polygon into barycentric coordinates
//  \returns true if the polygon is not clipped by the mesh texture
//  \returns the vector of barycentric points by reference
//  \returns a vector of coded half edge indices
//  the half edge indices are scaled by a factor of 4
//  the last two bits indicate the intersection type:
//  - 0 for face
//  - 1 for edge
//  - 2 for vertex
//  barycentric coordinate refer to the adjacent triangle
//  \returns a mapping from each original point into barycentric points.
//  if an original point is not mapped the value is -1
bool imesh_project_texture_to_barycentric(const imesh_mesh& mesh,
                                          const std::vector<vgl_point_2d<double> >& pts_2d,
                                          std::vector<vgl_point_2d<double> >& pts_uv,
                                          std::vector<unsigned long>& idxs,
                                          std::vector<int>& map_back);

//: compute the matrix that maps texture points to 3-d for a given triangle index
// $(x,y,1)$ maps into 3-d $(x,y,z)$
vnl_matrix_fixed<double,3,3>
imesh_project_texture_to_3d_map(const imesh_mesh& mesh, unsigned int tidx);

//: compute the affine matrix that maps triangle (a1,b1,c1) to (a2,b2,c2)
vnl_matrix_fixed<double,3,3>
imesh_affine_map(const vgl_point_2d<double>& a1,
                 const vgl_point_2d<double>& b1,
                 const vgl_point_2d<double>& c1,
                 const vgl_point_2d<double>& a2,
                 const vgl_point_2d<double>& b2,
                 const vgl_point_2d<double>& c2);

//: project barycentric coordinates with an index to texture space
//  \param idx is the face index
vgl_point_2d<double>
imesh_project_barycentric_to_texture(const imesh_mesh& mesh,
                                     const vgl_point_2d<double>& pt_uv,
                                     unsigned int idx);

//: project barycentric coordinates with an index the mesh surface (3D)
//  \param idx is the face index
vgl_point_3d<double>
imesh_project_barycentric_to_mesh(const imesh_mesh& mesh,
                                  const vgl_point_2d<double>& pt_uv,
                                  unsigned int idx);

//: back project image points onto the mesh using the camera
//  Returns a vector of all valid 3d points and indices to corresponding 2d points
void imesh_project_onto_mesh(const imesh_mesh& mesh,
                             const std::vector<vgl_vector_3d<double> >& normals,
                             const vpgl_perspective_camera<double>& camera,
                             const std::vector< vgl_point_2d<double> >& pts_2d,
                             std::vector<unsigned int >& idx_2d,
                             std::vector<vgl_point_3d<double> >& pts_3d);

//: back project an image point onto the mesh using the camera
//  The resulting point is in barycentric coordinates for the returned triangle
//  Returns the index of the intersected triangle, or -1 for no intersection
int imesh_project_onto_mesh_barycentric(const imesh_mesh& mesh,
                                        const std::vector<vgl_vector_3d<double> >& normals,
                                        const std::vector<vgl_point_2d<double> >& verts2d,
                                        const vpgl_perspective_camera<double>& camera,
                                        const vgl_point_2d<double>& pt_img,
                                        vgl_point_2d<double>& pt_bary);

//: back project an image point onto the mesh using the camera
//  Then project from the mesh into normalized texture coordinate (UV)
//  Assumes the mesh has both normals and texture coordinates
//  Returns the index of the intersected triangle, or -1 for no intersection
int imesh_project_onto_mesh_texture(const imesh_mesh& mesh,
                                    const std::vector<vgl_point_2d<double> >& verts2d,
                                    const vpgl_perspective_camera<double>& camera,
                                    const vgl_point_2d<double>& pt_img,
                                    vgl_point_2d<double>& pt_uv);

#endif // imesh_project_h_
