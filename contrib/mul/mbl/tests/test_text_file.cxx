#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_save_text_file.h>
#include <mbl/mbl_load_text_file.h>


void test_text_file()
{
  std::cout << "****************************\n"
           << " Testing mbl_test_text_file\n"
           << "****************************" << std::endl;

  //create double vector
  std::vector<double> double_vec(3);
  double_vec[0]= -1.1;
  double_vec[1]= 4.0;
  double_vec[2]= -5.3;

  mbl_save_text_file( double_vec, "double_vec.txt");
  std::vector<double> double_vec_in;
  mbl_load_text_file( double_vec_in, "double_vec.txt");
  TEST( "double_vec io - file", double_vec==double_vec_in, true );

  {
    std::ofstream os("double_vec2.txt");
    mbl_save_text_file( double_vec, os);
    os.close();
    std::ifstream is;
    is.open("double_vec2.txt", std::ifstream::in);
    std::vector<double> double_vec_in2;
    mbl_load_text_file( double_vec_in2, is);
    is.close();

    TEST( "double_vec io - stream", double_vec==double_vec_in2, true );
  }


  //create int vector
  std::vector<int> int_vec(3);
  int_vec[0]= -1;
  int_vec[1]= 4;
  int_vec[2]= -5;

  mbl_save_text_file( int_vec, "int_vec.txt");
  std::vector<int> int_vec_in;
  mbl_load_text_file( int_vec_in, "int_vec.txt");

  TEST( "int_vec io - file", int_vec==int_vec_in, true );

  {
    std::ofstream os("int_vec2.txt");
    mbl_save_text_file( int_vec, os);
    os.close();
    std::ifstream is;
    is.open("int_vec2.txt", std::ifstream::in);
    std::vector<int> int_vec_in2;
    mbl_load_text_file( int_vec_in2, is);
    is.close();

    TEST( "int_vec io - stream", int_vec==int_vec_in2, true );
  }

  //create bool vector
  std::vector<bool> bool_vec(3);
  int_vec[0]= true;
  int_vec[1]= false;
  int_vec[2]= true;

  mbl_save_text_file( bool_vec, "bool_vec.txt");
  std::vector<bool> bool_vec_in;
  mbl_load_text_file( bool_vec_in, "bool_vec.txt");

  TEST( "bool_vec io - file", bool_vec==bool_vec_in, true );

  {
    std::ofstream os("bool_vec2.txt");
    mbl_save_text_file( bool_vec, os);
    os.close();
    std::ifstream is;
    is.open("bool_vec2.txt", std::ifstream::in);
    std::vector<bool> bool_vec_in2;
    mbl_load_text_file( bool_vec_in2, is);
    is.close();

    TEST( "bool_vec io - stream", bool_vec==bool_vec_in2, true );
  }
}

TESTMAIN(test_text_file);
