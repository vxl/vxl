#include <rsdl/rsdl_borgefors.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <rsdl/rsdl_test.h>

bool location_close(const vnl_vector_fixed<double,2>& v1,
                    const vnl_vector_fixed<double,2>& v2)
{
  return rsdl_close(v1.x(), v2.x()) && rsdl_close(v1.y(), v2.y());
}

int
main()
{
  //Create one vertical and one horizontal lines to be stored
  int range = 250;
  vcl_vector<vnl_vector_fixed<double,2> > data;
  for (int i = -range ; i< range; i++)
    data.push_back(vnl_vector_fixed<double,2>(i,0.0));
  for (int j = -range; j< range; j++)
    data.push_back(vnl_vector_fixed<double,2>(0.0,j));

  rsdl_tests_start( vcl_string("Borgefors distance map") );

  vcl_cout << "\nTests a borgefors map without a mask.\n";
  rsdl_borgefors<vnl_vector_fixed<double,2> >
    bg_map1(-range+50, -range+50, 2*range-100, 2*range-100, data.begin(), data.end());
  TEST("in_map (out of boundary) ", (bg_map1.in_map(-range+49,0) == false));
  TEST("in_map (in boundary) ", (bg_map1.in_map(-range+51,-range+51) == true));
  vnl_vector_fixed<double,2> closest_vec(100,0);
  TEST("get_nearest ",(location_close(*(bg_map1.nearest(100,99)),closest_vec)));
  TEST("get_distnace ", rsdl_close(bg_map1.distance(100,99),99));

  rsdl_borgefors<vnl_vector_fixed<double,2> > bg_map2;
  bg_map2.set(-range+50, -range+50, 2*range-100, 2*range-100, data.begin(), data.end());

  TEST("test operator== ", (bg_map2 == bg_map1) );
  bg_map2.reset();
  bg_map1.reset();
  return rsdl_tests_summary();
}
