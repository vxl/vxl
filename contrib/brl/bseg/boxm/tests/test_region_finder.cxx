//:
// \file
// \brief  A test to verify that findind cells within a region is correct
// \author Isabel Restrepo
// \date 13-Aug-2010

#include <testlib/testlib_test.h>

#include "test_utils.h"

void test_region_finder()
{
  //create scene
  boxm_scene<boct_tree<short, float> > *scene = create_scene();

  //find a region
  vgl_point_3d<double> max_point(14.0, 14.0, 14.0);
  vgl_point_3d<double> min_point(6.0, 6.0, 6.0);
  vgl_box_3d<double> roi(min_point, max_point);

  vcl_vector<boct_tree_cell<short, float>* > cells;
  scene->cells_in_region(roi,cells);

  bool result = true;

  if (cells.size()!=32) {
    result=false;
    vcl_cerr << "Number of cells in the region: " << cells.size() <<vcl_endl;
  }

  for (unsigned i =0; i<cells.size(); i++)
    if (vcl_abs(cells[i]->data()- 0.8)>1.0e-7)
    {
      result = false;
      vcl_cerr << cells[i]->data()<<vcl_endl;
    }

  if (!scene) result=false;
  TEST("Correct region found", result, true);
  clean_up();
}


TESTMAIN(test_region_finder);
