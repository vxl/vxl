// This is core/vil/tests/test_file_format_read.cxx
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vxl_config.h> // for vxl_uint_16 etc.

#include <testlib/testlib_test.h>

#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/vil_config.h>

// For testing specific file formats
#include <vil/vil_stream_fstream.h>

//#define DEBUG

// Amitha Perera
// Apr 2002

// Compare the results of loading different files with the true data
// that's supposed to be in those files.

vcl_string image_base;


// A comparator interface to check if the pixels match. It will be
// called with all the components at a single location (given by i, j
// and p).
//
template<class TruePixelType, class ImgPixelType>
struct Compare
{
  virtual bool operator() ( vil_image_view<ImgPixelType> const&,
                            int p, int i, int j,
                            const vcl_vector<TruePixelType>& pixel ) const = 0;
};


// Below are various "real" comparators


template<class PixelType>
struct CompareRGB
  : public Compare<PixelType, vil_rgb<PixelType> >
{
  virtual bool operator() ( vil_image_view< vil_rgb<PixelType> > const& img,
                            int p, int i, int j,
                            const vcl_vector<PixelType>& pixel ) const
    {
      return p==0 && pixel.size() == 3 &&
             img(i,j).r == pixel[0] &&
             img(i,j).g == pixel[1] &&
             img(i,j).b == pixel[2];
    }
};


template<class PixelType>
struct CompareRGBNear
  : public Compare<PixelType, vil_rgb<PixelType> >
{
  virtual bool operator() ( vil_image_view< vil_rgb<PixelType> > const& img,
                            int p, int i, int j,
                            const vcl_vector<PixelType>& pixel ) const
    {
      if ( p == 0 && pixel.size() == 3 ) {
        // Find difference in two values whilst avoiding unsigned underflow
        PixelType diff_A = pixel[0]*pixel[0] + img(i,j).r * img(i,j).r +
                           pixel[1]*pixel[1] + img(i,j).g * img(i,j).g +
                           pixel[2]*pixel[2] + img(i,j).b * img(i,j).b;

        PixelType diff_B = 2 * pixel[0] * img(i,j).r + tol_sq_ +
                           2 * pixel[1] * img(i,j).g + tol_sq_ +
                           2 * pixel[2] * img(i,j).b + tol_sq_ ;
        return diff_A < diff_B;
      } else {
        return false;
      }
    }

  CompareRGBNear( PixelType tol )
    : tol_sq_( tol*tol )
    { }

  PixelType tol_sq_;
};


template<class PixelType, int num_planes>
struct ComparePlanes
  : public Compare<PixelType, PixelType >
{
  virtual bool operator() ( vil_image_view<PixelType> const& img,
                            int p, int i, int j,
                            const vcl_vector<PixelType>& pixel ) const
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


template<class PixelType>
struct CompareGreyFloat
  : public Compare<PixelType,PixelType>
{
  virtual bool operator() ( vil_image_view<PixelType> const& img,
                            int p, int i, int j,
                            const vcl_vector<PixelType>& pixel ) const
    {
      return p==0 && pixel.size() == 1 &&
             vcl_fabs( pixel[0] - img(i,j) ) <= 1e-6 * vcl_fabs( pixel[0] );
    }
};


template<class PixelType>
struct CompareGreyNear
  : public Compare<PixelType,PixelType>
{
  virtual bool operator() ( vil_image_view<PixelType> const& img,
                            int p, int i, int j,
                            const vcl_vector<PixelType>& pixel ) const
    {
      if ( p==0 && pixel.size() == 1 ) {
        // Find difference in two values whilst avoiding unsigned underflow
        PixelType diff_A = pixel[0] * pixel[0] + img(i,j) * img(i,j);
        PixelType diff_B = 2 * pixel[0] * img(i,j) + tol_sq_ ;
        return diff_A <= diff_B;
      } else {
        return false;
      }
    }

  CompareGreyNear( PixelType tol )
    : tol_sq_( tol*tol )
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
read_value( vcl_istream& fin, TruePixelType& pix )
{
  return ! (fin >> pix);
}

// Specialization to make char read as small integers and not characters
// See comments on template for return value.
VCL_DEFINE_SPECIALIZATION
bool
read_value( vcl_istream& fin, char& pix )
{
  int x;
  // use operator! to test the stream to avoid compiler warnings
  bool bad = ! ( fin >> x );
  if ( !bad ) pix = x;
  return bad;
}


// Specialization to make char read as small integers and not characters
// See comments on template for return value.
VCL_DEFINE_SPECIALIZATION
bool
read_value( vcl_istream& fin, unsigned char& pix )
{
  int x;
  // use operator! to test the stream to avoid compiler warnings
  bool bad = ! ( fin >> x );
  if ( !bad ) pix = x;
  return bad;
}


// Specialization to make char read as small integers and not characters
// See comments on template for return value.
VCL_DEFINE_SPECIALIZATION
bool
read_value( vcl_istream& fin, signed char& pix )
{
  int x;
  // use operator! to test the stream to avoid compiler warnings
  bool bad = ! ( fin >> x );
  if ( !bad ) pix = x;
  return bad;
}

// Specialization to make bool read as 0/1 integers
// See comments on template for return value.
VCL_DEFINE_SPECIALIZATION
bool
read_value( vcl_istream& fin, bool& pix )
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
             vil_image_resource_sptr ir )
{
  // The true data is a ASCII file consisting of a sequence of
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

  vcl_ifstream fin( (image_base+true_data_file).c_str() );
  if ( !( fin >> num_planes >> num_comp >> width >> height ) ) {
    vcl_cout << "[couldn't read header from " << true_data_file << ']' << vcl_flush;
    return false;
  }


  // Generate different views from the resource, including the full image.
  //
  vcl_vector< vil_image_view<ImgPixelType> > views;
  vcl_vector< unsigned > offi;
  vcl_vector< unsigned > offj;
  for ( unsigned dl = 0; dl < 3; ++dl ) {
    for ( unsigned dt = 0; dt < 2; ++dt ) {
      for ( unsigned dr = 0; dr < 3; ++dr ) {
        for ( unsigned db = 0; db < 2; ++db ) {
          if ( dl+dr < ir->ni() && dt+db < ir->nj() ) {
            vil_image_view_base_sptr im =
              ir->get_copy_view( dl, ir->ni()-dl-dr, dt, ir->nj()-dt-db );
            if ( !im ) {
              vcl_cout << "[ couldn't read (sub)image data from " << ir << "]\n"
                       << "[ off=(" << dl << ',' << dt
                       << ")x(" << ir->ni()-dl-dr << ','
                       << ir->nj()-dt-db << " ]" << vcl_endl;
              return false;
            }
            vil_image_view<ImgPixelType> img = im;
            if ( !img ) {
              vcl_cout << "[ couldn't read (sub)image data of the expected format from "
                       << im << " at offset " << dl << ',' << dt << ']' << vcl_endl;
              return false;
            }

            if ( img.ni() != ir->ni()-dl-dr || img.nj() != ir->nj()-dt-db ) {
              vcl_cout << "[ (sub)image has wrong size (!= "
                       << ir->ni()-dl-dr << 'x' << ir->nj()-dt-db
                       << ") in " << img << vcl_endl;
              return false;
            }

#ifdef DEBUG
            if (img.size() < 100) {
              vcl_cout << "\nSubimage " << views.size() << " at ("<<dl<<','<<dt<<"):\n";
              vil_print_all(vcl_cout, img); vcl_cout.flush();
            } else {
              vcl_cout << "Subimage size = " << img.size() << ".  Too large to display all pixels."
                       << vcl_endl;
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

  vcl_vector<TruePixelType> pixel( num_comp );

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
            vcl_cout << "View " << v << " at offset (" << offi[v] << ',' << offj[v]
                     << ") [ " << views[v] << " ] has a mismatch at pixel (p="
                     << p << ",i=" << i << ",j=" << j << ')' << vcl_endl;
            if ( views[v].size() < 100 ) {
              vil_print_all( vcl_cout, views[v] );
              vcl_cout.flush();
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
    vcl_cout << "[ couldn't load image file " << img_data_file << " ]" << vcl_endl;
    return false;
  } else {
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
      vcl_cout << "[ couldn't load image file " << img_data_file << " ]" << vcl_endl;
      result = false;
    } else {
      result = CheckPixels( check, true_data_file, ir );
    }
  } else {
    vcl_cout << "[ couldn't open file " << img_data_file << " for reading ]" << vcl_endl;
    result = false;
  }
  is->unref();
  return result;
}


int
test_file_format_read_main( int argc, char* argv[] )
{
  if ( argc >= 2 ) {
    image_base = argv[1];
#ifdef VCL_WIN32
    image_base += "\\";
#else
    image_base += "/";
#endif
  }

  testlib_test_start(" file format read");

  // Test generic file loads

  vcl_cout << "GENERIC FILE LOAD\n\n"

           << "Portable aNy Map [pnm]: pbm, pgm, ppm)\n";
  testlib_test_begin( "  1-bit pbm ascii" );
  testlib_test_perform(CheckFile(CompareGrey<bool>(), "ff_grey1bit_true.txt", "ff_grey1bit_ascii.pbm" ) );
  testlib_test_begin( "  1-bit pbm raw" );
  testlib_test_perform(CheckFile(CompareGrey<bool>(), "ff_grey1bit_true.txt", "ff_grey1bit_raw.pbm" ) );
  testlib_test_begin( "  8-bit pgm ascii" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit_ascii.pgm" ) );
  testlib_test_begin( "  8-bit pgm raw" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit_raw.pgm" ) );
  testlib_test_begin( " 16-bit pgm ascii" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_ascii.pgm" ) );
  testlib_test_begin( " 16-bit pgm raw" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_raw.pgm" ) );
  testlib_test_begin( "  8-bit ppm ascii" );
  testlib_test_perform(CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_ascii.ppm" ) );
  testlib_test_begin( "  8-bit ppm ascii as planar" );
  testlib_test_perform(CheckFile(ComparePlanes<vxl_byte,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit_ascii.ppm" ) );
  testlib_test_begin( "  8-bit ppm raw" );
  testlib_test_perform(CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_raw.ppm" ) );
  testlib_test_begin( "  8-bit ppm raw as planar" );
  testlib_test_perform(CheckFile(ComparePlanes<vxl_byte,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit_raw.ppm" ) );
  testlib_test_begin( " 16-bit ppm ascii" );
  testlib_test_perform(CheckFile(CompareRGB<vxl_uint_16>(), "ff_rgb16bit_true.txt", "ff_rgb16bit_ascii.ppm" ) );
  testlib_test_begin( " 16-bit ppm raw" );
  testlib_test_perform(CheckFile(CompareRGB<vxl_uint_16>(), "ff_rgb16bit_true.txt", "ff_rgb16bit_raw.ppm" ) );

  vcl_cout << "JPEG [jpg]\n";
  testlib_test_begin( "  8-bit grey, normal image to 4 quanta" );
  testlib_test_perform(CheckFile(CompareGreyNear<vxl_byte>(4), "ff_grey8bit_true.txt", "ff_grey8bit_compressed.jpg" ) );
  testlib_test_begin( "  8-bit RGB, easy image accurate to 3 quanta" );
  testlib_test_perform(CheckFile(CompareRGBNear<vxl_byte>(3), "ff_rgb8biteasy_true.txt", "ff_rgb8biteasy_compressed.jpg" ) );

  vcl_cout << "Windows bitmap [bmp]\n";
  testlib_test_begin( "  8-bit greyscale (xv created)" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit.bmp" ) );
  testlib_test_begin( "  8-bit RGB (xv created)" );
  testlib_test_perform(CheckFile(ComparePlanes<vxl_byte,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit_xv.bmp" ) );

  vcl_cout << "Portable Network Graphics [png]\n";
  testlib_test_begin( "  8-bit RGB uncompressed" );
  testlib_test_perform(CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_uncompressed.png" ) );
  testlib_test_begin( "  8-bit RGB compressed" );
  testlib_test_perform(CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_compressed.png" ) );

  vcl_cout << "TIFF [tiff]\n";
  testlib_test_begin( "  8-bit RGB uncompressed" );
  testlib_test_perform(CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_uncompressed.tif" ) );
  testlib_test_begin( "  8-bit RGB packbits" );
  testlib_test_perform(CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_packbits.tif" ) );

  vcl_cout << "Sun raster [ras]\n";
  testlib_test_begin( "  8-bit grey, no colourmap" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit_nocol.ras" ) );
  testlib_test_begin( "  8-bit RGB, no colourmap" );
  testlib_test_perform(CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_raw.ras" ) );
  testlib_test_begin( "  8-bit indexed RGB" );
  testlib_test_perform(CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit_indexed.ras" ) );

  vcl_cout << "Khoros VIFF [viff]\n";
  testlib_test_begin( "  8-bit grey big endian" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_8>(), "ff_grey8bit_true.txt", "ff_grey8bit_bigendian.viff" ) );
  testlib_test_begin( "  8-bit RGB big endian" );
  testlib_test_perform(CheckFile(ComparePlanes<vxl_uint_8,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit_bigendian.viff" ) );
  testlib_test_begin( "  16-bit grey big endian" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_bigendian.viff" ) );
  testlib_test_begin( "  16-bit RGB big endian" );
  testlib_test_perform(CheckFile(ComparePlanes<vxl_uint_16,3>(), "ff_planar16bit_true.txt", "ff_rgb16bit_bigendian.viff" ) );
  testlib_test_begin( "  32-bit grey big endian" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_32>(), "ff_grey32bit_true.txt", "ff_grey32bit_bigendian.viff" ) );
  testlib_test_begin( "  32-bit float grey big endian" );
  testlib_test_perform(CheckFile(CompareGreyFloat<float>(), "ff_grey_float_true.txt", "ff_grey_float_bigendian.viff" ) );
  testlib_test_begin( "  64-bit float grey big endian" );
  testlib_test_perform(CheckFile(CompareGreyFloat<double>(), "ff_grey_float_true.txt", "ff_grey_double_bigendian.viff" ) );

  testlib_test_begin( "  8-bit grey little endian" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_8>(), "ff_grey8bit_true.txt", "ff_grey8bit_littleendian.viff" ) );
  testlib_test_begin( "  8-bit RGB little endian" );
  testlib_test_perform(CheckFile(ComparePlanes<vxl_uint_8,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit_littleendian.viff" ) );
  testlib_test_begin( "  16-bit grey little endian" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_littleendian.viff" ) );
  testlib_test_begin( "  16-bit RGB little endian" );
  testlib_test_perform(CheckFile(ComparePlanes<vxl_uint_16,3>(), "ff_planar16bit_true.txt", "ff_rgb16bit_littleendian.viff" ) );
  testlib_test_begin( "  32-bit grey little endian" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_32>(), "ff_grey32bit_true.txt", "ff_grey32bit_littleendian.viff" ) );
  testlib_test_begin( "  32-bit float grey little endian" );
  testlib_test_perform(CheckFile(CompareGreyFloat<float>(), "ff_grey_float_true.txt", "ff_grey_float_littleendian.viff" ) );
  testlib_test_begin( "  64-bit float grey little endian" );
  testlib_test_perform(CheckFile(CompareGreyFloat<double>(), "ff_grey_float_true.txt", "ff_grey_double_littleendian.viff" ) );

  vcl_cout << "SGI IRIS [iris]\n";
  testlib_test_begin( "  8-bit grey rle" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit.iris" ) );
  testlib_test_begin( "  16-bit grey verbatim" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit.iris" ) );
  testlib_test_begin( "  8-bit RGB rle" );
  testlib_test_perform(CheckFile(ComparePlanes<vxl_byte,3>(), "ff_planar8bit_true.txt", "ff_rgb8bit.iris" ) );

  vcl_cout << "MIT [mit]\n";
  testlib_test_begin( "  8-bit grey" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_byte>(), "ff_grey8bit_true.txt", "ff_grey8bit.mit" ) );
  testlib_test_begin( "  16-bit grey" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit.mit" ) );
  testlib_test_begin( "  8-bit RGB" );
  testlib_test_perform(CheckFile(CompareRGB<vxl_byte>(), "ff_rgb8bit_true.txt", "ff_rgb8bit.mit" ) );

#if HAS_DCMTK
  vcl_cout << "DICOM [dcm]\n";
  testlib_test_begin( "  16-bit greyscale uncompressed" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true_for_dicom.txt", "ff_grey16bit_uncompressed.dcm"));
  // These only pass if the DCMTK-based DICOM loader is available
  testlib_test_begin( "  16-bit greyscale uncompressed 2" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_uncompressed2.dcm" ) );
  testlib_test_begin( "  8-bit greyscale uncompressed" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_8>(), "ff_grey8bit_true.txt", "ff_grey8bit_uncompressed.dcm" ) );
  testlib_test_begin( "  16-bit greyscale uncompressed 3" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true.txt", "ff_grey16bit_uncompressed3.dcm" ) );
  testlib_test_begin( "  12-bit greyscale float uncompressed" );
  testlib_test_perform(CheckFile(CompareGreyFloat<float>(), "ff_grey_float_true_for_dicom.txt",
                                                            "ff_grey_float_12bit_uncompressed.dcm" ) );
#endif // HAS_DCMTK

#if 0 // these are broken
  vcl_cout << "NITF [NITF v2.0]\n";
  testlib_test_begin( "  8-bit grey" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_8>(), "ff_grey8bit_true_for_nitf.txt", "ff_grey8bit_uncompressed.nitf" ) );
  testlib_test_begin( "  16-bit grey (actually 11-bit)" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_16>(), "ff_grey16bit_true_for_nitf.txt", "ff_grey16bit_uncompressed.nitf"));
  // ONLY 8 BIT AND 16 BIT GREY ARE VALID TESTS FOR NITF NOW.
#endif // these are broken too
#if 0
  testlib_test_begin( "  8-bit RGB" );
  testlib_test_perform(CheckFile(ComparePlanes<vxl_uint_,38>(), "ff_planar8bit_true.txt", "ff_rgb8bit_uncompressed.nitf" ) );
  testlib_test_begin( "  16-bit RGB" );
  testlib_test_perform(CheckFile(ComparePlanes<vxl_uint_16,3>(), "ff_planar16bit_true.txt", "ff_rgb16bit_uncompressed.nitf" ) );
  testlib_test_begin( "  32-bit grey" );
  testlib_test_perform(CheckFile(CompareGrey<vxl_uint_32>(), "ff_grey32bit_true.txt", "ff_grey32bit.nitf" ) );
  testlib_test_begin( "  32-bit float grey" );
  testlib_test_perform(CheckFile(CompareGreyFloat<float>(), "ff_grey_float_true.txt", "ff_grey_float.nitf" ) );
  testlib_test_begin( "  64-bit float grey" );
  testlib_test_perform(CheckFile(CompareGreyFloat<double>(), "ff_grey_float_true.txt", "ff_grey_double.nitf" ) );
#endif

  return testlib_test_summary();
}
