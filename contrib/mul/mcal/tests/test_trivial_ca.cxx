// This is mul/mcal/tests/test_trivial_ca.cxx
//=======================================================================
//
//  Copyright: (C) 2007 The University of Manchester
//
//=======================================================================
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mcal_trivial_ca

#include <vcl_iostream.h>
// not used? #include <vcl_string.h>
#include <vsl/vsl_binary_loader.h>
#include <vcl_cmath.h>
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
    v1[i] = vcl_cos(i*0.17);
    v2[i] = vcl_cos(i*0.37+2);
    v3[i] = vcl_cos(i*0.47+1.2);
  }
  v1.normalize();

  // Force orthogonality
  v2-=(v1*dot_product(v1,v2));
  v2.normalize();

  v3-=(v1*dot_product(v3,v1));
  v3-=(v2*dot_product(v3,v2));
  v3.normalize();

  vcl_vector<vnl_vector<double> > data(ns);
  for (unsigned i=0;i<ns;++i)
  {
    double A = i*(2.0*vnl_math::pi/ns);
    // Third term introduces small "noise"
    data[i] = mean + v1*(30*vcl_cos(A)) + v2*(10*vcl_sin(A)) + v3*(0.01*vcl_sin(3*A+0.1));
  }

  vnl_matrix<double> EVecs;
  vnl_vector<double> evals,m;

  mcal_trivial_ca trivial_ca;
  trivial_ca.build_from_array(&data[0],ns,m,EVecs,evals);

  vcl_cout<<"evals: "<<evals<<vcl_endl;
  TEST("N modes",evals.size(),nd);

  TEST_NEAR("Error on Mean",(m-mean).rms(),0,1e-6);
}

void test_trivial_ca()
{
  vcl_cout << "*************************\n"
           << " Testing mcal_trivial_ca\n"
           << "*************************\n";

  vsl_add_to_binary_loader(mcal_trivial_ca());

  vcl_cout<<"== Run test with more examples than dimensions =="<<vcl_endl;
  test_trivial_ca1(7,16);
  vcl_cout<<"== Run test with fewer examples than dimensions =="<<vcl_endl;
  test_trivial_ca1(15,4);

  vsl_delete_all_loaders();

  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------
  {
    mbl_cloneables_factory<mcal_component_analyzer>::add(mcal_trivial_ca());

    vcl_istringstream ss(
          "mcal_trivial_ca\n"
          "{\n"
          "}\n");

    vcl_auto_ptr<mcal_component_analyzer>
            ca = mcal_component_analyzer::create_from_stream(ss);

    TEST("Correct component analyzer",ca->is_a(),"mcal_trivial_ca");
    vsl_delete_all_loaders();
  }
}

TESTMAIN(test_trivial_ca);
