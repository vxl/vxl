// This is mul/vil3d/tests/test_file_format_read.cxx
#include <iostream>
#include <fstream>
#include <vector>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <cassert>

#include "vxl_config.h" // for vxl_uint_16 etc.

#include "testlib/testlib_test.h"
#include "vpl/vpl.h"
#include <vil/vil_config.h>

#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_print.h>

// \author Ian Scott
// \date Mar 2003

// Compare the results of loading different files with the true data
// that's supposed to be in those files. Only deals with single plane
// images so far.

typedef vxl_uint_32 TruePixelType;

class CheckPixel
{
 public:
  virtual ~CheckPixel() = default;
  virtual bool operator() ( unsigned int p, unsigned int i, unsigned int j, unsigned int k,
                            const std::vector<TruePixelType>& pixel ) const = 0;
};


template<class T>
class CheckPixelT : public CheckPixel
{
 public:
  CheckPixelT( const char* file )
  {
    vil3d_image_resource_sptr ir = vil3d_load_image_resource(file );
    if ( !ir )
    {
      std::cout << "[ couldn't read header from " << file << "]\n";
      return;
    }
    unsigned ni = ir->ni();
    unsigned nj = ir->nj();
    unsigned nk = ir->nk();
    vil3d_image_view_base_sptr im = ir->get_copy_view(0, ni, 0, nj, 0, nk);
    if ( !im )
    {
      std::cout << "[ couldn't read full image data from " << file << "]\n";
      return;
    }
    img_ = im;
#ifdef DEBUG
    std::cout << '\n' << std::flush; vil3d_print_all(std::cout, img_);
#endif
    // Now read just part of the image.
    im = ir->get_copy_view(ni/2, ni-ni/2, nj/2, nj-nj/2, nk/2, nk-nk/2);
    if ( !im )
    {
      std::cout << "[ couldn't read far octant image data from " << file << "]\n";
      return;
    }
    far_oct_img_ = im;
#ifdef DEBUG
    std::cout << '\n' << std::flush; vil3d_print_all(std::cout, far_oct_img_);
#endif
  }
 protected:
  vil3d_image_view< T > img_;
  vil3d_image_view< T > far_oct_img_;
};


template<class T>
class CheckGrey : public CheckPixelT<T>
{
 public:
  CheckGrey( const char* file ): CheckPixelT<T>(file) {}

  bool operator()(unsigned int p, unsigned int i, unsigned int j,
                  unsigned int k,
                  const std::vector<TruePixelType> &pixel) const override {
    assert( p == 0 );
    return this->img_ && pixel.size() == 1 &&
      i < this->img_.ni() && j < this->img_.nj() && k < this->img_.nk() &&
      pixel[0] == (TruePixelType)this->img_(i,j,k) &&
      ( !(i > this->img_.ni()/2 && j > this->img_.nj()/2 && k > this->img_.nk()/2)
        || pixel[0] == (TruePixelType)this->far_oct_img_(i-this->img_.ni()/2,
        j-this->img_.nj()/2, k-this->img_.nk()/2) );
  }
};

template<class T>
class CheckColourPlanes : public CheckPixelT<T>
{
 public:
  CheckColourPlanes( const char* file ): CheckPixelT<T>(file) {}

  bool operator() ( unsigned int p, unsigned int i, unsigned int j, unsigned int k,
                    const std::vector<TruePixelType>& pixel) const override
  {
    return this->img_ && pixel.size() == 1 && pixel[0] == this->img_(i,j,k,p) &&
      ( !(i > this->img_.ni()/2 && j > this->img_.nj()/2 && k > this->img_.nk()/2)
        || pixel[0] == (TruePixelType)this->far_oct_img_(i-this->img_.ni()/2,
        j-this->img_.nj()/2, k-this->img_.nk()/2) );
  }
};


bool test( const char* true_data_file, const CheckPixel& check )
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
  int depth;

  std::ifstream fin( true_data_file );
  if ( !( fin >> num_planes >> num_comp >> width >> height >> depth) ) {
    std::cout << "[couldn't read header from " << true_data_file << ']';
    return false;
  }

  std::vector<TruePixelType> pixel( num_comp );

  for ( int p=0; p < num_planes; ++p ) {
    for ( int k=0; k < depth; ++k ) {
      for ( int j=0; j < height; ++j ) {
        for ( int i=0; i < width; ++i ) {
          for ( int c=0; c < num_comp; ++c ) {
            if ( !( fin >> pixel[c] ) ) {
              std::cout << "[couldn't read value at " << p << ',' << i << ',' << j << ',' << c
                       << " from " << true_data_file << ']';
              return false;
            }
          }
          if ( !check( p, i, j, k, pixel ) )
          {
            std::cout << "[ image value at p=" << p <<
              ", k=" << k << ", j=" << j << ", i=" << i <<
              ", is wrong. Should be {" << pixel[0];
            for ( int c=1; c < num_comp; ++c )
              std::cout << ',' << pixel[c];
            std::cout << "} ]";

            return false;
          }
        }
      }
    }
  }

  return true;
}

void test_file_format_read( int argc, char* argv[] )
{
  constexpr unsigned ndir = 4098;
  char cwd[ndir];
  char *res = vpl_getcwd(cwd, ndir);
  if ( argc >= 2 )
    vpl_chdir(argv[1]);

  std::cout << "List of slices)\n";
  TEST("List of ppm slices", test("ff_3planes8bit_true.txt",
                                  CheckColourPlanes<vxl_byte>( "ff_rgb8bit_ascii.1.ppm;ff_rgb8bit_ascii.2.ppm" ) ), true);

  TEST("Implied List of ppm slices", test("ff_3planes8bit_true.txt",
                                          CheckColourPlanes<vxl_byte>( "ff_rgb8bit_ascii.#.ppm" ) ), true);

#if HAS_DCMTK
  TEST("Implied list of dicom slices", test("ff_grey16bit_true.txt",
                                            CheckGrey<vxl_uint_16>( "ff_grey16bit_uncompressed_####.dcm" ) ), true);
#endif // HAS_DCMTK

  std::cout << "GIPL images)\n";
  TEST("GIPL image", test("ff_grey_cross16bit_true.txt",
                          CheckGrey<vxl_uint_16>( "ff_grey_cross.gipl" ) ), true);

  if (res==cwd) vpl_chdir(cwd);
}

TESTMAIN_ARGS(test_file_format_read);
