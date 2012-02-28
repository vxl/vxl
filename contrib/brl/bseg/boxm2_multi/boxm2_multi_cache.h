#ifndef boxm2_multi_cache_h
#define boxm2_multi_cache_h
//:
// \file
// \brief boxm2_opencl_scene_streamer assists the processor in streaming blocks

#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vpgl/vpgl_generic_camera.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vcl_algorithm.h>

//vgl includes
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_distance.h>

//: Pair class, to help sorting by distance
template<class T>
class boxm2_dist_pair {
  public:
    boxm2_dist_pair(double dist, T dat):dist_(dist), dat_(dat) {}
    double dist_;
    T dat_;
    inline bool operator < (boxm2_dist_pair const& v) const {
      return dist_ < v.dist_;
    }
};

//: boxm2_multi_cache_group - a helper class that groups together
// contiguous blocks across devices.  Essentially this enforces that 
// a group contains just one block per device, in a contiguous manner (an odd number
// of devices might cause a problem).  For instance if there are two devices, blocks
// will be divided between devices a and b:
//  _____ _____ _____ _____
// | [a  |  b] | [a  |  b] |
// |_____|_____|_____|_____|
// | [a  |  b] | [a  |  b] |
// |_____|_____|_____|_____|
//
// Each group is represented by the brackets - these groupings ensure that contiguous 
// chunks of the scene will be ray traced simultaneously 
class boxm2_multi_cache_group
{
  public: 
    boxm2_multi_cache_group() {}
    boxm2_multi_cache_group(vcl_vector<boxm2_block_id> ids):ids_(ids) {}
    void add_block(boxm2_block_id id) { ids_.push_back(id); }
    void add_block(boxm2_block_metadata data) { 
      ids_.push_back(data.id_);
      bboxes_.push_back(data.bbox());
      bbox_.add(data.bbox());
    }

    vcl_vector<int> order_from_cam(vpgl_camera_double_sptr cam)
    {
      vgl_point_3d<double> pt;
      if ( cam->type_name() == "vpgl_generic_camera" ){
        vpgl_generic_camera<double>* gcam = (vpgl_generic_camera<double>*) cam.ptr();
        pt = gcam->max_ray_origin();
      }
      else if ( cam->type_name() == "vpgl_perspective_camera" ){
        vpgl_perspective_camera<double>* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
        pt = pcam->camera_center();
      }
      else {
        vcl_cout<<"get group order doesn't support camera type "<<cam->type_name()<<vcl_endl;
      }
      vcl_vector<boxm2_dist_pair<int> > distances; 
      for (int i=0; i<bboxes_.size(); ++i) 
        distances.push_back(boxm2_dist_pair<int>(vgl_distance(pt, bboxes_[i].centroid()), i));
      vcl_sort(distances.begin(), distances.end());

      //write and return order
      vcl_vector<int> vis_order; 
      for (int i=0; i<distances.size(); ++i)
        vis_order.push_back(distances[i].dat_);
      return vis_order;
    }

    //ids
    vcl_vector<boxm2_block_id> ids_; 

    //bboxes for each block
    vcl_vector<vgl_box_3d<double> > bboxes_;

    //group Bbox
    vgl_box_3d<double> bbox_; 
};

//group IO
vcl_ostream& operator<<(vcl_ostream &s, boxm2_multi_cache_group& grp);

//: boxm2_multi_cache - example realization of abstract cache class
//  multi cache holds an opencl cache for each device
class boxm2_multi_cache: public vbl_ref_count
{
  public:
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  public:
    boxm2_multi_cache( boxm2_scene_sptr              scene,
                       vcl_vector<bocl_device*> &    devices );
    ~boxm2_multi_cache();

    vcl_vector<boxm2_opencl_cache*>& ocl_caches() { return ocl_caches_; }
    vcl_vector<boxm2_scene_sptr>&    sub_scenes() { return sub_scenes_; }
    boxm2_scene_sptr                 get_scene()  { return scene_; }
    vcl_string to_string();

    //clear all OCL caches 
    void clear();

    //get group order
    vcl_vector<boxm2_multi_cache_group> get_vis_groups(vpgl_camera_double_sptr cam);
    vcl_vector<boxm2_multi_cache_group> group_order_from_pt(vgl_point_3d<double> const& pt,
                                                            vgl_box_2d<double> const& camBox); 

    //: return a vector of sub scene pointers in visibility order from this camera
    vcl_vector<boxm2_opencl_cache*> get_vis_order_from_pt(vgl_point_3d<double> const& pt);
    vcl_vector<boxm2_opencl_cache*> get_vis_sub_scenes(vpgl_perspective_camera<double>* cam);
    vcl_vector<boxm2_opencl_cache*> get_vis_sub_scenes(vpgl_generic_camera<double>* cam);
    vcl_vector<boxm2_opencl_cache*> get_vis_sub_scenes(vpgl_camera_double_sptr cam);
  private:

    boxm2_scene_sptr                    scene_;

    //: scene this cache is operating on
    vcl_vector<boxm2_scene_sptr>        sub_scenes_;

    //: list of regular opencl caches
    vcl_vector<boxm2_opencl_cache*>     ocl_caches_;

    //: list of block groups
    vcl_vector<boxm2_multi_cache_group> groups_;
};

vcl_ostream& operator<<(vcl_ostream &s, boxm2_multi_cache& cache);


#endif // boxm2_multi_cache_h
