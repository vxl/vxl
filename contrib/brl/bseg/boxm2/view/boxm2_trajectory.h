// This is brl/bseg/boxm2/view/boxm2_trajectory.h
#ifndef boxm2_trajectory_h_
#define boxm2_trajectory_h_
//:
// \file
// \brief A camera iterator that provides a smooth trajectory around a volume
// \author Andrew Miller
// \date March 2, 2011

// Given a start incline, end incline, and a radius, this class will provide
// a camera iterator that provides a smooth trajectory around a volume.  
// For example this will be used to render a smooth pass around a volume, and 
// turned into a video (for instance with mencoder)
// %% mencoder "mf://*.png" -mf type=png:fps=18 -ovc lavc -o output.avi 
//
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_perspective_camera.h>

//smart ptr includes
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>


class boxm2_trajectory : public vbl_ref_count
{
 public:

    //: construct camera vector from incline0, incline1, radius, bounding box and image size (ni,nj)
    boxm2_trajectory(double incline0, double incline1, double radius, vgl_box_3d<double> bb, unsigned ni, unsigned nj) { 
      init_cameras(incline0, incline1, radius, bb, ni, nj); 
    }
    virtual ~boxm2_trajectory() {}

    //: number of cameras in the trajectory
    unsigned size() { return cams_.size(); }

    //: Iterator
    typedef vcl_vector<vpgl_camera_double_sptr>::iterator iterator;
    iterator begin() { return cams_.begin(); }
    iterator end() { return cams_.end(); }
    
    //: next method
    vpgl_camera_double_sptr next() { 
      if( iter_ == cams_.end() ) iter_ = cams_.begin(); 
      return (*iter_++); 
    }

 protected:

    //list of cams
    vcl_vector<vpgl_camera_double_sptr> cams_; 
    
    //next cam
     vcl_vector<vpgl_camera_double_sptr>::iterator iter_; 
    
    //: initialize cameras
    void init_cameras(double incline0, double incline1, double radius, vgl_box_3d<double> bb, unsigned ni, unsigned nj); 

};

//: Smart_Pointer typedef for boxm2_data_base
typedef vbl_smart_ptr<boxm2_trajectory> boxm2_trajectory_sptr;

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_trajectory const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_trajectory* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_trajectory_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, boxm2_trajectory_sptr const& sptr);

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory* p);
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory_sptr const& sptr);

#endif // boxm2_trajectory_h_

