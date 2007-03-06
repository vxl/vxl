// This is mul/mcal/tests/test_pca.cxx
//=======================================================================
//
//  Copyright: (C) 2007 The University of Manchester
//
//=======================================================================
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mcal_pca

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vsl/vsl_binary_loader.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <mbl/mbl_cloneables_factory.h>
#if 0
#include <mcal/mcal_add_all_binary_loaders.h>
#endif // 0
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
  double sum0=0,sum1=0;
  for (unsigned i=0;i<ns;++i)
  {
    double A = i*(2.0*vnl_math::pi/ns);
    // Third term introduces small "noise"
    data[i] = mean + v1*(30*vcl_cos(A)) + v2*(10*vcl_sin(A)) + v3*(0.01*vcl_sin(3*A+0.1));
  }

  vnl_matrix<double> EVecs;
  vnl_vector<double> evals,m;

  mcal_pca pca;
  pca.set_var_prop(0.98);
  pca.build_from_array(&data[0],ns,m,EVecs,evals);

  vcl_cout<<"evals: "<<evals<<vcl_endl;
  TEST("Two modes",evals.size(),2);

  TEST_NEAR("Error on Mean",(m-mean).rms(),0,1e-6);

  vcl_cout<<"Check direction of eigenvectors"<<vcl_endl;
  vnl_vector<double> b1= EVecs.transpose()*v1;
  // Should be [+/-1,0]
  TEST_NEAR("EVec 1",vcl_fabs(b1[0]),1,1e-5);

  vnl_vector<double> b2= EVecs.transpose()*v2;
  // Should be [0,+/-1]
  TEST_NEAR("EVec 2",vcl_fabs(b2[1]),1,1e-5);
}

void test_pca()
{
  vcl_cout << "***********************\n"
           << " Testing mcal_pca\n"
           << "***********************\n";

  vsl_add_to_binary_loader(mcal_pca());

  vcl_cout<<"== Run test with more examples than dimensions =="<<vcl_endl;
  test_pca1(7,16);
  vcl_cout<<"== Run test with fewer examples than dimensions =="<<vcl_endl;
  test_pca1(15,4);

  vsl_delete_all_loaders();

  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------
  {
    mbl_cloneables_factory<mcal_component_analyzer>::add(mcal_pca());

    vcl_istringstream ss(
          "mcal_pca\n"
          "{\n"
          "  min_modes: 1\n"
          "  max_modes: 7\n"
          "  var_prop: 0.93\n"
          "  max_d_in_memory: 1.23e7\n"
          "  use_chunks:  true\n"
          "}\n");

    vcl_auto_ptr<mcal_component_analyzer>
            ca = mcal_component_analyzer::new_component_analyzer_from_stream(ss);

    TEST("Correct component analyzer",ca->is_a(),"mcal_pca");
    if (ca->is_a()=="mcal_pca")
    {
      mcal_pca &a_ca = static_cast<mcal_pca&>(*ca);
      vcl_cout<<a_ca<<vcl_endl;
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
