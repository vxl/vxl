// This is mul/mcal/tests/test_general_ca.cxx
//=======================================================================
//
//  Copyright: (C) 2007 The University of Manchester
//
//=======================================================================
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mcal_general_ca

#include <vsl/vsl_binary_loader.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mcal/mcal_add_all_loaders.h>
#include <mcal/mcal_general_ca.h>
#include <mcal/mcal_trivial_ca.h>
#include <mcal/mcal_pca.h>
#include <mcal/mcal_sparse_basis_cost.h>
#include <mcal/mcal_var_basis_cost.h>
#include <vcl_compiler.h>

//=======================================================================

void test_general_ca_nd(unsigned nd, unsigned ns)
{
  std::cout<<"Generating "<<ns<<" samples of "<<nd<<"-D data."<<std::endl;
  vnl_random rand1(4783);

  // Create random projection matrix
  vnl_matrix<double> M(nd,nd);
  for (unsigned j=0;j<nd;++j)
    for (unsigned i=0;i<nd;++i)
      M(i,j)=rand1.drand64(0,1);

  std::vector<vnl_vector<double> > data(ns);
  for (unsigned i=0;i<ns;++i)
  {
    data[i].set_size(nd);
    for (unsigned j=0;j<nd;++j) data[i][j]=rand1.normal64();
    data[i] = M*data[i];
  }

  vnl_matrix<double> modes;
  vnl_vector<double> mean,mode_var;

  mcal_general_ca ca;
  ca.set(mcal_trivial_ca(),mcal_var_basis_cost());
  ca.build_from_array(&data[0],ns,mean,modes,mode_var);

  TEST("Number of modes",mode_var.size(),nd);
  std::cout<<"Mode var: "<<mode_var<<std::endl;

  // Compare with PCA
  mcal_pca pca;
  pca.set_mode_choice(nd,nd,0.999);
  vnl_matrix<double> pca_modes;
  vnl_vector<double> pca_mean,pca_mode_var;
  pca.build_from_array(&data[0],ns,pca_mean,pca_modes,pca_mode_var);

  TEST_NEAR("Mean correct",vnl_vector_ssd(mean,pca_mean),0,1e-4);

  // Sort both mode var vectors, since order of mode_var arbitrary
  std::sort(mode_var.begin(),mode_var.end());
  std::sort(pca_mode_var.begin(),pca_mode_var.end());
  for (unsigned i=0;i<nd;++i)
    TEST_NEAR("Mode var",mode_var[i],pca_mode_var[i],1e-4);
}

void test_general_ca()
{
  std::cout << "*************************\n"
           << " Testing mcal_general_ca\n"
           << "*************************\n";

  vsl_add_to_binary_loader(mcal_general_ca());

  // Generate gaussian data at angle A to x axis
  unsigned ns = 50;
  double A = 0.3;
  double sinA=std::sin(A),cosA=std::cos(A);
  std::vector<vnl_vector<double> > data(ns);
  vnl_random rand1(4783);
  for (unsigned i=0;i<ns;++i)
  {
    data[i].set_size(2);
    double r1 = 5*rand1.normal64();
    double r2 = rand1.normal64();
    data[i][0]=r1*cosA+r2*sinA;
    data[i][1]=r2*cosA-r1*sinA;
  }

  vnl_matrix<double> modes;
  vnl_vector<double> mean,mode_var;

  mcal_general_ca ca;
  ca.set(mcal_trivial_ca(),mcal_var_basis_cost());
  ca.build_from_array(&data[0],ns,mean,modes,mode_var);

  TEST("Number of modes",mode_var.size(),2);
  std::cout<<"Mode var[0]="<<mode_var[0]<<std::endl
          <<"Mode var[1]="<<mode_var[1]<<std::endl

          <<"Modes: "<<modes<<std::endl;

  // Compare with PCA
  mcal_pca pca;
  vnl_matrix<double> pca_modes;
  vnl_vector<double> pca_mean,pca_mode_var;
  pca.build_from_array(&data[0],ns,pca_mean,pca_modes,pca_mode_var);

  std::sort(mode_var.begin(),mode_var.end());
  std::sort(pca_mode_var.begin(),pca_mode_var.end());

  TEST_NEAR("Mean correct",vnl_vector_ssd(mean,pca_mean),0,1e-4);
  TEST_NEAR("Mode 0 var",mode_var[0],pca_mode_var[0],1e-4);
  TEST_NEAR("Mode 1 var",mode_var[1],pca_mode_var[1],1e-4);

  std::cout<<"PCA Modes: "<<pca_modes<<std::endl;

  vsl_delete_all_loaders();


  test_general_ca_nd(3,50);
  test_general_ca_nd(4,50);

  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------
  {
    mbl_cloneables_factory<mcal_component_analyzer>::add(mcal_general_ca());
    mbl_cloneables_factory<mcal_component_analyzer>::add(mcal_trivial_ca());
    mbl_cloneables_factory<mcal_single_basis_cost>::add(mcal_sparse_basis_cost());

    std::istringstream ss(
          "mcal_general_ca\n"
          "{\n"
          "  initial_ca: mcal_trivial_ca { }\n"
          "  basis_cost: mcal_sparse_basis_cost { alpha: 0.1 }\n"
          "}\n");

    std::auto_ptr<mcal_component_analyzer>
            ca = mcal_component_analyzer::create_from_stream(ss);

    TEST("Correct component analyzer",ca->is_a(),"mcal_general_ca");
    if (ca->is_a()=="mcal_general_ca")
    {
      mcal_general_ca &a_ca = static_cast<mcal_general_ca&>(*ca);
      std::cout<<a_ca<<std::endl;
      TEST("initial_ca",a_ca.initial_ca().is_a(),"mcal_trivial_ca");
      TEST("basis_cost",a_ca.basis_cost().is_a(),"mcal_sparse_basis_cost");
    }
    vsl_delete_all_loaders();
  }
}

TESTMAIN(test_general_ca);
