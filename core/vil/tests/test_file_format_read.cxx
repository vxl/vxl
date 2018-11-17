// This is core/vil/tests/test_file_format_read.cxx
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vxl_config.h> // for vxl_uint_16 etc.

#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <vil/vil_file_format.h>
#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/vil_config.h>

// For testing specific file formats
#include <vil/vil_stream_fstream.h>

// For indication whether BigTIFF is available (libtiff version >= 4.0)
#include <tiff.h>

// \author Amitha Perera
// \date Apr 2002
//
// Compare the results of loading different files with the true data
// that's supposed to be in those files.
//
// \verbatim
//  Modifications
//   14 Nov  2011 - Gehua Yang     - added tests for loading 32bpp ARGB image.
//    7 July 2016 - Gehua Yang     - added tests for loading BigTIFF images.
// \endverbatim

static std::string image_base;


// A comparator interface to check if the pixels match. It will be
// called with all the components at a single location (given by i, j and p).
//
template<class TruePixelType, class ImgPixelType>
struct Compare
{
  virtual ~Compare() = default;
  virtual bool operator() ( vil_image_view<ImgPixelType> const&,
                            int p, int i, int j,
                            const std::vector<TruePixelType>& pixel ) const = 0;
};


// Below are various "real" comparators


template<class PixelType>
struct CompareRGB
  : public Compare<PixelType, vil_rgb<PixelType> >
{
  bool operator() ( vil_image_view< vil_rgb<PixelType> > const& img,
                            int p, int i, int j,
                            const std::vector<PixelType>& pixel ) const override
  {
    return p==0 && pixel.size() == 3 &&
           img(i,j).r == pixel[0] &&
           img(i,j).g == pixel[1] &&
           img(i,j).b == pixel[2];
  }
};


template<class PixelType>
struct CompareRGBA
  : public Compare<PixelType, vil_rgba<PixelType> >
{
  bool operator() ( vil_image_view< vil_rgba<PixelType> > const& img,
                            int p, int i, int j,
                            const std::vector<PixelType>& pixel ) const override
  {
    return p==0 && pixel.size() == 4 &&
           img(i,j).r == pixel[0] &&
           img(i,j).g == pixel[1] &&
           img(i,j).b == pixel[2] &&
           img(i,j).a == pixel[3];
  }
};


template<class PixelType>
struct CompareRGBNear
  : public Compare<PixelType, vil_rgb<PixelType> >
{
  bool operator() ( vil_image_view< vil_rgb<PixelType> > const& img,
                            int p, int i, int j,
                            const std::vector<PixelType>& pixel ) const override
    {
      if ( p == 0 && pixel.size() == 3 ) {
        // Find difference in two values whilst avoiding unsigned underflow
        auto diff_A = static_cast<PixelType>
                           ( pixel[0]*pixel[0] + img(i,j).r * img(i,j).r +
                             pixel[1]*pixel[1] + img(i,j).g * img(i,j).g +
                             pixel[2]*pixel[2] + img(i,j).b * img(i,j).b );

        auto diff_B = static_cast<PixelType>
                           ( 2 * pixel[0] * img(i,j).r + tol_sq_ +
                             2 * pixel[1] * img(i,j).g + tol_sq_ +
                             2 * pixel[2] * img(i,j).b + tol_sq_ );
        return diff_A < diff_B;
      }
      else {
        return false;
      }
    }

  CompareRGBNear( PixelType tol )
    : tol_sq_( static_cast<PixelType>(tol*tol) )
    { }

  PixelType tol_sq_;
};


template<class PixelType, int num_planes>
struct ComparePlanes
  : public Compare<PixelType, PixelType >
{
  bool operator() ( vil_image_view<PixelType> const& img,
                            int p, int i, int j,
                            const std::vector<PixelType>& pixel ) const override
  {
    return 0 <= p && p < num_planes && pixel.size() == 1 && img(i,j,p) == pixel[0];
  }
};


// Greyscale is simply planar with 1 plane.
template<class PixelType>
struct CompareGrey
  : public ComparePlanes<PixelType, 1>
{
};

// Greyscale+Alpha has two (2) planes
template<class PixelType>
struct CompareGreyAlpha
  : public ComparePlanes<PixelType, 2>
{
};

template<class PixelType>
struct CompareGreyFloat
  : public Compare<PixelType,PixelType>
{
  bool operator() ( vil_image_view<PixelType> const& img,
                            int p, int i, int j,
                            const std::vector<PixelType>& pixel ) const override
  {
    return p==0 && pixel.size() == 1 &&
           std::fabs( pixel[0] - img(i,j) ) <= 1e-6 * std::fabs( pixel[0] );
  }
};


template<class PixelType>
struct CompareGreyNear
  : public Compare<PixelType,PixelType>
{
  bool operator() ( vil_image_view<PixelType> const& img,
                            int p, int i, int j,
                            const std::vector<PixelType>& pixel ) const override
    {
      if ( p==0 && pixel.size() == 1 ) {
        // Find difference in two values whilst avoiding unsigned underflow
        auto diff_A = static_cast<PixelType>(pixel[0] * pixel[0] + img(i,j) * img(i,j));
        auto diff_B = static_cast<PixelType>(2 * pixel[0] * img(i,j) + tol_sq_);
        return diff_A <= diff_B;
      }
      else {
        return false;
      }
    }

  CompareGreyNear( PixelType tol )
    : tol_sq_( static_cast<PixelType>(tol*tol) )
    { }

  PixelType tol_sq_;
};


// ===========================================================================
// read value

// To read the true pixel data, we can't just do a fin >> x when x is
// a char, because that would read in a character, not a small
// integer. The following function and its specializations are to
// solve this issue.

// Read in a number from the stream into pix.
// Return value is true if the read FAILED.
template<class TruePixelType>
bool
read_value( std::istream& fin, TruePixelType& pix )
{
  return ! (fin >> pix);
}

// Specialization to make char read as small integers and not characters
// See comments on template for return value.
template <>
bool
read_value( std::istream& fin, char& pix )
{
  int x;
  // use operator! to test the stream to avoid compiler warnings
  bool bad = ! ( fin >> x );
  if ( !bad ) pix = char(x);
  return bad;
}


// Specialization to make char read as small integers and not characters
// See comments on template for return value.
template <>
bool
read_value( std::istream& fin, unsigned char& pix )
{
  int x;
  // use operator! to test the stream to avoid compiler warnings
  bool bad = ! ( fin >> x );
  if ( !bad ) pix = static_cast<unsigned char>(x);
  return bad;
}


// Specialization to make char read as small integers and not characters
// See comments on template for return value.
template <>
bool
read_value( std::istream& fin, signed char& pix )
{
  int x;
  // use operator! to test the stream to avoid compiler warnings
  bool bad = ! ( fin >> x );
  if ( !bad ) pix = static_cast<signed char>(x);
  return bad;
}

// Specialization to make bool read as 0/1 integers
// See comments on template for return value.
template <>
bool
read_value( std::istream& fin, bool& pix )
{
  int x;
  // use operator! to test the stream to avoid compiler warnings
  bool bad = ! ( fin >> x );
  if ( !bad ) pix = ( x != 0 );
  return bad;
}


// ===========================================================================
// Check pixels
//
// Compare the pixels in an already loaded image resource against the true
// pixel values with the given pixel comparator.
//
template<class TruePixelType, class ImgPixelType>
bool
CheckPixels( Compare<TruePixelType,ImgPixelType> const& check,
             char const* true_data_file,
             const vil_image_resource_sptr& ir )
{
  // The true data is an ASCII file consisting of a sequence of
  // numbers. The first set of numbers are:
  //    number of planes (P)
  //    number of components (C)
  //    width (in pixels, not components)
  //    height (in pixels, not components)
  //    planes*width*height*components of data, in the following order:
  //        plane1pix1comp1 plane1pix1comp2 ... plane1pixNcompC ... planePpix1comp1 ... planePpixNcompC
  //      where N = width*height

  int num_planes;
  int num_comp;
  int width;
  int height;

  std::ifstream fin( (image_base+true_data_file).c_str() );
  if ( !( fin >> num_planes >> num_comp >> width >> height ) ) {
    std::cout << "[couldn't read header from " << true_data_file << ']' << std::flush;
    return false;
  }

  // Generate different views from the resource, including the full image.
  //
  std::vector< vil_image_view<ImgPixelType> > views;
  std::vector< unsigned > offi;
  std::vector< unsigned > offj;
  for ( unsigned dl = 0; dl < 3; ++dl ) {
    for ( unsigned dt = 0; dt < 2; ++dt ) {
      for ( unsigned dr = 0; dr < 3; ++dr ) {
        for ( unsigned db = 0; db < 2; ++db ) {
          if ( dl+dr < ir->ni() && dt+db < ir->nj() ) {
            vil_image_view_base_sptr im =
              ir->get_copy_view( dl, ir->ni()-dl-dr, dt, ir->nj()-dt-db );
            if ( !im ) {
              std::cout << "[ couldn't read (sub)image data from " << ir << "]\n"
                       << "[ off=(" << dl << ',' << dt
                       << ")x(" << ir->ni()-dl-dr << ','
                       << ir->nj()-dt-db << " ]" << std::endl;
              return false;
            }
            vil_image_view<ImgPixelType> img = im;
            if ( !img ) {
              std::cout << "[ couldn't read (sub)image data of the expected format from "
                       << im << " at offset " << dl << ',' << dt << ']' << std::endl;
              return false;
            }

            if ( img.ni() != ir->ni()-dl-dr || img.nj() != ir->nj()-dt-db ) {
              std::cout << "[ (sub)image has wrong size (!= "
                       << ir->ni()-dl-dr << 'x' << ir->nj()-dt-db
                       << ") in " << img << std::endl;
              return false;
            }

#ifdef DEBUG
            if (img.size() < 100) {
              std::cout << "\nSubimage " << views.size() << " at ("<<dl<<','<<dt<<"):\n";
              vil_print_all(std::cout, img); std::cout.flush();
            }
            else {
              std::cout << "Subimage size = " << img.size() << ".  Too large to display all pixels." << std::endl;
            }
#endif

            views.push_back( img );
            offi.push_back( dl );
            offj.push_back( dt );
          }
        }
      }
    }
  }

  // Compare pixels
  //

  std::vector<TruePixelType> pixel( num_comp );

  for ( int p=0; p < num_planes; ++p ) {
    for ( int j=0; j < height; ++j ) {
      for ( int i=0; i < width; ++i ) {
        for ( int c=0; c < num_comp; ++c )
        {
          // Need to do things this way because pixel[c] may have a
          // type different from TruePixelType. For example,
          // vector<bool> has weird types to compensate for the packed
          // bit representation.
          //
          TruePixelType pv;
          if ( read_value( fin, pv ) )
          {
            return false;
          }
          pixel[c] = pv;
        }

        // Check the pixels of each view containing this coordinate.
        //
        for ( unsigned v = 0; v < views.size(); ++v ) {
          if ( i >= int(offi[v]) && i < int(offi[v]+views[v].ni()) &&
               j >= int(offj[v]) && j < int(offj[v]+views[v].nj()) &&
               !check( views[v], p, i-offi[v], j-offj[v], pixel ) ) {
            std::cout << "View " << v << " at offset (" << offi[v] << ',' << offj[v]
                     << ") [ " << views[v] << " ] has a mismatch at pixel (p="
                     << p << ",i=" << i << ",j=" << j << ')' << std::endl;
            if ( views[v].size() < 100 ) {
              vil_print_all( std::cout, views[v] );
              std::cout.flush();
            }
            return false;
          }
        }
      }
    }
  }

  return true;
}


// ===========================================================================
// Check file
//
// Load the given image file in the standard way and compare the
// pixels against the true pixel values with the given pixel
// comparator.
//
template<class TruePixelType, class ImgPixelType>
bool
CheckFile( Compare<TruePixelType,ImgPixelType> const& check,
           char const* true_data_file,
           char const* img_data_file )
{
  vil_image_resource_sptr ir = vil_load_image_resource((image_base + img_data_file).c_str());
  if ( !ir ) {
    std::cout << "[ couldn't load image file " << img_data_file << " ]" << std::endl;
    return false;
  }
  else {
    return CheckPixels( check, true_data_file, ir );
  }
}

// ===========================================================================
// Check format
//
// Load the given image file using the given file format and compare
// the pixels against the true pixel values with the given pixel
// comparator.
//
template<class TruePixelType, class ImgPixelType>
bool
CheckFormat( Compare<TruePixelType,ImgPixelType> const& check,
             char const* true_data_file,
             char const* img_data_file,
             vil_file_format* ffmt )
{
  bool result;
  vil_stream* is = new vil_stream_fstream( (image_base + img_data_file).c_str(), "r" );
  is->ref();
  if ( is->ok() ) {
    vil_image_resource_sptr ir = ffmt->make_input_image( is );
    if ( !ir ) {
      std::cout << "[ couldn't load image file " << img_data_file << " ]" << std::endl;
      result = false;
    }
    else {
      result = CheckPixels( check, true_data_file, ir );
    }
  }
  else {
    std::cout << "[ couldn't open file " << img_data_file << " for reading ]" << std::endl;
    result = false;
  }
  is->unref();
  return result;
}


// ===========================================================================
// Compute the peak difference between 2 views across the entire image
//
template<typename T_PIXEL>
bool
image_equals( const std::string &test_file, const std::string &ref_file)
{
  vil_image_view_base_sptr view_test_ptr =
    vil_load((image_base+test_file).c_str());
  if( !view_test_ptr )
  {
    std::cout << "[ couldn't open or decode file " << test_file << " ]\n";
    return false;
  }
  vil_image_view_base_sptr view_ref_ptr =
    vil_load((image_base+ref_file).c_str());
  if( !view_ref_ptr )
  {
    std::cout << "[ couldn't open or decode file " << ref_file << " ]\n";
    return false;
  }

  auto *view_test =
    dynamic_cast<vil_image_view<T_PIXEL>*>(view_test_ptr.as_pointer());
  auto *view_ref =
    dynamic_cast<vil_image_view<T_PIXEL>*>(view_ref_ptr.as_pointer());

  return vil_image_view_deep_equality(*view_test, *view_ref);
}


static void
test_file_format_read( int argc, char* argv[] )
{
  // test data path is not passed into argv
  bool exists = false;
  if ( argc >= 2 ) {
    image_base = argv[1];
  }else{
    std::string root = testlib_root_dir();
    image_base = root + "/core/vil/tests/file_read_data";
  }
  exists = vul_file::is_directory(image_base);
  image_base += "/";
  if(!exists){
    TEST("No test data", false, true);
    std::cout << "Failed file path: " << image_base << '\n';
    return;
  }
  // Test generic file loads

  std::cout << "GENERIC FILE LOAD\n\n"
           << "Portable aNy Map [pnm]: (pbm, pgm, ppm)\n";

  TEST("1-bit pbm ascii", CheckFile(CompareGrey<bool>(), "ff_grey1bit_true.txt", "ff_grey1bit_ascii.pbm" ), true);
  TEST("1-bit pbm raw", CheckFile(CompareGrey<bool>(), "ff_grey1bit_true.txt", "ff_grey1bit_raw.pbm" ), true);
  TEST("8-bit pgm ascii", CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit_ascii.pgm" ), true);
  TEST("8-bit pgm raw", CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit_raw.pgm" ), true);
  TEST("8-bit pgm raw 2", CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit_raw2.pgm" ), true);
  TEST("16-bit pgm ascii", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_ascii.pgm" ), true);
  TEST("16-bit pgm raw", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_raw.pgm" ), true);
  TEST("8-bit ppm ascii", CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_ascii.ppm" ), true);
  TEST("8-bit ppm ascii as planar", CheckFile(ComparePlanes<vxl_byte,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit_ascii.ppm" ), true);
  TEST("8-bit ppm raw", CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_raw.ppm" ), true);
  TEST("8-bit ppm raw as planar", CheckFile(ComparePlanes<vxl_byte,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit_raw.ppm" ), true);
  TEST("16-bit ppm ascii", CheckFile(CompareRGB<vxl_uint_16>(), "ff_rgb16bit_true.txt", "ff_rgb16bit_ascii.ppm" ), true);
  TEST("16-bit ppm raw", CheckFile(CompareRGB<vxl_uint_16>(), "ff_rgb16bit_true.txt", "ff_rgb16bit_raw.ppm" ), true);

  std::cout << "JPEG [jpg]\n";
  TEST("8-bit grey, normal image to 4 quanta", CheckFile(CompareGreyNear<vxl_byte>(4), "ff_grey8bit_true.txt", "ff_grey8bit_compressed.jpg" ), true);
  TEST("8-bit RGB, easy image accurate to 3 quanta", CheckFile(CompareRGBNear<vxl_byte>(3), "ff_rgb8biteasy_true.txt", "ff_rgb8biteasy_compressed.jpg" ), true);

  std::cout << "Windows bitmap [bmp]\n";
  TEST("8-bit greyscale (xv created)", CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit.bmp" ), true);
  //TEST("8-bit GA uncompressed", CheckFile(CompareGreyAlpha<vxl_byte>(), "ff_ga8bit_true.txt", "ff_ga8bit.bmp" ), true);
  TEST("8-bit RGB (xv created)", CheckFile(ComparePlanes<vxl_byte,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit_xv.bmp" ), true);
  TEST("8-bit RGB (Top-down scans, Photoshop created)", CheckFile(ComparePlanes<vxl_byte,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit_topdown_ps.bmp" ), true);
  TEST("8-bit RGBA uncompressed (Photoshop created)", CheckFile(CompareRGBA<vxl_byte>(), "ff_rgba8bit_true.txt", "ff_rgba8bit_uncompressed_ps.bmp" ), true);

  std::cout << "Portable Network Graphics [png]\n";
  TEST("1-bit grey compressed", CheckFile(CompareGrey<bool>(), "ff_grey1bit_true.txt", "ff_grey1bit.png" ), true);
  TEST("8-bit RGB uncompressed", CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_uncompressed.png" ), true);
  TEST("8-bit RGB compressed", CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_compressed.png" ), true);
  TEST("8-bit RGB Indexed",    CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_indexed_4bpp.png" ), true);
  TEST("8-bit GA compressed", CheckFile(CompareGreyAlpha<vxl_byte>(), "ff_ga8bit_true.txt", "ff_ga8bit_compressed.png" ), true);
  TEST("10-bit grey with bitdepth=true (from labview)", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey10bit_true.txt", "ff_grey10bit_labview_bitdepth_true.png" ), true);
  TEST("10-bit grey with bitdepth=false (from labview)", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey10bit_true.txt", "ff_grey10bit_labview_bitdepth_false.png" ), true);
  TEST("16-bit grey with bitdepth=true (from labview)", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_labview_bitdepth_true.png" ), true);
  TEST("16-bit grey with bitdepth=false (from labview)", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_labview_bitdepth_false.png" ), true);

  std::cout << "TIFF [tiff]\n";
  //TEST("1-bit grey uncompressed", CheckFile(CompareGrey<bool>(), "ff_grey1bit_true.txt", "ff_grey1bit_uncompressed.tif" ), true);
  TEST("8-bit RGB uncompressed", CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_uncompressed.tif" ), true);

  TEST("8-bit RGB packbits", CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_packbits.tif" ), true);
  TEST("32-bit float grey", CheckFile(CompareGreyFloat<float>(), "ff_grey_float_true.txt", "ff_grey_float.tif" ), true);
  TEST("8-bit RGBA uncompressed", CheckFile(CompareRGBA<vxl_byte>(), "ff_rgba8bit_true.txt", "ff_rgba8bit_uncompressed.tif" ), true);
  TEST("8-bit RGBA without RowsPerStrip", CheckFile(CompareRGBA<vxl_byte>(), "no_rowsperstrip_true.txt", "no_rowsperstrip.tif" ), true);
  TEST("8-bit GA uncompressed", CheckFile(CompareGreyAlpha<vxl_byte>(), "ff_ga8bit_true.txt", "ff_ga8bit_uncompressed.tif" ), true);
  //TEST("16-bit GA uncompressed", CheckFile(CompareGreyAlpha<vxl_uint_16>(), "ff_ga16bit_true.txt", "ff_ga16bit_uncompressed.tif" ), true);

#if defined(TIFF_VERSION_BIG)
  TEST("8-bit RGB Classic", CheckFile(ComparePlanes<vxl_byte, 3>(), "bigtiff/Classic.txt", "bigtiff/Classic.tif"), true);
  TEST("8-bit RGB BigTIFF", image_equals<vxl_byte>("bigtiff/BigTIFF.tif", "bigtiff/Classic.tif"), true);
  TEST("8-bit RGB BigTIFF long", image_equals<vxl_byte>("bigtiff/BigTIFFLong.tif", "bigtiff/Classic.tif"), true);
  TEST("8-bit RGB BigTIFF long8", image_equals<vxl_byte>("bigtiff/BigTIFFLong8.tif", "bigtiff/Classic.tif"), true);
  TEST("8-bit RGB BigTIFF long8Tiles", image_equals<vxl_byte>("bigtiff/BigTIFFLong8Tiles.tif", "bigtiff/Classic.tif"), true);
  TEST("8-bit RGB BigTIFF Motorola", image_equals<vxl_byte>("bigtiff/BigTIFFMotorola.tif", "bigtiff/Classic.tif"), true);
  TEST("8-bit RGB BigTIFF SubIFD4", image_equals<vxl_byte>("bigtiff/BigTIFFSubIFD4.tif", "bigtiff/Classic.tif"), true);
  TEST("8-bit RGB BigTIFF SubIFD8", image_equals<vxl_byte>("bigtiff/BigTIFFSubIFD8.tif", "bigtiff/Classic.tif"), true);
#endif

  // The following tests are targeted to the vil_nitf2_image class which can read NITF 2.1, NITF 2.0 and
  // NSIF 1.0 files.  All three of these formats are covered here as well as all four different
  // types of data layouts for uncompressed data (IMODE).  Also we test a fair number of different
  // data types (eg. 8-bit unsigned, 16-bit unsigned, 32-bit signed, double, float, bool)
  // TODO: Create NITF tests for the following cases:
  // 1) read second image test (ie. multiple images in one file)
  std::cout << "NITF 2.1 [nitf] (uncompressed)\n";
  TEST("8-bit unsigned int (IMODE=P)", CheckFile(ComparePlanes<vxl_byte,3>(), "ff_nitf_8bit_true.txt", "ff_nitf_8bit_p.nitf" ), true);
  TEST("8-bit unsigned int (IMODE=B)", CheckFile(ComparePlanes<vxl_byte,3>(), "ff_nitf_8bit_true.txt", "ff_nitf_8bit_b.nitf" ), true);
  TEST("8-bit unsigned int (IMODE=R)", CheckFile(ComparePlanes<vxl_byte,3>(), "ff_nitf_8bit_true.txt", "ff_nitf_8bit_r.nitf" ), true);
  TEST("8-bit unsigned int (IMODE=S)", CheckFile(ComparePlanes<vxl_byte,3>(), "ff_nitf_8bit_true.txt", "ff_nitf_8bit_s.nitf" ), true);
  TEST("16-bit unsigned int (ABPP=13)", CheckFile(CompareGrey<vxl_uint_16>(), "ff_nitf_16bit_true.txt", "ff_nitf_16bit.nitf" ), true);
  std::cout << "NITF 2.0 [nitf] (uncompressed)\n";
  TEST("32-bit signed int", CheckFile(CompareGrey<vxl_int_32>(), "ff_nitf_32bit_true.txt", "ff_nitf_32bit.nitf" ), true);
  TEST("32-bit float", CheckFile(CompareGreyFloat<float>(), "ff_nitf_float_true.txt", "ff_nitf_float.nitf" ), true);
  TEST("64-bit float (double)", CheckFile(CompareGreyFloat<double>(), "ff_nitf_float_true.txt", "ff_nitf_double.nitf" ), true);
  std::cout << "NSIF 1.0 [nitf] (uncompressed)\n";
  TEST("1-bit bool (NSIF w/ LUT to parse)", CheckFile(CompareGrey<bool>(), "ff_nitf_1bit_lut_true.txt", "ff_nitf_1bit_lut.nsif" ), true);

#if HAS_J2K
  std::cout << "JPEG 2000 [j2k,jpc]\n";
  TEST("p0_12 (3x5x1 x 8Bit)", CheckFile(CompareGrey<vxl_byte>(), "p0_12_true.txt", "p0_12.j2k" ), true);

  std::cout << "NITF 2.1 [nitf] (JPEG 2000 compressed)\n";
  TEST("p0_12 (3x5x1 x 8Bit)", CheckFile(CompareGrey<vxl_byte>(), "p0_12_true.txt", "p0_12a.ntf" ), true);
#endif // HAS_J2K

#if HAS_OPENJPEG2
  std::cout << "OpenJPEG v2 \n";
  TEST("file1 (768x512 8Bit RGB)", image_equals<vxl_byte>("jpeg2000/file1.jp2", "jpeg2000/opj_file1.tif"), true);
  TEST("file2 (480x640 8Bit RGB)", image_equals<vxl_byte>("jpeg2000/file2.jp2", "jpeg2000/opj_file2.tif"), true);
  TEST("file4 (768x512 8Bit Grey)", image_equals<vxl_byte>("jpeg2000/file4.jp2", "jpeg2000/opj_file4.tif"), true);
  TEST("file5 (768x512 8Bit RGB)", image_equals<vxl_byte>("jpeg2000/file5.jp2", "jpeg2000/opj_file5.tif"), true);
  //TEST("file6 (768x512 12Bit Grey)", image_diff_peak<vxl_uint_16>("jpeg2000/file6.jp2", "jpeg2000/opj_file6.tif"), 0);
  TEST("file7 (480x640 16Bit RGB)", image_equals<vxl_uint_16>("jpeg2000/file7.jp2", "jpeg2000/opj_file7.tif"), true);
  TEST("file8 (700x400 8Bit RGB)", image_equals<vxl_byte>("jpeg2000/file8.jp2", "jpeg2000/opj_file8.tif"), true);
  TEST("file9 (768x512 8Bit RGB)", image_equals<vxl_byte>("jpeg2000/file9.jp2", "jpeg2000/opj_file9.tif"), true);
#endif

  std::cout << "Sun raster [ras]\n";
  TEST("8-bit grey, no colourmap", CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit_nocol.ras" ), true);
  TEST("8-bit RGB, no colourmap", CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_raw.ras" ), true);
  TEST("8-bit indexed RGB", CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_indexed.ras" ), true);

  std::cout << "Khoros VIFF [viff]\n";
  TEST("8-bit grey big endian", CheckFile(CompareGrey<vxl_uint_8>(), "ff_grey8bit_true.txt", "ff_grey8bit_bigendian.viff" ), true);
  TEST("8-bit RGB big endian", CheckFile(ComparePlanes<vxl_uint_8,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit_bigendian.viff" ), true);
  TEST("16-bit grey big endian", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_bigendian.viff" ), true);
  TEST("16-bit RGB big endian", CheckFile(ComparePlanes<vxl_uint_16,3>(), "ff_planar16bit_true.txt", "ff_rgb16bit_bigendian.viff" ), true);
  TEST("32-bit grey big endian", CheckFile(CompareGrey<vxl_uint_32>(), "ff_grey32bit_true.txt", "ff_grey32bit_bigendian.viff" ), true);
  TEST("32-bit float grey big endian", CheckFile(CompareGreyFloat<float>(), "ff_grey_float_true.txt", "ff_grey_float_bigendian.viff" ), true);
  TEST("64-bit float grey big endian", CheckFile(CompareGreyFloat<double>(), "ff_grey_float_true.txt", "ff_grey_double_bigendian.viff" ), true);

  TEST("8-bit grey little endian", CheckFile(CompareGrey<vxl_uint_8>(), "ff_grey8bit_true.txt", "ff_grey8bit_littleendian.viff" ), true);
  TEST("8-bit RGB little endian", CheckFile(ComparePlanes<vxl_uint_8,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit_littleendian.viff" ), true);
  TEST("16-bit grey little endian", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_littleendian.viff" ), true);
  TEST("16-bit RGB little endian", CheckFile(ComparePlanes<vxl_uint_16,3>(), "ff_planar16bit_true.txt", "ff_rgb16bit_littleendian.viff" ), true);
  TEST("32-bit grey little endian", CheckFile(CompareGrey<vxl_uint_32>(), "ff_grey32bit_true.txt", "ff_grey32bit_littleendian.viff" ), true);
  TEST("32-bit float grey little endian", CheckFile(CompareGreyFloat<float>(), "ff_grey_float_true.txt", "ff_grey_float_littleendian.viff" ), true);
  TEST("64-bit float grey little endian", CheckFile(CompareGreyFloat<double>(), "ff_grey_float_true.txt", "ff_grey_double_littleendian.viff" ), true);

  std::cout << "MIT [mit]\n";
  TEST("8-bit grey", CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit.mit" ), true);
  TEST("16-bit grey", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit.mit" ), true);
  TEST("8-bit RGB", CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit.mit" ), true);

#if HAS_DCMTK
  std::cout << "DICOM [dcm]\n";
  TEST("16-bit greyscale uncompressed", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true_for_dicom.txt", "ff_grey16bit_uncompressed.dcm"), true);
  // These only pass if the DCMTK-based DICOM loader is available
  TEST("16-bit greyscale uncompressed 2", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_uncompressed2.dcm" ), true);
  TEST("8-bit greyscale uncompressed", CheckFile(CompareGrey<vxl_uint_8>(), "ff_grey8bit_true.txt", "ff_grey8bit_uncompressed.dcm" ), true);
  TEST("8-bit greyscale uncompressed 2", CheckFile(CompareGrey<vxl_uint_8>(), "ff_grey8bit_true.txt", "ff_grey8bit_uncompressed2.dcm" ), true);
  TEST("16-bit greyscale uncompressed 3", CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_uncompressed3.dcm" ), true);
  TEST("12-bit greyscale float uncompressed", CheckFile(CompareGreyFloat<float>(), "ff_grey_float_true_for_dicom.txt", "ff_grey_float_12bit_uncompressed.dcm" ), true);
#endif // HAS_DCMTK
}

TESTMAIN_ARGS(test_file_format_read);
