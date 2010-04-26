#ifndef boxm_plane_ransac_h_
#define boxm_plane_ransac_h_

#include <vgl/vgl_infinite_line_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/algo/vgl_intersection.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_closest_point.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_edge_tangent_sample.h>
#include <vcl_cstdlib.h> // for rand()
#include <vcl_iostream.h>
#include <vcl_list.h>
#define ITER_MAX 100

template <class T>
bool boxm_plane_ransac(vcl_vector<boxm_edge_tangent_sample<T> > aux_samples,
                       vcl_vector<T> weights,
                       vgl_infinite_line_3d<T>& line,
                       T &residual, vgl_box_3d<double> cell_global_box,
                       unsigned int threshold)
{
  unsigned int num_imgs = aux_samples.size();
  vgl_point_3d<T> local_origin((T)cell_global_box.centroid_x(),(T)cell_global_box.centroid_y(),(T)cell_global_box.centroid_z());
  vcl_list<vgl_plane_3d<T> > fit_planes;
  T min_res=T(1e10);
  line=vgl_infinite_line_3d<T>(vgl_vector_2d<T>(-10000,-10000),vgl_vector_3d<T>(0,0,1));
  vcl_vector<T> ws;
  bool set=false;
  for (unsigned iter=0; iter<ITER_MAX; iter++) {
      fit_planes.clear();
      ws.clear();
      // select two imgs randomly
      int index1 = vcl_rand() % num_imgs;
      int index2 = index1;
      while(index2==index1)
      {
          index2=vcl_rand() % num_imgs;
      }

      int index1_1 =vcl_rand() % aux_samples[index1].num_obs();
      int index2_1 =vcl_rand() % aux_samples[index2].num_obs();

      vgl_plane_3d<T> plane1 = aux_samples[index1].obs(index1_1).plane_;
      vgl_plane_3d<T> plane2 = aux_samples[index2].obs(index2_1).plane_;

      vgl_infinite_line_3d<T> inters_line;
      if (vgl_intersection(plane1, plane2, inters_line)) {
          vgl_vector_3d<T> line_dir = inters_line.direction();
          //vcl_cout<<"Line dir "<<line_dir;
          int cnt_imgs=0;
          // test the line on each plane
          for (unsigned i=0; i<aux_samples.size(); ++i) {
              bool flag=false;
              T accu_weights_per_image=T(0);
              vcl_vector<T> weights_per_image;
              for (unsigned j=0;j<aux_samples[i].num_obs();j++)
              {
                  vgl_plane_3d<T> plane = aux_samples[i].obs(j).plane_;
                  vgl_vector_3d<T> normal = plane.normal();
                  // see if the line direction and plane normal is perpendicular
                  T res = dot_product(normal,line_dir);
                  if (vcl_fabs(res) < 0.05)  {
                      vgl_point_3d<T> p=vgl_closest_point<T>(inters_line,local_origin);
                      if (plane.contains(p,cell_global_box.width()/8)) 
                      {
                          fit_planes.push_back(plane);
                          ws.push_back(weights[i]);
                          //weights_per_image.push_back(weights[i]);
                          //accu_weights_per_image+=weights[i];
                          flag=true;
                      }
                  }
              }
              
              if(flag)
              {
                  //for(unsigned l=0;l<weights_per_image.size();l++)
                  //    ws.push_back(weights_per_image[l]/accu_weights_per_image);
                  cnt_imgs++;
              }
          }

       
          // intersect the selected planes
       if (cnt_imgs > threshold ) {
              T res=0;
              vgl_infinite_line_3d<T> l;
              bool good = vgl_intersection(fit_planes, ws, l, res);
              if (good) {
                  if (res<min_res) {
                      min_res=res;
                      line=l;
                      //vcl_cout<<" "<<line.direction();
                      set=true;
                  }
              }
          
          }
       //vcl_cout<<"\n";
      }
  }
  residual=min_res;
 return set;
}


//  unsigned int num_planes = planes.size();
//  vcl_list<vgl_plane_3d<T> > fit_planes;
//  T min_res=T(1e10);
//  line=vgl_infinite_line_3d<T>(vgl_vector_2d<T>(0,0),vgl_vector_3d<T>(1,1,1));
//  vcl_vector<T> ws;
//  bool set=false;
//  for (unsigned iter=0; iter<ITER_MAX; iter++) {
//      fit_planes.clear();
//      ws.clear();
//      // select two planes randomly
//      int index1 = vcl_rand() % num_planes;
//      int index2 = index1;
//      while(index2==index1)
//      {
//          index2=vcl_rand() % num_planes;
//      }
//      vgl_plane_3d<T> plane1 = planes[index1];
//      vgl_plane_3d<T> plane2 = planes[index2];
//
//      // intersect them to get a line
//      vgl_infinite_line_3d<T> inters_line;
//      if (vgl_intersection(plane1, plane2, inters_line)) {
//          vgl_vector_3d<T> line_dir = inters_line.direction();
//
//          // test the line on each plane
//          T weight_sum=0;
//          T sub_weight_sum=0;
//          for (unsigned i=0; i<num_planes; ++i) {
//              vgl_plane_3d<T> plane = planes[i];
//              vgl_vector_3d<T> normal = plane.normal();
//              // see if the line direction and plane normal is perpendicular
//              T res = dot_product(normal,line_dir);
//              if (res < 0.01)  {
//                  vgl_point_3d<T> p=inters_line.point();
//                  if (plane.contains(p,0.01f)) {
//                      fit_planes.push_back(plane);
//                      ws.push_back(weights[i]);
//                      sub_weight_sum+=weights[i];
//                  }
//              }
//              weight_sum+=weights[i];
//          }
//          if(sub_weight_sum>0)
//          {
//              if(sub_weight_sum/weight_sum >0.4)
//              {
//          // intersect the selected planes
////          if (fit_planes.size() > threshold ) {
//              T res=0;
//              vgl_infinite_line_3d<T> l;
//              bool good = vgl_intersection(fit_planes, ws, l, res);
//              if (good) {
//                  if (res<min_res) {
//                      min_res=res;
//                      line=l;
//                      set=true;
//                  }
//              }
//          }
//          }
//      }
//  }
//  //vcl_cout<<".\n";
//  residual=min_res;
//  return set;
//}

#endif
