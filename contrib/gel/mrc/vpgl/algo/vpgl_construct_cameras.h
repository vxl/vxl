// This is brcv/mvg/algo/dvpgl_construct_cameras.h
#ifndef _dvpgl_construct_cameras_h_
#define _dvpgl_construct_cameras_h_
//:
// \file
// \brief This class creates a pair of cameras from a set of corresponding points from a pair of images.
// \author Vishal Jain, (vj@lems.brown.edu)
// \date 08/31/04
//
// The first camera is assumed to be identity and intrinsic parameters are assumed to be
// K=[2000 0 512
//    0 2000 384
//    0    0   1]
//
// This class needs to be rewritten in terms of dvpgl data structures

#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_vector.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vnl/vnl_double_3x3.h>
#include <mvl/FMatrix.h>
#include <mvl/PMatrix.h>

class dvpgl_construct_cameras{

public:
    //: constructor
    dvpgl_construct_cameras();
    //: constructor initializing the set of points
    dvpgl_construct_cameras(vcl_vector<vgl_point_2d<double> > p0,vcl_vector<vgl_point_2d<double> > p1);
    ~dvpgl_construct_cameras();

    //: fuunction to construct cameras
    bool construct();

    //: returns the projection matrix for the cameras
    PMatrix get_camera1(){return P1_;}
    PMatrix get_camera2(){return P2_;}

private: 
    //: set of points
    vcl_vector<vgl_point_2d<double> > points0_;
    vcl_vector<vgl_point_2d<double> > points1_;

    FMatrix F;
    vnl_double_3x3 K;
    vnl_double_3x3 E;
    PMatrix P1_,P2_;

};

#endif
