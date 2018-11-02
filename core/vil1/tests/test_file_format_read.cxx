// This is core/vil1/tests/test_file_format_read.cxx
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <testlib/testlib_test.h>

#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_load.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vxl_config.h> // for vxl_uint_16 etc.

// Amitha Perera
// Apr 2002

// Compare the results of loading different files with the true data
// that's supposed to be in those files.

static std::string image_base;

// Set to "double", which is the only type that can (roughly) represent all data types
typedef double TruePixelType;

class CheckPixel
{
 public:
  virtual ~CheckPixel() = default;
  virtual bool operator() ( int p, int x, int y, const std::vector<TruePixelType>& pixel ) const = 0;
};

template<class T>
class CheckRGB : public CheckPixel
{
 public:
  CheckRGB( const char* file )
  {
    vil1_image i = vil1_load( (image_base + file).c_str() );
    if ( !i )
      std::cout << "[ couldn't load " << file << "]\n";
    else
      img_ = i;
  };

  bool operator() ( int p, int x, int y, const std::vector<TruePixelType>& pixel ) const override
  {
    assert( p == 0 );
    if (!img_ || pixel.size() != 3) return false;
    vil1_rgb<T> imp = img_(x,y);
    if (pixel[0] != imp.r || pixel[1] != imp.g || pixel[2] != imp.b)
      std::cout << "(x,y)=(" << x << ',' << y << "): true="
               << pixel[0] << ',' << pixel[1] << ',' << pixel[2]
               << ", img=" << imp << '\n' << std::flush;
    return pixel[0] == imp.r && pixel[1] == imp.g && pixel[2] == imp.b;
  }
 private:
  vil1_memory_image_of< vil1_rgb<T> > img_;
};

template<class T>
class CheckColourPlanes : public CheckPixel
{
 public:
  CheckColourPlanes( const char* file )
  {
    vil1_image i = vil1_load( (image_base + file).c_str() );
    if ( !i )
      std::cout << "[ couldn't load " << file << "]\n";
    else
      img_ = i;
  };

  bool operator() ( int p, int x, int y, const std::vector<TruePixelType>& pixel ) const override
  {
    assert( p==0 || p==1 || p==2 );
    if (!img_ || pixel.size() != 1) return false;
    TruePixelType imp = img_.get_buffer()[img_.width()*(img_.height()*p+y)+x];
    if (pixel[0] != imp)
      std::cout << "(x,y,p)=(" << x << ',' << y << ',' << p << "): true="
               << pixel[0] << ", img=" << imp << '\n' << std::flush;
    return pixel[0] == imp;
  }
 private:
  vil1_memory_image_of< T > img_;
};

template<class T>
class CheckGrey : public CheckPixel
{
 public:
  CheckGrey( const char* file )
  {
    vil1_image i = vil1_load( (image_base + file).c_str() );
    if ( !i )
      std::cout << "[ couldn't load " << file << "]\n";
    else
      img_ = i;
  };

  bool operator() ( int p, int x, int y, const std::vector<TruePixelType>& pixel ) const override
  {
    assert( p == 0 );
    if (!img_ || pixel.size() != 1) return false;
    TruePixelType imp = img_(x,y);
    if (pixel[0] != imp)
      std::cout << "(x,y)=(" << x << ',' << y << "): true=" << pixel[0] << ", img=" << imp << '\n' << std::flush;
    // no exact check, to allow for small rounding errors in float pixel types:
    return std::fabs(pixel[0] - imp) <= 1e-7*std::fabs(imp);
  }
 private:
  vil1_memory_image_of< T > img_;
};

template<class T>
class CheckBit : public CheckPixel
{
 public:
  CheckBit( const char* file )
  {
    vil1_image i = vil1_load( (image_base + file).c_str() );
    if ( !i )
      std::cout << "[ couldn't load " << file << "]\n";
    else
      img_ = i;
  };

  bool operator() ( int p, int x, int y, const std::vector<TruePixelType>& pixel ) const override
  {
    assert( p == 0 );
    if (!img_ || pixel.size() != 1) return false;
    TruePixelType imp = (img_(x/8,y)>>(7-(x&7)))&1;
    if (pixel[0] != imp)
      std::cout << "(x,y)=(" << x << ',' << y << "): true=" << pixel[0] << ", img=" << imp << '\n' << std::flush;
    return pixel[0] == imp;
  }
 private:
  vil1_memory_image_of< T > img_;
};

template class CheckBit< vxl_uint_8 >;
template class CheckGrey< vxl_uint_8 >;
template class CheckGrey< vxl_uint_16 >;
template class CheckGrey< vxl_uint_32 >;
template class CheckGrey< float >;
template class CheckGrey< double >;
template class CheckRGB< vxl_uint_8 >;
template class CheckRGB< vxl_uint_16 >;
template class CheckColourPlanes< vxl_uint_8 >;
template class CheckColourPlanes< vxl_uint_16 >;

bool
test( const char* true_data_file, const CheckPixel& check )
{
  // The true data is an ASCII file consisting of a sequence of numbers. The first set of numbers are:
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
    std::cout << "[couldn't read header from " << true_data_file << ']';
    return false;
  }

  std::vector<TruePixelType> pixel( num_comp );

  for ( int p=0; p < num_planes; ++p ) {
    for ( int y=0; y < height; ++y ) {
      for ( int x=0; x < width; ++x ) {
        for ( int c=0; c < num_comp; ++c ) {
          if ( !( fin >> pixel[c] ) ) {
            std::cout << "[couldn't read value at " << p << ',' << x << ',' << y << ',' << c
                     << " from " << true_data_file << ']';
            return false;
          }
        }
        if ( !check( p, x, y, pixel ) )
          return false;
      }
    }
  }

  return true;
}

static void
test_file_format_read( int argc, char* argv[] )
{
  if ( argc >= 2 ) {
    image_base = argv[1];
    image_base += "/";
  }

  std::cout << "Portable aNy Map [pnm]: pbm, pgm, ppm)\n";
  TEST("1-bit pbm ascii", test( "ff_grey1bit_true.txt", CheckBit<vxl_uint_8>( "ff_grey1bit_ascii.pbm" ) ), true);
  TEST("1-bit pbm raw", test( "ff_grey1bit_true.txt", CheckBit<vxl_uint_8>( "ff_grey1bit_raw.pbm" ) ), true);
  TEST("8-bit pgm ascii", test( "ff_grey8bit_true.txt", CheckGrey<vxl_uint_8>( "ff_grey8bit_ascii.pgm" ) ), true);
  TEST("8-bit pgm raw", test( "ff_grey8bit_true.txt", CheckGrey<vxl_uint_8>( "ff_grey8bit_raw.pgm" ) ), true);
  TEST("8-bit ppm ascii", test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_ascii.ppm" ) ), true);
  TEST("8-bit ppm raw", test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_raw.ppm" ) ), true);
  TEST("16-bit pgm ascii", test( "ff_grey16bit_true.txt", CheckGrey<vxl_uint_16>( "ff_grey16bit_ascii.pgm" ) ), true);
  TEST("16-bit pgm raw", test( "ff_grey16bit_true.txt", CheckGrey<vxl_uint_16>( "ff_grey16bit_raw.pgm" ) ), true);
  TEST("16-bit ppm ascii", test( "ff_rgb16bit_true.txt", CheckRGB<vxl_uint_16>( "ff_rgb16bit_ascii.ppm" ) ), true);
  TEST("16-bit ppm raw", test( "ff_rgb16bit_true.txt", CheckRGB<vxl_uint_16>( "ff_rgb16bit_raw.ppm" ) ), true);

  std::cout << "Sun raster [ras]\n";
  TEST("8-bit grey, no colourmap", test( "ff_grey8bit_true.txt", CheckGrey<vxl_uint_8>( "ff_grey8bit_nocol.ras" ) ), true);
  TEST("8-bit RGB, no colourmap", test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_raw.ras" ) ), true);
  TEST("8-bit indexed RGB", test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_indexed.ras" ) ), true);

  std::cout << "Windows bitmap [bmp]\n";
  TEST("8-bit RGB (xv created)", test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_xv.bmp" ) ), true);

  std::cout << "TIFF [tiff]\n";
  TEST("1-bit grey uncompressed", test( "ff_grey1bit_true.txt", CheckBit<vxl_uint_8>( "ff_grey1bit_uncompressed.tif" ) ), true);
  TEST("8-bit RGB uncompressed", test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_uncompressed.tif" ) ), true);
  TEST("8-bit RGB packbits", test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_packbits.tif" ) ), true);
  TEST("16-bit RGB uncompressed", test( "ff_rgb16bit_true.txt", CheckRGB<vxl_uint_16>( "ff_rgb16bit_uncompressed.tif" ) ), true);

  std::cout << "Khoros VIFF [viff]\n";
  TEST("8-bit grey big endian", test( "ff_grey8bit_true.txt", CheckGrey<vxl_uint_8>( "ff_grey8bit_bigendian.viff" ) ), true);
  TEST("8-bit RGB big endian", test( "ff_planar8bit_true.txt", CheckColourPlanes<vxl_uint_8>( "ff_rgb8bit_bigendian.viff" ) ), true);
  TEST("16-bit grey big endian", test( "ff_grey16bit_true.txt", CheckGrey<vxl_uint_16>( "ff_grey16bit_bigendian.viff" ) ), true);
  TEST("16-bit RGB big endian", test( "ff_planar16bit_true.txt", CheckColourPlanes<vxl_uint_16>( "ff_rgb16bit_bigendian.viff" ) ), true);
  TEST("32-bit grey big endian", test( "ff_grey32bit_true.txt", CheckGrey<vxl_uint_32>( "ff_grey32bit_bigendian.viff" ) ), true);
  TEST("32-bit float grey big endian", test( "ff_grey_float_true.txt", CheckGrey<float>( "ff_grey_float_bigendian.viff" ) ), true);
  TEST("64-bit float grey big endian", test( "ff_grey_float_true.txt", CheckGrey<double>( "ff_grey_double_bigendian.viff" ) ), true);

  TEST("8-bit grey little endian", test( "ff_grey8bit_true.txt", CheckGrey<vxl_uint_8>( "ff_grey8bit_littleendian.viff" ) ), true);
  TEST("8-bit RGB little endian", test( "ff_planar8bit_true.txt", CheckColourPlanes<vxl_uint_8>( "ff_rgb8bit_littleendian.viff" ) ), true);
  TEST("16-bit grey little endian", test( "ff_grey16bit_true.txt", CheckGrey<vxl_uint_16>( "ff_grey16bit_littleendian.viff" ) ), true);
  TEST("16-bit RGB little endian", test( "ff_planar16bit_true.txt", CheckColourPlanes<vxl_uint_16>( "ff_rgb16bit_littleendian.viff" ) ), true);
  TEST("32-bit grey little endian", test( "ff_grey32bit_true.txt", CheckGrey<vxl_uint_32>( "ff_grey32bit_littleendian.viff" ) ), true);
  TEST("32-bit float grey little endian", test( "ff_grey_float_true.txt", CheckGrey<float>( "ff_grey_float_littleendian.viff" ) ), true);
  TEST("64-bit float grey little endian", test( "ff_grey_float_true.txt", CheckGrey<double>( "ff_grey_double_littleendian.viff" ) ), true);

  std::cout << "Portable Network Graphics [png]\n";
  TEST("8-bit RGB uncompressed", test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_uncompressed.png" ) ), true);
  TEST("8-bit RGB compressed", test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_compressed.png" ) ), true);
}

TESTMAIN_ARGS(test_file_format_read);
