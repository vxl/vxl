#include "boxm_raytrace_operations.h"

bool boxm_alpha_seg_len(double *xverts_2d, double* yverts_2d, float* vert_distances, boct_face_idx visible_faces, float alpha, vil_image_view<float> &alpha_distance)
{
  // multiply each vertex distance by alpha
  float vert_alpha_distances[8];
  float *vert_dist_ptr = vert_distances;
  float *vert_alpha_dist_ptr = vert_alpha_distances;
  for (unsigned int i=0; i<8; ++i) {
    *vert_alpha_dist_ptr++ = *vert_dist_ptr++ * alpha;
  }

  // for each face, create two triangle iterators and fill in pixel data
  // X_LOW
  // tri 0
  boxm_triangle_interpolation_iterator<float> tri_it(xverts_2d, yverts_2d, vert_alpha_distances, 0, 4, 3);
  if (visible_faces & X_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);

  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 4, 7, 3);
  if (visible_faces & X_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);


  // X_HIGH
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 1, 2, 5);
  if (visible_faces & X_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);

  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 2, 6, 5);
  if (visible_faces & X_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);


  // Y_LOW
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 0, 1, 5);
  if (visible_faces & Y_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 0, 5, 4);
  if (visible_faces & Y_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);

  // Y_HIGH
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 3, 2, 6);
  if (visible_faces & Y_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 3, 6, 7);
  if (visible_faces & Y_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);

  // Z_LOW
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 0, 1, 2);
  if (visible_faces & Z_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 0, 2, 3);
  if (visible_faces & Z_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);

  // Z_HIGH
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 4, 5, 6);
  if (visible_faces & Z_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 4, 7, 6);
  if (visible_faces & Z_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);

  return true;
}


bool boxm_depth_fill(double *xverts_2d, double* yverts_2d,
                     float* vert_distances, boct_face_idx visible_faces,
                     vil_image_view<float> &depth_image)
{
  // for each face, create two triangle iterators and fill in pixel data
  // X_LOW
  // tri 0
  boxm_triangle_interpolation_iterator<float> tri_it(xverts_2d, yverts_2d, vert_distances, 0, 4, 3);
  if (visible_faces & X_LOW)
    tri_interpolate_values(tri_it, depth_image, true);

  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_distances, 4, 7, 3);
  if (visible_faces & X_LOW)
    tri_interpolate_values(tri_it, depth_image, true);


  // X_HIGH
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_distances, 1, 2, 5);
  if (visible_faces & X_HIGH)
    tri_interpolate_values(tri_it, depth_image, true);

  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_distances, 2, 6, 5);
  if (visible_faces & X_HIGH)
    tri_interpolate_values(tri_it, depth_image, true);


  // Y_LOW
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_distances, 0, 1, 5);
  if (visible_faces & Y_LOW)
    tri_interpolate_values(tri_it, depth_image, true);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_distances, 0, 5, 4);
  if (visible_faces & Y_LOW)
    tri_interpolate_values(tri_it, depth_image, true);

  // Y_HIGH
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_distances, 3, 2, 6);
  if (visible_faces & Y_HIGH)
    tri_interpolate_values(tri_it, depth_image, true);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_distances, 3, 6, 7);
  if (visible_faces & Y_HIGH)
    tri_interpolate_values(tri_it, depth_image, true);

  // Z_LOW
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_distances, 0, 1, 2);
  if (visible_faces & Z_LOW)
    tri_interpolate_values(tri_it, depth_image, true);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_distances, 0, 2, 3);
  if (visible_faces & Z_LOW)
    tri_interpolate_values(tri_it, depth_image, true);

  // Z_HIGH
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_distances, 4, 5, 6);
  if (visible_faces & Z_HIGH)
    tri_interpolate_values(tri_it, depth_image, true);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_distances, 4, 7, 6);
  if (visible_faces & Z_HIGH)
    tri_interpolate_values(tri_it, depth_image, true);

  return true;
}


bool cube_fill_value(double* xverts_2d, double* yverts_2d, boct_face_idx visible_faces, vil_image_view<float> &img, float const& val)
{
  // for each face, create two triangle iterators and fill in pixel data
  boxm_triangle_scan_iterator tri_it(xverts_2d, yverts_2d, 0,4,3);
  if (visible_faces & X_LOW) {
    // tri 0
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 0, 4, 3);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 4, 7, 3);
    tri_fill_value(tri_it, img, val);
  }
  if (visible_faces & X_HIGH) {
    // tri 0
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 1, 2, 5);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 2, 6, 5);
    tri_fill_value(tri_it, img, val);
  }
  if (visible_faces & Y_LOW) {
    // tri 0
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 0, 1, 5);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 0, 5, 4);
    tri_fill_value(tri_it, img, val);
  }
  if (visible_faces & Y_HIGH) {
    // tri 0
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 3, 2, 6);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 3, 6, 7);
    tri_fill_value(tri_it, img, val);
  }
  if (visible_faces & Z_LOW) {
    // tri 0
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 0, 1, 2);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 0, 2, 3);
    tri_fill_value(tri_it, img, val);
  }
  if (visible_faces & Z_HIGH) {
    // tri 0
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 4, 5, 6);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = boxm_triangle_scan_iterator(xverts_2d, yverts_2d, 4, 7, 6);
    tri_fill_value(tri_it, img, val);
  }
  return true;
}
