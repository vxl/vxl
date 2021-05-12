// This is core/vil/tests/test_flatten.cxx

#include <iostream>
#include <vector>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vil/vil_flatten.h"


template <class T>
void view_cout(const vil_image_view<T>& view)
{
  for (unsigned p = 0; p < view.nplanes(); ++p)
  {
    std::cout << "Plane " << p << "\n";
    for (unsigned j = 0; j < view.nj(); ++j)
    {
      for (unsigned i = 0; i < view.ni(); ++i)
      {
        std::cout << view(i, j, p) << ' ';
      }
      std::cout << '\n';
    }
  }
}

template <class T>
void vector_cout(std::string name, const std::vector<T>& vec)
{
  std::cout << name << "\n";
  for (auto item : vec)
    std::cout << item << " ";
  std::cout << std::endl;
}


template <class T>
static void
_test_flatten(std::string type_name)
{
  std::cout << "************************************************\n"
            << " test_flatten vil_image_view<" << type_name << ">\n"
            << "************************************************\n";

  // Create a test image (2 columns, 4 rows, 3 planes)
  // plane0 [ 0, 1, 2, 3]
  //        [ 4, 5, 6, 7]
  // plane1 [ 8, 9,10,11]
  //        [12,13,14,15]
  // plane2 [16,17,18,19]
  //        [20,21,22,23]

  unsigned ni = 4, nj = 2, nplanes = 3;
  vil_image_view<T> img(ni, nj, nplanes);
  T val = T(0);

  for (unsigned p = 0; p < nplanes; p++) // plane
  {
    for (unsigned j = 0; j < nj; ++j)  // row
    {
      for (unsigned i = 0; i < ni; ++i)  // column
      {
        img(i, j, p) = T(val);
        val++;
      }
    }
  }
  view_cout(img);

  // test row-major flattening
  // [ (plane0, row0, col0), (p0, r0, c1), (p0, r0, c2) ... ]
  std::vector<T> row_truth = { 0, 1 ,2, 3,
                               4, 5, 6, 7,
                               8, 9,10,11,
                              12,13,14,15,
                              16,17,18,19,
                              20,21,22,23};
  std::vector<T> row = vil_flatten_row_major(img);

  vector_cout("flattened...", row);
  vector_cout("expected....", row_truth);
  TEST("vil_flatten_row_major()", row, row_truth);

  // test column-major flattening
  // [ (plane0, row0, col0), (p0, r1, c0), (p0, r2, c0) ... ]
  std::vector<T> col_truth = { 0, 4, 1, 5, 2, 6, 3, 7,
                               8,12, 9,13,10,14,11,15,
                              16,20,17,21,18,22,19,23};
  std::vector<T> col = vil_flatten_column_major(img);

  vector_cout("flattened...", col);
  vector_cout("expected....", col_truth);
  TEST("vil_flatten_column_major()", col, col_truth);
}

static void
test_flatten()
{
  _test_flatten<vxl_byte>("vxl_byte");
  _test_flatten<int>("int");
  _test_flatten<float>("float");
  _test_flatten<double>("double");
}

TESTMAIN(test_flatten);
