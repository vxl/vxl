// This is mul/mil3d/tests/test_image_3d_of.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <mil3d/mil3d_image_3d_of.h>
#include <vxl_config.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

bool deepEquality(const mil3d_image_3d_of<vxl_byte>& im0,
                  const mil3d_image_3d_of<vxl_byte>& im1)
{
  bool ret=false;

  if (im0.world2im()==im1.world2im() && im0.n_planes()==im1.n_planes()
      && im0.nx() == im1.nx()
      && im0.ny() == im1.ny()
      && im0.nz() == im1.nz())
  {
    bool ok=true;
    for (int i=0;i<im0.nx();++i)
      for (int j=0;j<im0.ny();++j)
        for (int k=0;k<im0.nz();++k)
          if (im0(i,j,k)!=im1(i,j,k))
            ok=false;
    ret=ok;
  }

  return ret;
}

void test_image_3d_of_byte()
{
  vcl_cout << "***************************\n"
           << " Testing mil3d_image_3d_of\n"
           << "***************************\n";

  mil3d_image_3d_of<vxl_byte> image(2,2,2);
  mil3d_transform_3d t;
  t.set_zoom_only(1,2,3,0,0,0);
  image.print_all(vcl_cout);
  image(1,1,1)=10;
  image.setWorld2im(t);

  // gets range
  int val=100;
  vxl_byte a,b;
  image.fill(val);
  image.getRange(a,b,0);
  TEST("Filling with 100 and checking range: ",a==val&&b==val,true);

  // set up image
  image(0,0,0)=0;
  image(0,0,1)=1;
  image(0,1,0)=2;
  image(0,1,1)=3;
  image(1,0,0)=4;
  image(1,0,1)=5;
  image(1,1,0)=6;
  image(1,1,1)=7;

  mil3d_image_3d_of<vxl_byte> image2;
  image2.deepCopy(image);
  image.print_all(vcl_cout);
  mil_image_2d_of<vxl_byte> slice;

  // check slicing
  image.deepSlice(mil3d_image_3d_of<vxl_byte>::XAXIS,0,slice);
  bool data_ok = slice(0,0)==0 && slice(0,1)==1 && slice(1,0)==2 && slice(1,1)==3;
  TEST("Extracting a slice from the X axis: ",data_ok,true);

  image.deepSlice(mil3d_image_3d_of<vxl_byte>::YAXIS,0,slice);
  data_ok = slice(0,0)==0 && slice(0,1)==1 && slice(1,0)==4 && slice(1,1)==5;
  TEST("Extracting a slice from the Y axis: ",data_ok,true);

  image.deepSlice(mil3d_image_3d_of<vxl_byte>::ZAXIS,0,slice);
  data_ok = slice(0,0)==0 && slice(0,1)==2 && slice(1,0)==4 && slice(1,1)==6;
  TEST("Extracting a slice from the Z axis: ",data_ok,true);

  // loading and saving
  mil3d_image_3d_of<vxl_byte> im_in;

  vsl_b_ofstream bfs_out("test_image_3d_of.bvl.tmp");
  TEST("Opened test_image_3d_of.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, image);
  TEST("Finished writing file successfully", (!bfs_out), false);
  bfs_out.close();

  vsl_b_ifstream bfs_in("test_image_3d_of.bvl.tmp");
  TEST("Opened test_image_3d_of.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, im_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink("test_image_3d_of.bvl.tmp");
#endif

  TEST("Image IO deep equality", deepEquality(image,im_in),true);
}


void test_image_3d_of()
{
  test_image_3d_of_byte();
}


TESTLIB_DEFINE_MAIN(test_image_3d_of);
