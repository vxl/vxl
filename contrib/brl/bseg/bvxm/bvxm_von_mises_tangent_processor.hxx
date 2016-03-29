#ifndef bvxm_von_mises_tangent_processor_hxx_
#define bvxm_von_mises_tangent_processor_hxx_

#include "bvxm_von_mises_tangent_processor.h"

#include <bsta/algo/bsta_von_mises_updater.h>
#include <bsta/algo/bsta_gaussian_updater.h>
#include <vpgl/vpgl_proj_camera.h>//restricted to projective camera for now
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_infinite_line_3d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_math.h>

const bool von_mises_debug = false;
template <class T>
bool bvxm_von_mises_tangent_processor<T>::
tangent_3d_from_2d(T img_a0, T img_b0, T img_c0,
                   vpgl_proj_camera<double> const& cam0,
                   T img_a1, T img_b1, T img_c1,
                   vpgl_proj_camera<double> const& cam1,
                   vgl_infinite_line_3d<T>& line_3d)
{
  //find the two planes corresponding to the image lines
  vgl_homg_line_2d<double> l0(img_a0, img_b0, img_c0);
  vgl_plane_3d<double> plane0 = cam0.backproject(l0);

  vgl_homg_line_2d<double> l1(img_a1, img_b1, img_c1);

  vgl_plane_3d<double> plane1 = cam1.backproject(l1);

  vgl_infinite_line_3d<double> line_3d_dbl;
  if (!vgl_intersection(plane0, plane1, line_3d_dbl))
    return false;
  //cast back to type
  vgl_point_3d<double> lp0d = line_3d_dbl.point();
  vgl_vector_3d<double> dir0d = line_3d_dbl.direction();
  vgl_point_3d<T> lp0(T(lp0d.x()),T(lp0d.y()),T(lp0d.z()));
  vgl_vector_3d<T> dir0(T(dir0d.x()),T(dir0d.y()),T(dir0d.z()));
  line_3d = vgl_infinite_line_3d<T>(lp0, dir0);
  return true;
}

template <class T>
bool bvxm_von_mises_tangent_processor<T>::
pos_dir_from_tangent_plane(vgl_plane_3d<T> const& plane,
                           pos_dist_t const& pos_dist,
                           dir_dist_t const& dir_dist,
                           vgl_infinite_line_3d<T>& line_3d)
{
  //the plane normal (is a unit vector)
  vgl_vector_3d<T> normal = plane.normal(), muv, dirv;
  //the mean direction vector from the von mises distribution
  dir_t mu = dir_dist.mean();
  muv.set(mu[0], mu[1], mu[2]);
  //the vector in the tangent plane closest to mu
  dirv = normalized(muv -dot_product(normal, muv)*normal);

  // the current mean x0 for the tangent line
  pos_t pos_mean = pos_dist.mean();
  vgl_vector_2d<T> pos_meanv(pos_mean[0], pos_mean[1]);
  // current mean tangent line
  vgl_infinite_line_3d<T> tan_line0(pos_meanv, muv);

  // the point on the line closest to the origin
  vgl_point_3d<T> p0 = tan_line0.point();
  vgl_point_3d<T> close_pt = vgl_closest_point(plane, p0);
  // construct a new line representing the new sample tangent line
  line_3d = vgl_infinite_line_3d<T>(close_pt, dirv);
  return true;
}

template <class T>
bool bvxm_von_mises_tangent_processor<T>::
pos_dir_from_image_tangent(T img_a, T img_b, T img_c,
                           vpgl_proj_camera<double> const& cam,
                           pos_dist_t const& pos_dist,
                           dir_dist_t const& dir_dist,
                           vgl_infinite_line_3d<T>& line_3d)
{
  //find the back projected plane from the line tangent
  vgl_homg_line_2d<double> l(static_cast<double>(img_a),
                             static_cast<double>(img_b),
                             static_cast<double>(img_c));
  vgl_plane_3d<double> plane = cam.backproject(l);
  vgl_plane_3d<T> plane_T(static_cast<T>(plane.a()),
                          static_cast<T>(plane.b()),
                          static_cast<T>(plane.c()),
                          static_cast<T>(plane.d()));
  return bvxm_von_mises_tangent_processor<T>::
    pos_dir_from_tangent_plane(plane_T, pos_dist, dir_dist, line_3d);
}

template <class T>
bool bvxm_von_mises_tangent_processor<T>::
update(bvxm_voxel_slab<dir_dist_t> & dir_dist,
       bvxm_voxel_slab<pos_dist_t> & pos_dist,
       bvxm_voxel_slab<dir_t> const& dir,
       bvxm_voxel_slab<pos_t> const& pos,
       bvxm_voxel_slab<bool> const& flag)
{
  //the updater
  bsta_von_mises_updater<bsta_von_mises<T, 3> > vm_3d_updater;
  //the slab iterators
  typename bvxm_voxel_slab<dir_dist_t>::iterator dir_dist_it = dir_dist.begin();
  typename bvxm_voxel_slab<pos_dist_t>::iterator pos_dist_it = pos_dist.begin();
  typename bvxm_voxel_slab<dir_t>::const_iterator dir_it = dir.begin();
  typename bvxm_voxel_slab<pos_t>::const_iterator pos_it = pos.begin();
  typename bvxm_voxel_slab<bool>::const_iterator flag_it = flag.begin();
  for (;
       (dir_dist_it!=dir_dist.end())&&(pos_dist_it!=pos_dist.end())&&
       (dir_it!=dir.end())&&(pos_it!=pos.end())&&(flag_it!=flag.end());
       ++dir_dist_it, ++pos_dist_it, ++dir_it, ++pos_it, ++flag_it)
    if (*flag_it)
    {
      T npos = (*pos_dist_it).num_observations + T(1.0);
      T ndir = (*dir_dist_it).num_observations + T(1.0);
      if (ndir != npos)
        return false;
      T alpha = T(1);
      //case I - initial stage with no learning acceleration
      if (npos <= k_) {
        alpha = T(1)/npos;
        if (npos ==1) {
          T init_kap = (*dir_dist_it).kappa();//override updater initialization
          vm_3d_updater(*dir_dist_it, *dir_it, alpha);
          (*dir_dist_it).set_kappa(init_kap);
          T var = (*pos_dist_it).var();
          //update with alpha==1 sets var=0
          bsta_update_gaussian<T, 2>(*pos_dist_it, alpha, *pos_it);
          //restore the correct initial var
          (*pos_dist_it).set_var(var);
          (*pos_dist_it).num_observations = npos;
          //note that von mises does its own attribute updating
          continue;
        }
      }
      else //case II, learning is accelerated compared to simple averaging
      alpha = k_/npos;
      double dir_prob =  (*dir_dist_it).probability(*dir_it, x0_radius_);
      pos_t delta(x0_radius_);
      pos_t s = (*pos_it);
      pos_t minp = s-delta;
      pos_t maxp = s+delta;
      double pos_prob = (*pos_dist_it).probability(minp, maxp);
      double prob = std::pow(dir_prob*pos_prob, 0.125);
      if (prob<0.05) continue;//JLM temporary hardcoded threshold
      //update the number of observations on the pos distribution
      (*pos_dist_it).num_observations = npos;
      //note that von mises does its own attribute updating
      // previous mean direction
      dir_t mean_dir = (*dir_dist_it).mean();
      vgl_vector_3d<T> mean_dirv(mean_dir[0], mean_dir[1], mean_dir[2]);
      //update the direction
      vm_3d_updater(*dir_dist_it, *dir_it, alpha);

      // the updated direction
      dir_t updt_mean = (*dir_dist_it).mean();
      vgl_vector_3d<T> updt_mean_dirv(updt_mean[0], updt_mean[1],
                                      updt_mean[2]);
      // the current mean position before update
      pos_t mean_pos = (*pos_dist_it).mean();
      vgl_vector_2d<T> mean_posv(mean_pos[0], mean_pos[1]);

      // The mean tangent line before update
      vgl_infinite_line_3d<T> mean_line(mean_posv, mean_dirv);

      // The sample tangent line
      vgl_vector_3d<T> samp_dirv((*dir_it)[0],(*dir_it)[1], (*dir_it)[2]);
      vgl_vector_2d<T> samp_posv((*pos_it)[0], (*pos_it)[1]);
      vgl_infinite_line_3d<T> samp_line(samp_posv, samp_dirv);

      //Find the endpoints of the line joining the closest points
      //on the mean tangent line and the sample tangent line
      bool u = false;
      std::pair<vgl_point_3d<T>, vgl_point_3d<T> > closest
        = vgl_closest_points(mean_line, samp_line, &u);
      double pdist = (closest.first-closest.second).length();//jlm debug
      //generate a new mean line with the modified intersection point
      vgl_infinite_line_3d<T> adj_mean_line(closest.first, updt_mean_dirv);
      //generate a new sample line with the modified intersection point
      vgl_infinite_line_3d<T> adj_samp_line(closest.second, updt_mean_dirv);

      //convert adjusted positions to the pos_t data type
      vgl_vector_2d<T> adj_mean_posv = adj_mean_line.x0();
      vgl_vector_2d<T> adj_samp_posv = adj_samp_line.x0();
      pos_t adj_mean_pos, adj_samp_pos;
      adj_mean_pos[0]=adj_mean_posv.x(); adj_mean_pos[1]=adj_mean_posv.y();
      adj_samp_pos[0]=adj_samp_posv.x(); adj_samp_pos[1]=adj_samp_posv.y();
#if 0
      //update the position distribution
      (*pos_dist_it).set_mean(adj_mean_pos);
#endif
      bsta_update_gaussian<T, 2>(*pos_dist_it, alpha, adj_samp_pos);
      pos_t diff = mean_pos-(*pos_dist_it).mean();
      double dist = diff.magnitude();
      if (von_mises_debug)
        std::cout  << std::fabs(angle(updt_mean_dirv, mean_dirv))*vnl_math::deg_per_rad
                  << ' ' << dir_prob << ' ' << pdist << ' ' <<  pos_prob <<'\n'
                  << "Distance: " << dist << '\n';
    }
  return true;
}


#undef BVXM_VON_MISES_TANGENT_PROCESSOR_INSTANTIATE
#define BVXM_VON_MISES_TANGENT_PROCESSOR_INSTANTIATE(T) \
template class bvxm_von_mises_tangent_processor<T >

#endif // bvxm_von_mises_tangent_processor_hxx_
