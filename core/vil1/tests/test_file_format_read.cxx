#include <vil/vil_test.h>

#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>
#include <vil/vil_load.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>

#include <vxl_config.h>

// Amitha Perera
// Apr 2002

// Compare the results of loading different files with the true data
// that's supposed to be in those files. Only deals with single plane
// images so far.

vcl_string image_base;

typedef vxl_uint_32 TruePixelType;

class CheckPixel
{
public:
  virtual ~CheckPixel() { }
  virtual bool operator() ( int p, int x, int y, const vcl_vector<TruePixelType>& pixel ) const = 0;
};

template<class T>
class CheckRGB : public CheckPixel
{
public:
  CheckRGB( const char* file )
  {
    vil_image i = vil_load( (image_base + file).c_str() );
    if( !i )
      vcl_cout << "[ couldn't load " << file << "]\n";
    else
      img_ = i;
  };

  bool operator() ( int p, int x, int y, const vcl_vector<TruePixelType>& pixel ) const
  {
    assert( p == 0 );
    return img_ && pixel.size() == 3 && pixel[0] == img_(x,y).r && pixel[1] == img_(x,y).g && pixel[2] == img_(x,y).b;
  } 
private:
  vil_memory_image_of< vil_rgb<T> > img_;
};

template<class T>
class CheckGrey : public CheckPixel
{
public:
  CheckGrey( const char* file )
  {
    vil_image i = vil_load( (image_base + file).c_str() );
    if( !i )
      vcl_cout << "[ couldn't load " << file << "]\n";
    else
      img_ = i;
  };

  bool operator() ( int p, int x, int y, const vcl_vector<TruePixelType>& pixel ) const
  {
    assert( p == 0 );
    return img_ && pixel.size() == 1 && pixel[0] == img_(x,y);
  } 
private:
  vil_memory_image_of< T > img_;
};

template class CheckRGB< vxl_uint_8 >;
template class CheckGrey< vxl_uint_8 >;


bool
test( const char* true_data_file, const CheckPixel& check )
{
  // The true data is a ASCII file consisting of a sequence of numbers. The first set of numbers are:
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
  if( !( fin >> num_planes >> num_comp >> width >> height ) ) {
    vcl_cout << "[couldn't read header from " << true_data_file << "]";
    return false;
  }

  vcl_vector<TruePixelType> pixel( num_comp );

  for( int p=0; p < num_planes; ++p ) {
    for( int y=0; y < height; ++y ) {
      for( int x=0; x < width; ++x ) {
        for( int c=0; c < num_comp; ++c ) {
          if( !( fin >> pixel[c] ) ) {
            vcl_cout << "[couldn't read value at " << p << "," << x << "," << y << "," << c
                     << " from " << true_data_file << "]";
            return false;
          }
        }
        if( !check( p, x, y, pixel ) )
          return false;
      }
    }
  }

  return true;
}

int
main( int argc, char* argv[] )
{
  if( argc >= 2 ) {
    image_base = argv[1];
#ifdef VCL_WIN32
    image_base += "\\";
#else
    image_base += "/";
#endif
  }

  vil_test_start("test_file_format_read");

  vcl_cout << "Portable aNy Map [pnm]: pbm, pgm, ppm)\n";
  vil_test_begin( "  8-bit pgm ascii" );
  vil_test_perform( test( "ff_grey8bit_true.txt", CheckGrey<vxl_uint_8>( "ff_grey8bit_ascii.pgm" ) ) );
  vil_test_begin( "  8-bit pgm raw" );
  vil_test_perform( test( "ff_grey8bit_true.txt", CheckGrey<vxl_uint_8>( "ff_grey8bit_raw.pgm" ) ) );
  vil_test_begin( "  8-bit ppm ascii" );
  vil_test_perform( test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_ascii.ppm" ) ) );
  vil_test_begin( "  8-bit ppm raw" );
  vil_test_perform( test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_raw.ppm" ) ) );

  vcl_cout << "Sun raster [ras]\n";
  vil_test_begin( "  8-bit grey, no colourmap" );
  vil_test_perform( test( "ff_grey8bit_true.txt", CheckGrey<vxl_uint_8>( "ff_grey8bit_nocol.ras" ) ) );
  vil_test_begin( "  8-bit RGB, no colourmap" );
  vil_test_perform( test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_raw.ras" ) ) );
  vil_test_begin( "  8-bit indexed RGB" );
  vil_test_perform( test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_indexed.ras" ) ) );

//    vcl_cout << "Windows bitmap [bmp]\n";
//    vil_test_begin( "  8-bit RGB (xv created)" );
//    vil_test_perform( test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_xv.bmp" ) ) );

  vcl_cout << "TIFF [tiff]\n";
  vil_test_begin( "  8-bit RGB uncompressed" );
  vil_test_perform( test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_uncompressed.tif" ) ) );
  vil_test_begin( "  8-bit RGB packbits" );
  vil_test_perform( test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_packbits.tif" ) ) );

//    vcl_cout << "SGI IRIS [iris]\n";
//    vil_test_begin( "  8-bit RGB rle" );
//    vil_test_perform( test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit.iris" ) ) );

  vcl_cout << "Portable Network Graphics [png]\n";
  vil_test_begin( "  8-bit RGB uncompressed" );
  vil_test_perform( test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_uncompressed.png" ) ) );
  vil_test_begin( "  8-bit RGB compressed" );
  vil_test_perform( test( "ff_rgb8bit_true.txt", CheckRGB<vxl_uint_8>( "ff_rgb8bit_compressed.png" ) ) );

  return vil_test_summary();
}
