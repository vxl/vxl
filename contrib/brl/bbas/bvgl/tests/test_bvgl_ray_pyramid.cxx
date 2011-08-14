//:
// \file
#include <testlib/testlib_test.h>
#include <bvgl/bvgl_ray_pyramid.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>

//: Test changes
static void test_bvgl_ray_pyramid()
{
  //create all the arguments
  vbl_array_2d<vgl_ray_3d<float> > rays(8,8);
  float ray_d[8*8*4];
  int count = 0;
  for (int j=0; j<8; ++j) {
    for (int i=0; i<8; ++i) {
      rays(j,i) = vgl_ray_3d<float>( vgl_point_3d<float>(0.0f,0.0f,0.0f),
                                     vgl_vector_3d<float>((float)i,(float)j,0.0f) );
      ray_d[count++] = (float)i;
      ray_d[count++] = (float)j;
      ray_d[count++] = 0.0f;
      ray_d[count++] = .5f;
    }
  }

  //create pyramid
  vcl_cout<<"Creating pyramid"<<vcl_endl;
  bvgl_ray_pyramid pyramid(rays, 4);
  vcl_cout<<"created"<<vcl_endl;

  //////////////////////////////////////////////////////////////////////////////
  //calculate ground truth values
  float out1GT[4*4*4];
  int oc=0;
  for (int j=0; j<8; j+=2) {
    for (int i=0; i<8; i+=2) {
      //upper left idx
      int idx = 4*(8*j + i);
      int idx1 = 4*(8*(j+1) + i);
      int idx2 = 4*(8*j + i+1);
      int idx3 = 4*(8*(j+1) + i+1);
      vgl_vector_3d<float> r(ray_d[idx] + ray_d[idx1] + ray_d[idx2] + ray_d[idx3],
                             ray_d[idx+1] + ray_d[idx1+1] + ray_d[idx2+1] + ray_d[idx3+1],
                             ray_d[idx+2] + ray_d[idx1+2] + ray_d[idx2+2] + ray_d[idx3+2] );
      normalize(r);
      out1GT[oc++] = r.x();
      out1GT[oc++] = r.y();
      out1GT[oc++] = r.z();
      out1GT[oc++] = 1.0;
    }
  }

  float out2GT[4*2*2];
  oc=0;
  for (int j=0; j<4; j+=2) {
    for (int i=0; i<4; i+=2) {
      //upper left idx
      int idx = 4*(4*j + i);
      int idx1 = 4*(4*(j+1) + i);
      int idx2 = 4*(4*j + i+1);
      int idx3 = 4*(4*(j+1) + i+1);
      vgl_vector_3d<float> r(out1GT[idx] + out1GT[idx1] + out1GT[idx2] + out1GT[idx3],
                             out1GT[idx+1] + out1GT[idx1+1] + out1GT[idx2+1] + out1GT[idx3+1],
                             out1GT[idx+2] + out1GT[idx1+2] + out1GT[idx2+2] + out1GT[idx3+2] );
      normalize(r);
      out2GT[oc++] = r.x();
      out2GT[oc++] = r.y();
      out2GT[oc++] = r.z();
      out2GT[oc++] = 2.0;
    }
  }

  float out3GT[4*1*1] = {0};
  for (int i=0; i<4*2*2; i+=4) {
    out3GT[0] += out2GT[i+0];
    out3GT[1] += out2GT[i+1];
    out3GT[2] += out2GT[i+2];
    out3GT[3] += out2GT[i+3];
  }
  vgl_vector_3d<float> r(out3GT[0],out3GT[1],out3GT[2]);
  normalize(r);
  out3GT[0] = r.x();
  out3GT[1] = r.y();
  out3GT[2] = r.z();
  out3GT[3] = 4.0f;

#if 0
  unsigned imgdims[4] = {0, 0, 8, 8};
  float out1[4*4*4];
  float out2[2*2*4];
  float out3[1*1*4];

  vcl_cout<<"Out1GT.."<<vcl_endl;
  print_column_major(out1GT, 4, 4, 4);

  vcl_cout<<"Out2GT.."<<vcl_endl;
  print_column_major(out2GT, 2, 2, 4);
  vcl_cout<<"Out2.."<<vcl_endl;
  print_column_major(out2, 2, 2, 4);

  vcl_cout<<"Out3GT.."<<vcl_endl;
  print_column_major(out3GT,1,1,4);
  vcl_cout<<"Out3..."<<vcl_endl;
  print_column_major(out3, 1,1,4);
#endif

  //test 4x4 image
  vbl_array_2d<vgl_ray_3d<float> >& fbf = pyramid(1);
  int idx = 0;
  for (int i=0; i<4; ++i) {
    for (int j=0; j<4; ++j) {
      vgl_ray_3d<float> ray = fbf(i,j);
      if ( vcl_fabs(ray.direction().x() - out1GT[idx] ) > 1e-5f ) {
        vcl_cout<<" img1 No match at "<<i<<','<< j << '\n'
                <<"gt "<<out1GT[idx]<<" calc "<<ray.direction().x()<<vcl_endl;
      }
      idx+=4;
    }
  }

  fbf = pyramid(2);
  idx = 0;
  for (int j=0; j<2; ++j) {
    for (int i=0; i<2; ++i) {
      vgl_ray_3d<float> ray = fbf(i,j);
      if ( vcl_fabs(ray.direction().x() - out2GT[idx] ) > 1e-5f ) {
        vcl_cout<<" img2 No match at "<<i<<','<< j << '\n'
                <<"gt "<<out2GT[idx]<<" calc "<<ray.direction().x()<<vcl_endl;
      }
      idx+=4;
    }
  }
#if 0
  //test 1x1 image
  for (int i=0; i<4*1*1; ++i) {
    if ( vcl_fabs(out3GT[i] - out3[i]) > 1e-6f )
      vcl_cout<<" img3 No match at "<<i<<':'<<out3GT[i]<<" != "<<out3[i]<<vcl_endl;
  }
#endif
}

TESTMAIN( test_bvgl_ray_pyramid );
