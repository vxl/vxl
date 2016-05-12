//:
// \file
// \brief  A test to verify that findind cells within a region is correct
// \author Isabel Restrepo
// \date 13-Aug-2010

#include <testlib/testlib_test.h>

#include "test_utils.h"

void test_region_finder()
{
  clean_up();
  //create scene
  boxm_scene<boct_tree<short, float> > *scene = create_scene();

  bool result = true;

  if(!scene) result=false;

  //find a region

  {
    vgl_point_3d<double> max_point(14.0, 14.0, 14.0);
    vgl_point_3d<double> min_point(6.0, 6.0, 6.0);
    vgl_box_3d<double> roi(min_point, max_point);

    std::vector<boct_tree_cell<short, float>* > cells;
    scene->leaves_in_region(roi,cells);


    if(cells.size()!=64){
      result=false;
      std::cerr << "Number of cells in the region: " << cells.size() <<std::endl;
    }

    for(unsigned i =0; i<cells.size(); i++)
      if(std::abs(cells[i]->data()- 0.8f)>1.0e-7)
      {
        result = false;
        std::cerr << cells[i]->data()<<std::endl;
      }

    cells.clear();
    scene->unload_active_blocks();

  }
#ifdef DEBUG_LEAKS
  std::cerr << "Leaks Region1: " << boct_tree_cell<short, float >::nleaks() << std::endl;
#endif

  //find another region
  {
    vgl_point_3d<double> max_point(19.0, 19.0, 19.0);
    vgl_point_3d<double> min_point(6.0, 6.0, 6.0);
    vgl_box_3d<double> roi(min_point, max_point);

    std::vector<boct_tree_cell<short, float>* > cells;
    scene->leaves_in_region(roi,cells);

    if(cells.size()!=83){
      result=false;
      std::cerr << "Number of cells in the region: " << cells.size() <<std::endl;
    }

    cells.clear();
    scene->unload_active_blocks();
  }
#ifdef DEBUG_LEAKS
  std::cerr << "Leaks Region2: " << boct_tree_cell<short, float >::nleaks() << std::endl;
#endif

  if(!scene) result=false;

  TEST("Correct region found", result, true);
  clean_up();
}


TESTMAIN(test_region_finder);
