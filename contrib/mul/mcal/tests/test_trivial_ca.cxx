// This is mul/mcal/tests/test_trivial_ca.cxx
//=======================================================================
//
//  Copyright: (C) 2007 The University of Manchester
//
//=======================================================================
#include <iostream>
#include <sstream>
#include <cmath>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mcal_trivial_ca

#include <vsl/vsl_binary_loader.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mcal/mcal_add_all_loaders.h>
#include <mcal/mcal_trivial_ca.h>

//=======================================================================

//: Run test with ns samples in nd dimensions
void test_trivial_ca1(unsigned nd, unsigned ns)
{
  // Generate some basis vectors
  vnl_vector<double> mean(nd),v1(nd),v2(nd),v3(nd);
  for (unsigned i=0;i<nd;++i)
  {
    mean[i] = i;
    v1[i] = std::cos(i*0.17);
    v2[i] = std::cos(i*0.37+2);
    v3[i] = std::cos(i*0.47+1.2);
  }
  v1.normalize();

  // Force orthogonality
  v2-=(v1*dot_product(v1,v2));
  v2.normalize();

  v3-=(v1*dot_product(v3,v1));
  v3-=(v2*dot_product(v3,v2));
  v3.normalize();

  std::vector<vnl_vector<double> > data(ns);
  for (unsigned i=0;i<ns;++i)
  {
    double A = vnl_math::twopi/ns*i;
    // Third term introduces small "noise"
    data[i] = mean + v1*(30*std::cos(A)) + v2*(10*std::sin(A)) + v3*(0.01*std::sin(3*A+0.1));
  }

  vnl_matrix<double> EVecs;
  vnl_vector<double> evals,m;

  mcal_trivial_ca trivial_ca;
  trivial_ca.build_from_array(&data[0],ns,m,EVecs,evals);

  std::cout<<"evals: "<<evals<<std::endl;
  TEST("N modes",evals.size(),nd);

  TEST_NEAR("Error on Mean",(m-mean).rms(),0,1e-6);
}

void test_trivial_ca()
{
  std::cout << "*************************\n"
           << " Testing mcal_trivial_ca\n"
           << "*************************\n";

  vsl_add_to_binary_loader(mcal_trivial_ca());

  std::cout<<"== Run test with more examples than dimensions =="<<std::endl;
  test_trivial_ca1(7,16);
  std::cout<<"== Run test with fewer examples than dimensions =="<<std::endl;
  test_trivial_ca1(15,4);

  vsl_delete_all_loaders();

  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------
  {
    mbl_cloneables_factory<mcal_component_analyzer>::add(mcal_trivial_ca());

    std::istringstream ss(
          "mcal_trivial_ca\n"
          "{\n"
          "}\n");

    std::unique_ptr<mcal_component_analyzer>
            ca = mcal_component_analyzer::create_from_stream(ss);

    TEST("Correct component analyzer",ca->is_a(),"mcal_trivial_ca");
    vsl_delete_all_loaders();
  }
}

TESTMAIN(test_trivial_ca);
