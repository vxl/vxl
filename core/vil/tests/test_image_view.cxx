// This is core/vil/tests/test_image_view.cxx
#define VXL_LEGACY_ERROR_REPORTING // REQUIRED FOR PASSING TESTS 2018-11-02
#include <iostream>
#include <sstream>
#include <functional>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_crop.h>
#include <vil/vil_copy.h>
#include <vil/vil_print.h>
#include <vil/vil_plane.h>
#include <vil/vil_convert.h>
#include <vil/vil_transpose.h>
#include <vil/vil_view_as.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_fill.h>
#include <vil/vil_exception.h>

template <class S, class T>
void test_image_view_rgba(vil_image_view<S> & /*image2*/, vil_image_view<T> & /*image7*/)
{
  // do nothing in general case
}

template <>
void test_image_view_rgba(vil_image_view<vxl_byte> &image2, vil_image_view<float> &image7)
{
  image2.set_size(10,10,2);
  vil_image_view<vil_rgba<vxl_byte> > image6;
#if !defined VXL_LEGACY_ERROR_REPORTING
  bool caught_exception = false;
  try
  {
    image6 = image2;
  }
  catch (const vil_exception_pixel_formats_incompatible &e)
  {
    caught_exception = true;
    std::cout << "Exception: " << e.what() << std::endl;
  }
  TEST("Can't assign a 3 plane images to rgba view", caught_exception, true);
#else
  image6 = image2;
  TEST("Can't assign a 3 plane images to rgba view", image6?true:false, false);
#endif

  vil_convert_cast(image7, image6);
  TEST("vil_convert_cast<float, rgba<byte> >", image6?true:false, true);

  image6 = vil_convert_cast(vil_rgba<vxl_byte>(), vil_new_image_view_base_sptr(image7));
  TEST("vil_convert_cast(vil_rgba<vxl_byte>, sptr)", image6?true:false, true);
  vil_print_all(std::cout, image6);

  image2 = vil_plane(vil_view_as_planes(image6),1);
  vil_transform(vil_plane(vil_view_as_planes(image6),1), image2,
                std::bind2nd(std::plus<vxl_byte>(),1));

  vil_print_all(std::cout, image6);
  image7.clear();
  vil_convert_rgb_to_grey(image6, image7);
  TEST("vil_convert_rgb_to_grey(vil_rgba)", image7?true:false, true);

  TEST_NEAR("Conversion rgba to grey", image7(0,0),  0.7154, 1e-5);

  TEST_NEAR("Conversion rgba to grey", image7(2,1),  33.7154, 1e-5);

  vil_print_all(std::cout, image7);
}

template <>
void test_image_view_rgba(vil_image_view<float> &image2, vil_image_view<double> &image7)
{
  image2.set_size(10,10,2);
  vil_image_view<vil_rgba<float> > image6;

#if !defined VXL_LEGACY_ERROR_REPORTING
  bool caught_exception = false;
  try
  {
    image6 = image2;
  }
  catch (const vil_exception_pixel_formats_incompatible &e)
  {
    caught_exception = true;
    std::cout << "Exception: " << e.what() << std::endl;
  }
  TEST("Can't assign a 3 plane images to rgba view", caught_exception, true);
#else
  image6 = image2;
  TEST("Can't assign a 3 plane images to rgba view", image6?true:false, false);
#endif


  vil_convert_cast(image7, image6);
  TEST("vil_convert_cast<double,rgba<float> >", image6?true:false, true);

  image2 = vil_plane(vil_view_as_planes(image6),1);
  vil_transform(vil_plane(vil_view_as_planes(image6),1), image2,
                std::bind2nd(std::plus<float>(),1));

  vil_print_all(std::cout, image6);
  image7.clear();
  vil_convert_rgb_to_grey(image6, image7);
  vil_print_all(std::cout, image7);
  TEST("vil_convert_rgb_to_grey(vil_rgba)", image7?true:false, true);

  TEST_NEAR("Conversion rgba to grey wibble", image7(0,0),  1.3154, 1e-5);

  TEST_NEAR("Conversion rgba to grey", image7(2,1), 34.9114, 1e-5);
}


template <class S, class T>
void test_image_view(S /*d1*/, std::string s_name, T /*d2*/)
{
  vil_image_view<S> image0;
  image0.set_size(10,8);
  std::cout<<"image0: "<<image0<<std::endl;

  TEST("N.Planes", image0.nplanes(), 1);
  TEST("set_size i", image0.ni(), 10);
  TEST("set_size j", image0.nj(), 8);

  for (unsigned int j=0;j<image0.nj();++j)
     for (unsigned int i=0;i<image0.ni();++i)
       image0(i,j) = S(i+j);

  vil_print_all(std::cout, image0);

  {
    // Test the shallow copy
    vil_image_view<S> image1;
    image1 = image0;

    TEST("Shallow copy (size)",
         image0.ni()==image1.ni() &&
         image0.nj()==image1.nj() &&
         image0.nplanes()==image1.nplanes(), true);

    image0(4,6)=127;
    TEST("Shallow copy (values)", image1(4,6), image0(4,6));
  }

  vil_image_view<S> image2;
  {
    // Check data remains valid if a copy taken
    vil_image_view<S> image3;
    image3.set_size(4,5,3);
    image3.fill(111);
    image2 = image3;
  }

  TEST("Shallow copy 2", image2.ni()==4
                      && image2.nj()==5
                      && image2.nplanes()==3, true);

  image2(1,1)=17;
  TEST("Data still in scope", image2(3,3), 111);
  TEST("Data still in scope", image2(1,1), 17);

  std::cout<<image2<<std::endl;

  {
    // Test the deep copy
    vil_image_view<S> image4;
    image4.deep_copy(image0);
    TEST("Deep copy (size)", image0.ni()==image4.ni()
                          && image0.nj()==image4.nj()
                          && image0.nplanes()==image4.nplanes(), true);
    TEST("Deep copy (values)", image4(4,6), image0(4,6));

    S v46 = image0(4,6);
    image0(4,6)=255;
    TEST("Deep copy (values really separate)", image4(4,6), v46);
  }

  vil_image_view<S> image_win;
  image_win = vil_crop(image0,2,4,3,4);
  TEST("vil_crop size",
       image_win.ni()==4 && image_win.nj()==4 && image_win.nplanes()==image0.nplanes(),
       true);

  image0(2,3)=222;
  TEST("vil_crop is shallow copy", image_win(0,0), 222);

  std::cout<<image0.is_a()<<std::endl;
  TEST("is_a() specialisation for S", image0.is_a(), "vil_image_view<"+s_name+">");

  vil_image_view<vil_rgb<S> > image5;
  image5.set_size(5,4);
  image5.fill(vil_rgb<S>(25,35,45));
  image5(2,2).b = 50;

  image2 = image5;
  TEST("Can assign rgb images to 3 plane view", image2?true:false, true);
  TEST("Pixels are correct", image2(2,2,1) == 35 && image2(2,2,2) == 50, true);

  image5 = image2;
  TEST("Can assign 3 planes suitable image to rgb view", image5?true:false, true);


  TEST("Equality", image2, image2);
  TEST("comparison", image2 < image2, false);
  image0 = image2;
  TEST("Equality", image2, image0);
  TEST("comparison", image2 < image0, false);

  vil_image_view<T> image7;
  TEST("Equality", image2 == image7, false);
  TEST("Comparison", image2 < image7, true);
  TEST("Comparison", image2 > image7, false);

  std::cout << "**********************************\n"
           << " Testing vil_image_view functions\n"
           << "**********************************\n";

  image2.fill(0);
  image_win = vil_crop(image2,2,1,1,2);
  TEST("Equality", image2 == image_win, false);
  TEST("Comparison", image_win < image2, true);
  TEST("Comparison", image_win > image2, false);
  image5.set_size(1,2);
  image5(0,0).r = 25; image5(0,0).g = 35; image5(0,0).b = 45;
  image5(0,1).r = 25; image5(0,1).g = 35; image5(0,1).b = 45;
  image0 = image5;

  vil_copy_reformat(image0, image_win);
  vil_print_all(std::cout, image2);
  vil_image_view<S> test_image(5,4,3);
  test_image.fill(0);
  test_image(2,1,0) = test_image(2,2,0) = 25;
  test_image(2,1,1) = test_image(2,2,1) = 35;
  test_image(2,1,2) = test_image(2,2,2) = 45;
  TEST("vil_reformat_copy, vil_crop and vil_deep_equality",
       vil_image_view_deep_equality(test_image,image2), true);
  test_image(2,2,2) = 44;
  TEST("!vil_deep_equality", vil_image_view_deep_equality(test_image,image2), false);
  test_image.set_size(5,4,4);
  TEST("!vil_deep_equality", vil_image_view_deep_equality(test_image,image2), false);
  vil_print_all(std::cout, image2);

  vil_convert_cast(image2, image7);
  vil_print_all(std::cout, image7);
  vil_transform(image7, image7, std::bind2nd(std::plus<T>(), 0.6f));
  vil_convert_cast(image7, image2);
  vil_print_all(std::cout, image2);

  const vil_pixel_format format = vil_pixel_format_of(S());
  if (format == VIL_PIXEL_FORMAT_FLOAT || format == VIL_PIXEL_FORMAT_DOUBLE)
  {
    TEST_NEAR("Casting ", (double)image2(0,0,0), 0.6, 1e-5);
    TEST_NEAR("Casting ", (double)image2(2,2,1), 35.6, 1e-5);
  }
  else
  {
    TEST("Casting ", image2(0,0,0) == 0 && image2(2,2,1) == 35, true);
  }

  image7.clear();
  vil_convert_rgb_to_grey(vil_view_as_rgb(image2), image7);
  vil_print_all(std::cout, image7);
  TEST("vil_convert_rgb_to_grey(vil_rgb)", image7?true:false, true);

  if (format == VIL_PIXEL_FORMAT_FLOAT || format == VIL_PIXEL_FORMAT_DOUBLE)
  {
    TEST_NEAR("Conversion rgb to grey", image7(0,0), 0.6, 1e-5);
  }
  else
  {
    TEST_NEAR("Conversion rgb to grey", image7(0,0), 0.0, 1e-5);
  }
  if (format == VIL_PIXEL_FORMAT_FLOAT || format == VIL_PIXEL_FORMAT_DOUBLE)
  {
    TEST_NEAR("Conversion rgb to grey", image7(2,1), 34.196, 1e-5);
  }
  else
  {
    TEST_NEAR("Conversion rgb to grey", image7(2,1), 33.596, 1e-5);
  }

  vil_convert_cast(image7, image5);
  TEST("vil_convert_cast<T,S>", image5?true:false, true);
  vil_print_all(std::cout, image5);

  vil_image_view<T> image10 =
    vil_convert_to_n_planes(3, vil_new_image_view_base_sptr(image7));
  TEST("vil_convert_round(image10<T>, sptr)", image10?true:false, true);
  vil_print_all(std::cout, image10);


  // Only test rgba conversion for float and byte.
  // other components types don't have rgba instantiated.
  test_image_view_rgba(image2, image7);

  //Test vil_planes
  vil_image_view<S> image8(11,17,9);
  vil_image_view<S> image9 = vil_planes(image8,7,-2,3);
  TEST("vil_planes",
       vil_plane(image9,0) == vil_plane(image8,7) &&
       vil_plane(image9,1) == vil_plane(image8,5) &&
       vil_plane(image9,2) == vil_plane(image8,3), true);
}

static void test_contiguous()
{
  vil_image_view<vxl_byte> im1( 4, 5, 6 );
  TEST( "internal memory: contiguous", im1.is_contiguous(), true );

  vxl_byte memory[ 3*5*7 ];

  for ( unsigned d1 = 0; d1 < 3; ++d1 )
  {
    for ( unsigned d2 = 0; d2 < 3; ++d2 )
    {
      if ( d2==d1 ) continue;
      for ( unsigned d3 = 0; d3 < 3; ++d3 )
      {
        if ( d3==d1 || d3==d2 ) continue;
        std::ptrdiff_t step[3];
        step[d1] = 1;
        step[d2] = 3;
        step[d3] = 15;
        std::ostringstream str;
        str << "external memory: " << step[0] << 'x'<<step[1]<<'x'<<step[2]<<" step contiguous";
        vil_image_view<vxl_byte> im( memory, 3, 5, 7, step[d1], step[d2], step[d3] );
        TEST( str.str().c_str(), im.is_contiguous(), true );
      }
    }
  }

  for ( unsigned d1 = 0; d1 < 3; ++d1 )
  {
    for ( unsigned d2 = 0; d2 < 3; ++d2 )
    {
      if ( d2==d1 ) continue;
      for ( unsigned d3 = 0; d3 < 3; ++d3 )
      {
        if ( d3==d1 || d3==d2 ) continue;
        std::ptrdiff_t step[3];
        step[d1] = 2;
        step[d2] = 3;
        step[d3] = 15;
        std::ostringstream str;
        str << "external memory: " << step[0] << 'x'<<step[1]<<'x'<<step[2]<<" step not contiguous";
        vil_image_view<vxl_byte> im( memory, 3, 5, 7, step[d1], step[d2], step[d3] );
        TEST( str.str().c_str(), im.is_contiguous(), false );
      }
    }
  }
  vil_image_view<float> im2 = vil_new_image_view_plane_i_j(4, 5, 6, float());
  TEST( "vil_new_image_view_plane_i_j is_continuous", im2.is_contiguous(), true );
}

static void test_image_view_fill()
{
  vil_image_view<vxl_byte> image(10,10,2);
  image.fill(vxl_byte(17));
  TEST("fill (contiguous) (0,0,0)", image(0,0,0), vxl_byte(17));
  TEST("fill (contiguous) (9,9,1)", image(9,9,1), vxl_byte(17));

  vil_image_view<vxl_byte> crop_image=vil_crop(image,4,4,4,4);
  crop_image.fill(vxl_byte(23));
  TEST("fill (istep==1) (0,0,0)", crop_image(0,0,0), vxl_byte(23));
  TEST("fill (istep==1) (3,3,1)", crop_image(3,3,1), vxl_byte(23));
  TEST("fill (original) ", image(4,4,0), vxl_byte(23));
  TEST("fill (no i under-run) ", image(3,4,0), vxl_byte(17));
  TEST("fill (no i over-run) ", image(8,4,0), vxl_byte(17));
  TEST("fill (no j under-run) ", image(4,3,0), vxl_byte(17));
  TEST("fill (no j over-run) ", image(8,4,0), vxl_byte(17));

  image.fill(vxl_byte(11));
  vil_transpose(crop_image).fill(vxl_byte(25));
  TEST("fill (jstep==1) (0,0,0)", crop_image(0,0,0), vxl_byte(25));
  TEST("fill (jstep==1) (3,3,1)", crop_image(3,3,1), vxl_byte(25));
  TEST("fill (original) ", image(4,4,0), vxl_byte(25));
  TEST("fill (no i under-run) ", image(3,4,0), vxl_byte(11));
  TEST("fill (no i over-run) ", image(8,4,0), vxl_byte(11));
  TEST("fill (no j under-run) ", image(4,3,0), vxl_byte(11));
  TEST("fill (no j over-run) ", image(8,4,0), vxl_byte(11));

  image.fill(vxl_byte(11));
  vil_fill_line<vxl_byte>(image, 0, 0, 9, 4, 25);
  unsigned n_pix=0, n_wrong_pix=0;
  for (unsigned i=0; i<10; ++i)
    for (unsigned j=0; j<10; ++j)
    {
      if (image(i,j) == 25)
      {
        n_pix++;
        if (i/2 != j) n_wrong_pix++;
      }
    }
  TEST("fill line, expected number of pixels",n_pix, 10);
  TEST("fill line, correct places",n_wrong_pix, 0);
  vil_print_all(std::cout , image);

  image.fill(vxl_byte(11));
  vil_fill_line<vxl_byte>(image, 0, 0, 4, 9, 25);
  n_pix=0, n_wrong_pix=0;
  for (unsigned i=0; i<10; ++i)
    for (unsigned j=0; j<10; ++j)
    {
      if (image(i,j) == 25)
      {
        n_pix++;
        if (j/2 != i) n_wrong_pix++;
      }
    }
  TEST("fill line, expected number of pixels",n_pix, 10);
  TEST("fill line, correct places",n_wrong_pix, 0);
  vil_print_all(std::cout , image);
}

static void test_complex_image_view()
{
  std::cout << "************************************************\n"
           << " Testing vil_image_view complex image functions\n"
           << "************************************************\n";

  vil_image_view<std::complex<float> > image_cf (10,7,1);
  image_cf.fill (std::complex<float> (1.1f, 2.2f));

  vil_image_view<std::complex<double> > image_cd;
  vil_convert_cast(image_cf, image_cd);
  TEST("vil_convert_cast<complex<float>, complex<double> >", image_cd?true:false, true);

  vil_image_view<double> image_rd;
  image_rd = vil_view_real_part (image_cd);
  TEST("vil_view_real_part<double>", image_rd?true:false, true);
  TEST_NEAR("Real part correct value", image_rd(1,1,0), 1.1, 1e-5);

  vil_image_view<double> image_id;
  image_id = vil_view_imag_part (image_cd);
  TEST("vil_view_imag_part<double>", image_id?true:false, true);
  TEST_NEAR("Imag part correct value", image_id(1,1,0), 2.2, 1e-5);

  image_rd (1,1,0) = 3.3;
  TEST_NEAR("Changing real view changes complex view", image_cd(1,1,0).real(), 3.3, 1e-5);

  vil_image_view<float> image_pf;
  image_pf = vil_view_as_planes (image_cf);
  TEST("vil_view_as_planes<float>", image_pf?true:false, true);

  vil_image_view<std::complex<float> > image_cf2;
  image_cf2 = vil_view_as_complex (image_pf);
  TEST("vil_view_as_complex<float>", image_cf2?true:false, true);

  TEST("Conversion from complex<float> to planes and back", image_cf2, image_cf);
}


static void test_image_view_assignment_operator()
{
  // This test makes sure that the assignment operator does the
  // correct thing w.r.t. reference counts and such. The test copies a
  // temporary view to an existing view. If a bitwise copy is done, the
  // existing view's reference count is overwritten, and the routine
  // will segfault.

  testlib_test_begin( "Assignment operator" );

  typedef vil_image_view<vxl_byte> byte_view;

  // Construct two views with non-zero reference counts
  //
  vil_image_view_base_sptr im1p = new byte_view( 5, 5 );
  vil_image_view_base_sptr im2p = new byte_view( 5, 5 );

  // Assign one to the other use a temporary (which has a reference
  // count of zero).
  auto& im2 = static_cast< byte_view& >( *im2p );
  im2 = byte_view( im1p );

  // If we get here, then all is well. We should check that the
  // reference count is non-zero, but that's private information.
  //
  testlib_test_perform( true );


  // Test error reporting on dodgy type assignment.

#if !defined VXL_LEGACY_ERROR_REPORTING
  bool caught_exception = false;
  try
  {
    vil_image_view<float> float_im = *im1p;
  }
  catch (const vil_exception_pixel_formats_incompatible &e)
  {
    caught_exception = true;
    std::cout << "Exception: " << e.what() << std::endl;
  }
  TEST ("Successfully caught expected exception", caught_exception, true);
#else
    vil_image_view<float> float_im = *im1p;
    TEST ("Successfully detected dodgy assignment", float_im?true:false, false);
#endif
}

static void test_non_standard_type();

static void test_image_view()
{
  test_non_standard_type();
  std::cout << "****************************************\n"
           << " Testing vil_image_view<byte and float>\n"
           << "****************************************\n";
  test_image_view(vxl_byte(), "vxl_byte", float());
  std::cout << "******************************************\n"
           << " Testing vil_image_view<float and double>\n"
           << "******************************************\n";
  test_image_view(float(), "float", double());
  std::cout << "******************************************\n"
           << " Testing vil_image_view<int_16 and float>\n"
           << "******************************************\n";
  test_image_view(vxl_int_16(), "vxl_int_16", float());
  std::cout << "*******************************************\n"
           << " Testing vil_image_view<uint_32 and float>\n"
           << "*******************************************\n";
  test_image_view(vxl_uint_32(), "vxl_uint_32", float());
#if VXL_HAS_INT_64
  std::cout << "*******************************************\n"
           << " Testing vil_image_view<uint_64 and float>\n"
           << "*******************************************\n";
  test_image_view(vxl_uint_64(), "vxl_uint_64", float());
#endif
  test_contiguous();
  test_image_view_fill();
  test_image_view_assignment_operator();
  test_complex_image_view();
}

TESTMAIN(test_image_view);


#include <vil/vil_image_view.hxx>

class my_int
{
  int a;
 public:
  my_int(): a(0) {};
  my_int(const my_int& i) = default;;
  explicit my_int(int i): a(i) {};
  my_int operator +(const my_int& rhs) const
  {
    my_int rv;
    rv.a = this->a + rhs.a;
    return rv;
  }
  my_int operator -(const my_int& rhs) const
  {
    my_int rv;
    rv.a = this->a - rhs.a;
    return rv;
  }
  const my_int& operator -()
  {
    this->a = -this->a;
    return *this;
  }
  my_int operator *(int s) const
  {
    my_int rv;
    rv.a = this->a*s;
    return rv;
  }
  const my_int& operator =(double v)
  {
    this->a = static_cast<int>(v);
    return *this;
  }
  operator double () const { return (double) this->a; }
};


template <>
inline bool convert_components_from_planes(vil_image_view<my_int> &,
                                           const vil_image_view_base &)
{ return false; }  // when lhs has scalar pixels, don't attempt conversion

template <>
void vil_print_value(std::ostream& os, const my_int& v, unsigned)
{ os<<double(v); }

VIL_IMAGE_VIEW_INSTANTIATE( my_int );


static void test_non_standard_type()
{
  std::cout << "********************************\n"
           << " Testing vil_image_view<my_int>\n"
           << "********************************\n";

  vil_image_view<my_int> my_int_image(3,4);


  TEST("vil_image_view<my_int> construction", !my_int_image, false);

  my_int_image.fill(my_int(1));
  my_int_image(0,0) = my_int(100);
  my_int_image(2,3) = my_int(100);

  vil_math_scale_values(my_int_image, 2.0);
  vil_print_all(std::cout, my_int_image);
}

