#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>

#include <brec/brec_part_base.h>
#include <brec/brec_part_base_sptr.h>
#include <brec/brec_part_hierarchy.h>
#include <brec/brec_part_hierarchy_sptr.h>
#include <brec/brec_hierarchy_edge.h>
#include <brec/brec_hierarchy_edge_sptr.h>
#include <brec/brec_part_hierarchy_builder.h>
#include <brec/brec_part_gaussian.h>

#include <brip/brip_vil_float_ops.h>
#include <bsta/algo/bsta_gaussian_updater.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_brec_hierarchy_edge()
{
  brec_part_gaussian_sptr p0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, -45.0f, false, 0);
  brec_part_gaussian_sptr p1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 3.0f, 1.5f, 45.0f, false, 1);

  brec_hierarchy_edge_sptr e = new brec_hierarchy_edge(p0->cast_to_base(), p1->cast_to_base(), false);

  vnl_vector_fixed<float,2> c(111.0f,119.0f); // center measured from the image
  vnl_vector_fixed<float,2> p(107.0f,123.0f);
  vnl_vector_fixed<float,2> sample = p - c;


  float a, d;
  e->calculate_dist_angle(p0->cast_to_instance(), sample, d, a);
  std::cout << "a: " << a << " d: " << d << std::endl;
}

TESTMAIN( test_brec_hierarchy_edge );
