#include "bvxm_expected_edge_functor.h"

#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/algo/vgl_convex_hull_2d.h>

bool bvxm_expected_edge_functor::apply(const vpgl_camera_double_sptr& camera,
                                       vil_image_view<float> *img_eei)
{
  img_eei->fill(0.0f);

  int ni = img_eei->ni();
  int nj = img_eei->nj();

  // check the camera type
  if (camera->type_name() == "vpgl_perspective_camera") {
    auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.ptr());
    if (!cam)
      return false;

    // Layer version of expected edge image
    //vgl_box_3d<double> box_(sx_,sy_,(sz_+((double)nz_)*dz_),(sx_+((double)nx_)*dx_),(sy_+((double)ny_)*dy_),sz_);
    vgl_polygon<double> poly_2d = convert_3d_box_to_2d_polygon(box_,cam);
    double sz = box_.max_z();
    double sy = box_.min_y();
    double sx = box_.min_x();
    double dz = vox_dim_.z();
    double dy = vox_dim_.y();
    double dx = vox_dim_.x();
    double nz = world_dim_.z();
    double ny = world_dim_.y();
    double nx = world_dim_.x();

    vgl_homg_plane_3d<double> plane_0(0.0,0.0,1.0,-sz);
    vgl_homg_plane_3d<double> plane_1(0.0,0.0,1.0,-sz-dz);

    for (int i=0; i<ni; i++) {
      for (int j=0; j<nj; j++) {
        if (poly_2d.contains(double(i),(double)j)) {
          // following line 640 milliseconds
          vgl_line_3d_2_points<double> bp = cam->backproject(vgl_point_2d<double>((double)i,(double)j));
          vgl_homg_line_3d_2_points<double> bp_h(vgl_homg_point_3d<double>(bp.point1()),vgl_homg_point_3d<double>(bp.point2()));
          vgl_point_3d<double> pt_0(vgl_homg_operators_3d<double>::intersect_line_and_plane(bp_h,plane_0));
          vgl_point_3d<double> pt_1(vgl_homg_operators_3d<double>::intersect_line_and_plane(bp_h,plane_1));

          vgl_vector_3d<double> diff = pt_1 - pt_0;

          double start_x = pt_0.x();
          double start_y = pt_0.y();
          //double start_z = pt_0.z();

          double diff_x = diff.x();
          double diff_y = diff.y();
          //double diff_z = diff.z();

          for (int vz=0; vz<nz; vz++) {
            int vx = (int)((((start_x + (diff_x*vz)) - sx)/dx) + 0.5f);
            if (vx<0 || vx>=nx) {
              continue;
            }

            int vy = (int)((((start_y + (diff_y*vz)) - sy)/dy) + 0.5f);
            if (vy<0 || vy>=ny) {
              continue;
            }

            // this line takes 2200 milliseconds
            // with float ***data it takes 1100 ms
            (*img_eei)(i,j) = std::max((*img_eei)(i,j),(*slab_)(vx,vy,vz));
          }
        }
      }
    }
    return true;
  }
  else {
    std::cout << "bvxm_expected_edge_functor::apply() -- The camera type: " << camera->type_name() << " is not implemented yet!" << std::endl;
    return false;
  }
}

std::vector<vgl_point_3d<double> > bvxm_expected_edge_functor::convert_3d_box_to_3d_points(const vgl_box_3d<double> box_3d)
{
  double box_x[2],box_y[2],box_z[2];
  box_x[0] = box_3d.min_x();
  box_y[0] = box_3d.min_y();
  box_z[0] = box_3d.min_z();
  box_x[1] = box_3d.max_x();
  box_y[1] = box_3d.max_y();
  box_z[1] = box_3d.max_z();

  std::vector<vgl_point_3d<double> > box_pts_3d;
  for (double & i : box_x) {
    for (double & j : box_y) {
      for (double & k : box_z) {
        box_pts_3d.emplace_back(i,j,k);
      }
    }
  }

  return box_pts_3d;
}

vgl_polygon<double> bvxm_expected_edge_functor::convert_3d_box_to_2d_polygon(const vgl_box_3d<double> box_3d,
                                                                             const vpgl_perspective_camera<double> *cam)
{
  std::vector<vgl_point_3d<double> > box_pts_3d = convert_3d_box_to_3d_points(box_3d);

  std::vector<vgl_point_2d<double> > box_pts_2d;
  for (auto & i : box_pts_3d) {
    double u,v;
    cam->project(i.x(),i.y(),i.z(),u,v);
    vgl_point_2d<double> curr_pt_2d(u,v);
    box_pts_2d.push_back(curr_pt_2d);
  }

  vgl_convex_hull_2d<double> ch(box_pts_2d);
  vgl_polygon<double> polygon_2d = ch.hull();

  return polygon_2d;
}
