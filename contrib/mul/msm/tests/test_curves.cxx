// This is mul/msm/tests/test_curves.cxx
//=======================================================================
//
//  Copyright: (C) 2010 The University of Manchester
//
//=======================================================================
#include <sstream>
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test msm_curves

#include <msm/msm_curve.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

void test_curve()
{
  std::cout << "***********************\n"
           << " Testing msm_curve\n"
           << "***********************\n";

  msm_curve curve(3,9,true,"Fish");
  TEST("N.points",curve.index().size(),7);
  TEST("First",curve.index()[0],3);
  TEST("Last",curve.index()[6],9);

  std::cout<<curve<<std::endl;

  std::istringstream params2("{ name: fred open: false indices: { 2 3 9 } }");
  msm_curve curve2;
  curve2.config_from_stream(params2);
  std::cout<<curve2<<std::endl;
  TEST("Name",curve2.name(),"fred");
  TEST("Open",curve2.open(),false);
  TEST("First",curve2.index()[0],2);
  TEST("Last",curve2.index()[2],9);

  std::istringstream params3("{ name: jim open: true indices: { 4 : 7 11 12 } }");
  msm_curve curve3;
  curve3.config_from_stream(params3);
  std::cout<<curve3<<std::endl;
  TEST("Name",curve3.name(),"jim");
  TEST("Open",curve3.open(),true);
  TEST("[0]",curve3.index()[0],4);
  TEST("[3]",curve3.index()[3],7);
  TEST("[4]",curve3.index()[4],11);
}


void test_curves()
{
  test_curve();

  std::cout << "***********************\n"
           << " Testing msm_curves\n"
           << "***********************\n";

  msm_curves curves1(2,8,true,"curve1");
  TEST("One curve",curves1.size(),1);
  TEST("[0]",curves1[0].index()[0],2);
  TEST("[6]",curves1[0].index()[6],8);

  std::cout<<curves1<<std::endl;

  std::istringstream params2("{ curve: { name: c1 open: true indices: { 2 3 9 } }  curve: { name: c2 open: true indices: { 9 : 11 } } }");

  msm_curves curves2;
  curves2.config_from_stream(params2);
  std::cout<<curves2<<std::endl;
  TEST("Two curves",curves2.size(),2);
  TEST("[0][0]",curves2[0].index()[0],2);
  TEST("[0][2]",curves2[0].index()[2],9);
  TEST("[1][0]",curves2[1].index()[0],9);
  TEST("[1][2]",curves2[1].index()[2],11);

  curves2.write_text_file("test.crvs");
  msm_curves curves3;
  curves3.read_text_file("test.crvs");
  std::cout<<curves3<<std::endl;


}

TESTMAIN(test_curves);
