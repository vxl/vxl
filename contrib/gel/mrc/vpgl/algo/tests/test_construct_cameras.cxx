
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <dvpgl/algo/dvpgl_construct_cameras.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_3x3.h>

static void test_construct_cameras()
{
#if 0
    //: define cameras
    vnl_double_3x4 P1,P2;

    vnl_double_3x3 K;

    K[0][0]=2000;K[0][1]=0;K[0][2]=-512;
    K[1][0]=0;K[1][1]=2000;K[1][2]=384;
    K[2][0]=0;K[2][1]=0;K[2][2]=1;


    P1[0][0] = 1;     P1[0][1] = 0;      P1[0][2] = 0;        P1[0][3] = 0;
    P1[1][0] = 0;     P1[1][1] = 1;      P1[1][2] = 0;        P1[1][3] = 0;
    P1[2][0] = 0;     P1[2][1] = 0;      P1[2][2] = 1;        P1[2][3] = 0;

    P2[0][0] = 1;     P2[0][1] = 0;      P2[0][2] = 0;        P2[0][3] = 10;
    P2[1][0] = 0;     P2[1][1] = 1;      P2[1][2] = 0;        P2[1][3] = 0;
    P2[2][0] = 0;     P2[2][1] = 0;      P2[2][2] = 1;        P2[2][3] = 0;

    P1=K*P1;
    P2=K*P2;

    vcl_cout<<P1;
    vcl_cout<<"\n";
    // define a cube 
    double x_cen=5;
    double y_cen=0;
    double z_cen=100;


    vcl_vector<vgl_point_2d<double> > pointsl_,pointsr_;
    for(int x=-1;x<=1;)
    {
        for(int y=-1;y<=1;)
        {
            for(int z=-1;z<=1;)
            {
                vnl_double_4 temp(x*5+x_cen,y*5+y_cen,z*5+z_cen,1);
                vnl_double_3 ptemp1=P1*temp;
                vnl_double_3 ptemp2=P2*temp;
                vgl_point_2d<double> p2d_1(ptemp1[0]/ptemp1[2],ptemp1[1]/ptemp1[2]);
                vgl_point_2d<double> p2d_2(ptemp2[0]/ptemp2[2],ptemp2[1]/ptemp2[2]);
                pointsl_.push_back(p2d_1);
                pointsr_.push_back(p2d_2);
                
                z+=2;
            }
            y+=2;
        }
        x+=2;
    }

    
   

    dvpgl_construct_cameras testcase(pointsl_,pointsr_);
    testcase.construct();
    //
    ////: first camera
    vcl_cout<<testcase.get_camera1();
    ////: second camera
    vcl_cout<<testcase.get_camera2();
#endif
}


TESTMAIN(test_construct_cameras);
