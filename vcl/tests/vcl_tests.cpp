// vcl_tests.cpp : Run all vcl tests from one app.
//  I think this is prefereable to having many vcl_test_* projects.
//  awf, mar 2000
//

#define main test_vcl_vector
#include "test_vcl_vector.cxx"
#undef main

#define main test_vcl_string
#include "test_vcl_string.cxx"
#undef main

int main(int argc, char* argv[])
{
	printf("Hello World!\n");
  test_vcl_vector();
  test_vcl_string();

	return 0;
}

