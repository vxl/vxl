// This is mul/mcal/tests/test_pca.cxx
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
// \brief test mcal_pca

#include <vsl/vsl_binary_loader.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mcal/mcal_add_all_loaders.h>
#include <mcal/mcal_pca.h>

//=======================================================================

//: Run test with ns samples in nd dimensions
void test_pca1(unsigned nd, unsigned ns)
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
  for (unsigned i=0; i<ns; ++i)
  {
    double A = vnl_math::twopi/ns*i;
    // Third term introduces small "noise"
    data[i] = mean + v1*(30*std::cos(A)) + v2*(10*std::sin(A)) + v3*(0.01*std::sin(3*A+0.1));
  }

  vnl_matrix<double> EVecs;
  vnl_vector<double> evals,m;

  mcal_pca pca;
  pca.set_var_prop(0.98);
  pca.build_from_array(&data[0],ns,m,EVecs,evals);

  std::cout<<"evals: "<<evals<<std::endl;
  TEST("Two modes",evals.size(),2);

  TEST_NEAR("Error on Mean",(m-mean).rms(),0,1e-6);

  std::cout<<"Check direction of eigenvectors"<<std::endl;
  vnl_vector<double> b1= EVecs.transpose()*v1;
  // Should be [+/-1,0]
  TEST_NEAR("EVec 1",std::fabs(b1[0]),1,1e-5);

  vnl_vector<double> b2= EVecs.transpose()*v2;
  // Should be [0,+/-1]
  TEST_NEAR("EVec 2",std::fabs(b2[1]),1,1e-5);
}

void test_pca()
{
  std::cout << "***********************\n"
           << " Testing mcal_pca\n"
           << "***********************\n";

  vsl_add_to_binary_loader(mcal_pca());

  std::cout<<"== Run test with more examples than dimensions =="<<std::endl;
  test_pca1(7,16);
  std::cout<<"== Run test with fewer examples than dimensions =="<<std::endl;
  test_pca1(15,4);

  vsl_delete_all_loaders();

  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------
  {
    mbl_cloneables_factory<mcal_component_analyzer>::add(mcal_pca());

    std::istringstream ss(
          "mcal_pca\n"
          "{\n"
          "  min_modes: 1\n"
          "  max_modes: 7\n"
          "  var_prop: 0.93\n"
          "  max_d_in_memory: 1.23e7\n"
          "  use_chunks:  true\n"
          "}\n");

    std::unique_ptr<mcal_component_analyzer>
            ca = mcal_component_analyzer::create_from_stream(ss);

    TEST("Correct component analyzer",ca->is_a(),"mcal_pca");
    if (ca->is_a()=="mcal_pca")
    {
      auto &a_ca = static_cast<mcal_pca&>(*ca);
      std::cout<<a_ca<<std::endl;
      TEST("min_modes configured",a_ca.min_modes(),1);
      TEST("max_modes configured",a_ca.max_modes(),7);
      TEST_NEAR("var_prop configured",a_ca.var_prop(),0.93,1e-6);
      TEST_NEAR("max_d configured",a_ca.max_d_in_memory(),1.23e7,10);
      TEST("use_chunks configured",a_ca.use_chunks(),true);
    }
    vsl_delete_all_loaders();
  }
}

TESTMAIN(test_pca);
