// This is mul/vimt/tests/test_image_2d_of.cxx
#include <testlib/testlib_test.h>
#include <vxl_config.h> // for vxl_byte
#include <vimt/vimt_image_2d_of.h>

bool Equal(const vimt_image_2d_of<vxl_byte>& im0,
           const vimt_image_2d_of<vxl_byte>& im1)
{
  return im0.image().nplanes()==im1.image().nplanes()
      && im0.image().ni() == im1.image().ni()
      && im0.image().nj() == im1.image().nj()
      && im0.image()(0,0,0) == im1.image()(0,0,0)
      && im0.image()(1,1,1) == im1.image()(1,1,1)
      && im0.world2im() == im1.world2im();
}


void test_image_2d_byte_io()
{
  // -------- Test the binary I/O --------
  vimt_image_2d_of<vxl_byte> image_out0;
  vimt_image_2d_of<vxl_byte> image_out1;
  image_out0.image().resize(5,6,2);
  for (int p=0;p<2;++p)
    for (int j=0;j<6;++j)
      for (int i=0;i<5;++i)
        image_out0.image()(i,j,p)=i+10*j+100*p;

  image_out1 = image_out0;

  vsl_b_ofstream bfs_out("test_image_2d_of.bvl.tmp");
  TEST ("Created test_image_2d_of.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, image_out0);
  vsl_b_write(bfs_out, image_out1);
  bfs_out.close();

  vimt_image_2d_of<vxl_byte> image_in0,image_in1;

  vsl_b_ifstream bfs_in("test_image_2d_of.bvl.tmp");
  TEST ("Opened test_image_2d_of.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, image_in0);
  vsl_b_read(bfs_in, image_in1);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST("Binary IO", Equal(image_in0,image_out0),true);
  TEST("Binary IO (2)", Equal(image_in1,image_out1),true);

  // Check that image_in0 and 1 refer to same data block
  image_in0.image()(3,4,1)=17;
  TEST("Data connected correctly",image_in1.image()(3,4,1),17);
}

MAIN( test_image_2d_of )
{
  START( "vimt_image_2d_of" );
  test_image_2d_byte_io();

  SUMMARY();
}
