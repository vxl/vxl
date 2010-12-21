#include "boxm2_util.h"

#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>

//: fills a float buffer (should be 16*3 floats) with a perspective cam to be sent
void boxm2_util::set_persp_camera(vpgl_camera_double_sptr cam, cl_float* persp_cam)
{
  if (vpgl_proj_camera<double>* pcam =
      dynamic_cast<vpgl_proj_camera<double>*>(cam.ptr()))
  {
    vcl_cout<<"CAM: "<<(*pcam)<<vcl_endl;
    vnl_svd<double>* svd=pcam->svd();
    vnl_matrix<double> Ut=svd->U().conjugate_transpose();
    vnl_matrix<double> V=svd->V();
    vnl_vector<double> Winv=svd->Winverse().diagonal();

    int cnt=0;
    for (unsigned i=0;i<Ut.rows();i++)
    {
      for (unsigned j=0;j<Ut.cols();j++)
        persp_cam[cnt++]=(cl_float)Ut(i,j);

      persp_cam[cnt++]=0;
    }

    for (unsigned i=0;i<V.rows();i++)
      for (unsigned j=0;j<V.cols();j++)
        persp_cam[cnt++]=(cl_float)V(i,j);

    for (unsigned i=0;i<Winv.size();i++)
      persp_cam[cnt++]=(cl_float)Winv(i);

    vgl_point_3d<double> cam_center=pcam->camera_center();
    persp_cam[cnt++]=(cl_float)cam_center.x();
    persp_cam[cnt++]=(cl_float)cam_center.y();
    persp_cam[cnt++]=(cl_float)cam_center.z();
  }
  else {
    vcl_cerr << "Error set_persp_camera() : unsupported camera type\n";
  }
}

//: fills in a 256 char array with number of BITS for each value (255 = 8, 254 = 7 etc)
void boxm2_util::set_bit_lookup(cl_uchar* lookup)
{
  unsigned char bits[] = { 0,   1,   1,   2,   1,   2,   2,   3,   1,   2,   2,   3,   2,   3,   3,   4,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5 ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           4,   5,   5,   6,   5,   6,   6,   7,   5,   6,   6,   7,   6,   7,   7,   8 };
  vcl_memcpy(lookup, bits, 256); 
  //for(int i=0; i<256; i++) lookup[i] = bits[i]; 
}
