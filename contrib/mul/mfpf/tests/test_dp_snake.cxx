// This is mul/mfpf/tests/test_dp_snake.cxx
//=======================================================================
//
//  Copyright: (C) 2007 The University of Manchester
//
//=======================================================================
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mfpf_dp_snake

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mfpf/mfpf_dp_snake.h>
#include <mfpf/mfpf_edge_finder.h>
#include <vgl/vgl_point_2d.h>
#include <vil/algo/vil_gauss_filter.h>

void test_dp_snake()
{
  std::cout << "***********************\n"
           << " Testing mfpf_dp_snake\n"
           << "***********************\n";

  // Create image containing disk
  unsigned ni = 29;
  double c = 0.5*ni;
  double r2 = 7.5*7.5;

  vil_image_view<float> im(ni,ni);
  im.fill(0.0f);
  for (unsigned j=0;j<ni;++j)
    for (unsigned i=0;i<ni;++i)
    {
       double dx=i-c, dy=j-c;
       double dr2 = dx*dx+dy*dy;
       if (dr2<=r2) im(i,j)=100.0f;
    }

  vimt_image_2d_of<float> image;
  vil_gauss_filter_2d(im,image.image(),0.1,3);

  // Set up snake as small offset circle
  mfpf_dp_snake dp_snake;
  mfpf_edge_finder edge_finder;
  edge_finder.set_search_area(7,0);
  dp_snake.set_to_circle(edge_finder,
                         20, vgl_point_2d<double>(11,9),6);

  std::cout<<"Initial Snake: "<<dp_snake<<std::endl;

  for (unsigned i=0;i<10;++i)
  {
    double move = dp_snake.update_step(image);

    std::cout<<i<<") "<<dp_snake<<std::endl
            <<"Mean move = "<<move<<std::endl;
  }

//  dp_snake.search(image);

//  std::cout<<"After search. Snake: "<<dp_snake<<std::endl;
}

TESTMAIN(test_dp_snake);
