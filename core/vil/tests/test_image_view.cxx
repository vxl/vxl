#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <testlib/testlib_test.h>
#include <vil2/vil2_image_view.h>
#include <vil/vil_byte.h>

bool Equal(const vil2_image_view<vil_byte>& im0,
           const vil2_image_view<vil_byte>& im1)
{
  return im0.nplanes()==im1.nplanes()
      && im0.nx() == im1.nx()
      && im0.ny() == im1.ny()
      && im0(0,0,0) == im1(0,0,0)
      && im0(1,1,1) == im1(1,1,1);
}


void test_image_view_byte()
{
  vcl_cout << "*******************************\n";
  vcl_cout << " Testing vil2_image_view<byte>\n";
  vcl_cout << "*******************************\n";

  vil2_image_view<vil_byte> image0;
  image0.resize(10,8);
  vcl_cout<<"image0: "<<image0<<vcl_endl;

  TEST("N.Planes",image0.nplanes(),1);
  TEST("resize x",image0.nx(),10);
  TEST("resize y",image0.ny(),8);

  for (int y=0;y<image0.ny();++y)
     for (int x=0;x<image0.nx();++x)
     {
       image0(x,y) = x+y;
     }

  image0.print_all(vcl_cout);

  {
    // Test the shallow copy
    vil2_image_view<vil_byte> image1;
    image1 = image0;

    TEST("Shallow copy (size)",image0.nx()==image1.nx() && image0.ny()==image1.ny()
                        && image0.nplanes()==image1.nplanes(), true);

    image0(4,6)=127;
    TEST("Shallow copy (values)",image1(4,6),image0(4,6));
  }


   vil2_image_view<vil_byte> image2;
  {
    // Check data remains valid if a copy taken
    vil2_image_view<vil_byte> image3;
    image3.resize(4,5,3);
    image3.fill(111);
    image2 = image3;
  }

  TEST("Shallow copy 2",image2.nx()==4
       && image2.ny()==5 && image2.nplanes()==3, true);

  image2(1,1)=17;
  TEST("Data still in scope",image2(3,3),111);
  TEST("Data still in scope",image2(1,1),17);

  vcl_cout<<image2<<vcl_endl;

  {
    // Test the deep copy
    vil2_image_view<vil_byte> image4;
    image4.deep_copy(image0);
    TEST("Deep copy (size)",image0.nx()==image4.nx()
                         && image0.ny()==image4.ny()
                         && image0.nplanes()==image4.nplanes(), true);
    TEST("Deep copy (values)",image4(4,6),image0(4,6));

    vil_byte v46 = image0(4,6);
    image0(4,6)=255;
    TEST("Deep copy (values really separate)",image4(4,6),v46);
  }

  vil2_image_view<vil_byte> image_win;
  image_win.set_to_window(image0,2,4,3,4);
  TEST("set_to_window size",
        image_win.nx()==4 && image_win.ny()==4
        && image_win.nplanes()==image0.nplanes(),true);

  image0(2,3)=222;
  TEST("set_to_window is shallow copy",image_win(0,0),222);

	vcl_cout<<image0.is_a()<<vcl_endl;
  TEST("is_a() specialisation for vil_byte",image0.is_a(),"vil2_image_view<unsigned char>");
}

/*
void test_image_2d_byte_io()
{
  // -------- Test the binary I/O --------
  vil2_image_view<vil_byte> image_out0;
  vil2_image_view<vil_byte> image_out1;
  image_out0.set_nplanes(2);
  image_out0.resize(5,6);
  for (int i=0;i<2;++i)
    for (int y=0;y<6;++y)
      for (int x=0;x<5;++x)
        image_out0(x,y,i)=x+10*y+100*i;

  image_out1 = image_out0;

  vsl_b_ofstream bfs_out("test_image_view.bvl.tmp");
  TEST ("Created test_image_view.bvl.tmp for writing",
             (!bfs_out), false);
  vsl_b_write(bfs_out, image_out0);
  vsl_b_write(bfs_out, image_out1);
  bfs_out.close();

  vil2_image_view<vil_byte> image_in0,image_in1;

  vsl_b_ifstream bfs_in("test_image_view.bvl.tmp");
  TEST ("Opened test_image_view.bvl.tmp for reading",
           (!bfs_in), false);
  vsl_b_read(bfs_in, image_in0);
  vsl_b_read(bfs_in, image_in1);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST("Binary IO", Equal(image_in0,image_out0),true);
  TEST("Binary IO (2)", Equal(image_in1,image_out1),true);

  // Check that image_in0 and 1 refer to same data block
  image_in0(3,4,1)=17;
  TEST("Data connected correctly",image_in1(3,4,1),17);
}
*/
MAIN( test_image_view )
{
  START( "vil2_image_view" );

  test_image_view_byte();
//  test_image_2d_byte_io();
}
