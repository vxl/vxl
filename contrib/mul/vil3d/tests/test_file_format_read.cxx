// This is mul/vil2/tests/test_file_format_read.cxx
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>

#include <vxl_config.h> // for vxl_uint_16 etc.

#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_print.h>

#define DEBUG

// Ian Scott
// Mar 2003

// Compare the results of loading different files with the true data
// that's supposed to be in those files. Only deals with single plane
// images so far.

typedef vxl_uint_32 TruePixelType;

class CheckPixel
{
 public:
  virtual ~CheckPixel() { }
  virtual bool operator() ( int p, int i, int j, int k, const vcl_vector<TruePixelType>& pixel ) const = 0;
};

template<class T>
class CheckGrey : public CheckPixel
{
 public:
  CheckGrey( const char* file )
  {
    vil3d_image_resource_sptr ir = vil3d_load_image_resource(file );
    if ( !ir )
      vcl_cout << "[ couldn't read header from " << file << " ]\n";
    vil3d_image_view_base_sptr im = ir->get_copy_view(0,ir->ni(),0,ir->nj(),0,ir->nk());
    if ( !im )
      vcl_cout << "[ couldn't read image data from " << file << " ]\n";
    else
    {
      img_ = im;
#ifdef DEBUG
      vcl_cout << '\n' << vcl_flush; vil2_print_all(vcl_cout, img_);
#endif
    }
  };

  bool operator() ( int p, int i, int j, int k, const vcl_vector<TruePixelType>& pixel ) const
  {
    assert( p == 0 );
    return img_
      && pixel.size() == 1 &&
      pixel[0] == (TruePixelType)img_(i,j,k);
  }
 protected:
  vil3d_image_view< T > img_;
};

template<class T>
class CheckColourPlanes : public CheckPixel
{
 public:
  CheckColourPlanes( const char* file )
  {
    vil3d_image_resource_sptr ir = vil3d_load_image_resource(file );
    if ( !ir )
      vcl_cout << "[ couldn't read header from " << file << "]\n";
    else
    {
      vil3d_image_view_base_sptr im = ir->get_copy_view(0,ir->ni(),0,ir->nj(),0,ir->nk());
      if ( !im )
        vcl_cout << "[ couldn't read image data from " << file << "]\n";
      else
      {
        img_ = im;
#ifdef DEBUG
        vcl_cout << '\n' << vcl_flush; vil3d_print_all(vcl_cout, img_);
#endif
      }
    }
  }

  bool operator() ( int p, int i, int j, int k, const vcl_vector<TruePixelType>& pixel
) const
  {
    return img_ && pixel.size() == 1 && pixel[0] == img_(i,j,k,p);
  }
 protected:
  vil3d_image_view< T > img_;
};


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
  int depth;

  vcl_ifstream fin( true_data_file );
  if ( !( fin >> num_planes >> num_comp >> width >> height >> depth) ) {
    vcl_cout << "[couldn't read header from " << true_data_file << "]";
    return false;
  }

  vcl_vector<TruePixelType> pixel( num_comp );

  for ( int p=0; p < num_planes; ++p ) {
    for ( int k=0; k < depth; ++k ) {
      for ( int j=0; j < height; ++j ) {
        for ( int i=0; i < width; ++i ) {
          for ( int c=0; c < num_comp; ++c ) {
            if ( !( fin >> pixel[c] ) ) {
              vcl_cout << "[couldn't read value at " << p << "," << i << "," << j << "," << c
                << " from " << true_data_file << "]";
              return false;
            }
          }
          if ( !check( p, i, j, k, pixel ) )
          {
            vcl_cout << "[ image value at p=" << p <<
              ", k=" << k << ", j=" << j << ", i=" << i <<
              ", is wrong. Should be {" << pixel[0];
            for ( int c=1; c < num_comp; ++c )
              vcl_cout << "," << pixel[c];
            vcl_cout << "} ]";

            return false;
          }
        }
      }
    }
  }

  return true;
}

int test_file_format_read_main( int argc, char* argv[] )
{
  const unsigned ndir = 4098;
  char cwd[ndir];
  char *res = vpl_getcwd(cwd, ndir);
  if ( argc >= 2 )
    vpl_chdir(argv[1]);


  testlib_test_start(" file format read");

  vcl_cout << "List of slices)\n";
  testlib_test_begin( "  List of ppm slices" );
  testlib_test_perform( test( "ff_3planes8bit_true.txt", CheckColourPlanes<vxl_byte>( "ff_rgb8bit_ascii.1.ppm:ff_rgb8bit_ascii.2.ppm" ) ) );

  if (res==cwd) vpl_chdir(cwd);
  return testlib_test_summary();
}
