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

class boxm2_trajectory
{
 public:

    //: construct camera vector from incline0, incline1, radius, bounding box and image size (ni,nj)
    boxm2_trajectory(double incline0, double incline1, double radius, vgl_box_3d<double> bb, unsigned ni, unsigned nj) { 
      init_cameras(incline0, incline1, radius, bb, ni, nj); 
    }
    virtual ~boxm2_trajectory() {}

    //: Iterator
    typedef vcl_vector<vpgl_camera_double_sptr>::iterator iterator;
    iterator begin() { return cams_.begin(); }
    iterator end() { return cams_.end(); }

 protected:

    //Current Cam, init cam and current stare point
    vcl_vector<vpgl_camera_double_sptr> cams_; 
    
    //: initialize cameras
    void init_cameras(double incline0, double incline1, double radius, vgl_box_3d<double> bb, unsigned ni, unsigned nj); 

};
#endif // boxm2_trajectory_h_

