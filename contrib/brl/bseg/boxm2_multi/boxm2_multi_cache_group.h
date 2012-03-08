#ifndef boxm2_multi_cache_group_h_
#define boxm2_multi_cache_group_h_
//:
// \file
#include "boxm2_multi_cache_group.h"
#include "boxm2_multi_util.h"
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vcl_iosfwd.h>

//: a helper class that groups together contiguous blocks across devices.
// Essentially this enforces that a group contains just one block per device,
// in a contiguous manner (an odd number of devices might cause a problem).
// For instance if there are two devices, blocks will be divided between devices a and b:
// \verbatim
//  _____ _____ _____ _____
// | [a  |  b] | [a  |  b] |
// |_____|_____|_____|_____|
// | [a  |  b] | [a  |  b] |
// |_____|_____|_____|_____|
// \endverbatim
// Each group is represented by the brackets - these groupings ensure that contiguous
// chunks of the scene will be ray traced simultaneously
class boxm2_multi_cache_group
{
  public:
    boxm2_multi_cache_group() {}
    boxm2_multi_cache_group(vcl_vector<boxm2_block_id> ids):ids_(ids) {}

    //: add a block to the group
    void add_block(boxm2_block_metadata data, boxm2_opencl_cache* cache) {
      ids_.push_back(data.id_);
      bboxes_.push_back(data.bbox());
      bbox_.add(data.bbox());
      caches_.push_back(cache);

      //so they are the right size...
      vis_imgs_.push_back(NULL);
      pre_imgs_.push_back(NULL);
    }

    //: visibility order of blocks from camera (returns indices)
    vcl_vector<int> order_from_cam(vpgl_camera_double_sptr cam);

    //: set visibility image, set pre image
    void set_vis(int i, float* vis) { vis_imgs_[i] = vis; }
    void set_pre(int i, float* pre) { pre_imgs_[i] = pre; }
    float* get_vis(int i) { return vis_imgs_[i]; }
    float* get_pre(int i) { return pre_imgs_[i]; }

    //get id
    boxm2_block_id& id(int i) { return ids_[i]; }
    vcl_vector<boxm2_block_id>& ids() { return ids_; };

    //get boxes
    vcl_vector<vgl_box_3d<double> >& bboxes() { return bboxes_; }
    vgl_box_3d<double>& bbox(int i) { return bboxes_[i]; }

    //get full bbox
    vgl_box_3d<double>& groupBox() { return bbox_; }

  private:
    //ids
    vcl_vector<boxm2_block_id> ids_;

    //cache taht each block belongs to
    vcl_vector<boxm2_opencl_cache*> caches_;

    //bboxes for each block
    vcl_vector<vgl_box_3d<double> > bboxes_;

    //group Bbox
    vgl_box_3d<double> bbox_;

    //vis/pre images
    vcl_vector<float*> vis_imgs_;
    vcl_vector<float*> pre_imgs_;
};

//group IO
vcl_ostream& operator<<(vcl_ostream &s, boxm2_multi_cache_group& grp);

#endif // boxm2_multi_cache_group_h_
