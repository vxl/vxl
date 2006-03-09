// This is brcv/mvg/algo/dvpgl_construct_cameras.cxx

//:
// \file

#include "dvpgl_construct_cameras.h"
#include <mvl/FMatrixComputeLinear.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <bmvl/brct/brct_algos.h>

//: constructor
dvpgl_construct_cameras::dvpgl_construct_cameras()
{
}

//: contructor with initialization of correponding points
dvpgl_construct_cameras::dvpgl_construct_cameras(vcl_vector<vgl_point_2d<double> > p0,vcl_vector<vgl_point_2d<double> > p1)
{
    points0_=p0;
    points1_=p1;


    K[0][0]=2000;K[0][1]=0;K[0][2]=-512;
    K[1][0]=0;K[1][1]=2000;K[1][2]=384;
    K[2][0]=0;K[2][1]=0;K[2][2]=1;
}
dvpgl_construct_cameras::~dvpgl_construct_cameras()
{
}

//: to construct the cameras according to the correspondence given
bool dvpgl_construct_cameras::construct()
{
    int ymax=768;
    //int xmax=1024;

    vcl_vector<vgl_homg_point_2d<double> > p0,p1;

    for(unsigned int i=0;i<points0_.size();i++)
    {
        vgl_homg_point_2d<double> p(points0_[i]);
        p0.push_back(p);
    }
    for(unsigned int i=0;i<points1_.size();i++)
    {
        vgl_homg_point_2d<double> p(points1_[i]);
        p1.push_back(p);
    }

    FMatrixComputeLinear compF;
    compF.compute(p0,p1,F);
    vnl_double_3x3 Fm,E;

    Fm=F.get_matrix();
  
    vnl_double_3x3  Kt,Kinv;
    Kt=K.transpose();
    Kinv=vnl_inverse(K); 

    //: computing the essential matrix
    //: if K is unknown, it is assumed to be identity matrix
    E=Kt*Fm*K;
    vnl_double_3x3 U, V, W;

    W[0][0]=0;W[0][1]=-1;W[0][2]=0;
    W[1][0]=1;W[1][1]=0;W[1][2]=0;
    W[2][0]=0;W[2][1]=0;W[2][2]=1;
    
    vnl_svd<double> SVD(E);
    U=SVD.U();
    V=SVD.V();

    
    vnl_double_3x4 P1,P2;

    P1[0][0] = 1;     P1[0][1] = 0;      P1[0][2] = 0;        P1[0][3] = 0;
    P1[1][0] = 0;     P1[1][1] = 1;      P1[1][2] = 0;        P1[1][3] = 0;
    P1[2][0] = 0;     P1[2][1] = 0;      P1[2][2] = 1;        P1[2][3] = 0;

    P1_.set(P1);
  
    vnl_vector<double> point2d1(3);    vnl_vector<double> point2d2(3);
    point2d1[0]=points0_[0].x();       point2d2[0]=points1_[0].x();
    point2d1[1]=ymax-points0_[0].y();  point2d2[1]=ymax-points1_[0].y();
    point2d1[2]=1;                     point2d2[2]=1;


    point2d1=Kinv*point2d1;
    point2d2=Kinv*point2d2;
    

    //: nonhomogenize the points
    vgl_point_2d<double> pnorm1(point2d1[0]/point2d1[2],point2d1[1]/point2d1[2]);
    vgl_point_2d<double> pnorm2(point2d2[0]/point2d2[2],point2d2[1]/point2d2[2]);

    //: checking for the correct combination of cameras
    //case 1
    {
        vnl_double_3x3 R;
        R=U*W.transpose()*V.transpose();
        vnl_vector<double> t=U.get_column(2);
        PMatrix P(R,t);
        vgl_point_3d<double> p3d=brct_algos::triangulate_3d_point(pnorm1,P1,
                                                                  pnorm2,P.get_matrix());
         vgl_homg_point_3d<double> ph3d(p3d);
         if(!P1_.is_behind_camera(ph3d) && !P.is_behind_camera(ph3d))
            P2_=P;
    }
    //case 2
    {
        vnl_double_3x3 R;
        R=U*W*V.transpose();
        vnl_vector<double> t=U.get_column(2);
        PMatrix P(R,t);

        vgl_point_3d<double> p3d=brct_algos::triangulate_3d_point(pnorm1,P1,
                                                                  pnorm2,P.get_matrix());
         vgl_homg_point_3d<double> ph3d(p3d);

        if(!P1_.is_behind_camera(ph3d) && !P.is_behind_camera(ph3d))
            P2_=P;
            
    }
    //case 3
    {
        vnl_double_3x3 R;
        R=U*W.transpose()*V.transpose();
        vnl_vector<double> t=-U.get_column(2);
        PMatrix P(R,t);
        vgl_point_3d<double> p3d=brct_algos::triangulate_3d_point(pnorm1,P1,
                                                                  pnorm2,P.get_matrix());
         vgl_homg_point_3d<double> ph3d(p3d);
        if(!P1_.is_behind_camera(ph3d) && !P.is_behind_camera(ph3d))
            P2_=P;
    }
    //case 4
    {
        vnl_double_3x3 R;
        R=U*W*V.transpose();
        vnl_vector<double> t=-U.get_column(2);
        PMatrix P(R,t);
         vgl_point_3d<double> p3d=brct_algos::triangulate_3d_point(pnorm1,P1,
                                                                  pnorm2,P.get_matrix());
         vgl_homg_point_3d<double> ph3d(p3d);
         if(!P1_.is_behind_camera(ph3d) && !P.is_behind_camera(ph3d))
            P2_=P;
    }

    double scale_=10;
   
    vgl_point_3d<double> p3d=brct_algos::triangulate_3d_point(pnorm1,P1,pnorm2,P2_.get_matrix());
    vnl_double_3x4 Ptemp2=P2_.get_matrix();

    Ptemp2[0][3]=scale_*Ptemp2[0][3];
    Ptemp2[1][3]=scale_*Ptemp2[1][3];
    Ptemp2[2][3]=scale_*Ptemp2[2][3];

    P2_.set(Ptemp2);

    p3d=brct_algos::triangulate_3d_point(pnorm1,P1_.get_matrix(),pnorm2,P2_.get_matrix());

    return true;
}
