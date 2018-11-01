// This is core/vil/vil_print.h
#ifndef vil_print_h_
#define vil_print_h_
//:
// \file
// \author Ian Scott, Tim Cootes.

#include <iomanip>
#include <iostream>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: How to print value in vil_print_all(image_view)
// \relatesalso vil_image_view
template<class T>
void vil_print_value(std::ostream& s, const T& value, unsigned=0);

// Specializations must be declared before use, so they need to be
// declared here.

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define vil_print_declare_specialization( T ) \
  template <> \
  void vil_print_value(std::ostream& os, const T & value, unsigned)

vil_print_declare_specialization( bool );
vil_print_declare_specialization( vxl_byte );
vil_print_declare_specialization( vxl_sbyte );
vil_print_declare_specialization( vxl_int_16 );
vil_print_declare_specialization( vxl_uint_16 );
vil_print_declare_specialization( vxl_int_32 );
vil_print_declare_specialization( vxl_uint_32 );
#if VXL_HAS_INT_64
vil_print_declare_specialization( vxl_int_64 );
vil_print_declare_specialization( vxl_uint_64 );
#endif
vil_print_declare_specialization( float );
vil_print_declare_specialization( double );
vil_print_declare_specialization( std::complex<float> );
vil_print_declare_specialization( std::complex<double> );
vil_print_declare_specialization( vil_rgb<vxl_byte> );
vil_print_declare_specialization( vil_rgb<vxl_sbyte> );
vil_print_declare_specialization( vil_rgb<vxl_int_16> );
vil_print_declare_specialization( vil_rgb<vxl_uint_16> );
vil_print_declare_specialization( vil_rgb<vxl_int_32> );
vil_print_declare_specialization( vil_rgb<vxl_uint_32> );
#if VXL_HAS_INT_64
vil_print_declare_specialization( vil_rgb<vxl_int_64> );
vil_print_declare_specialization( vil_rgb<vxl_uint_64> );
#endif
vil_print_declare_specialization( vil_rgb<float> );
vil_print_declare_specialization( vil_rgb<double> );
vil_print_declare_specialization( vil_rgba<vxl_byte> );
vil_print_declare_specialization( vil_rgba<vxl_sbyte> );
vil_print_declare_specialization( vil_rgba<vxl_int_16> );
vil_print_declare_specialization( vil_rgba<vxl_uint_16> );
vil_print_declare_specialization( vil_rgba<vxl_int_32> );
vil_print_declare_specialization( vil_rgba<vxl_uint_32> );
#if VXL_HAS_INT_64
vil_print_declare_specialization( vil_rgba<vxl_int_64> );
vil_print_declare_specialization( vil_rgba<vxl_uint_64> );
#endif
vil_print_declare_specialization( vil_rgba<float> );
vil_print_declare_specialization( vil_rgba<double> );

#undef vil_print_declare_specialization

#endif // DOXYGEN_SHOULD_SKIP_THIS


//: Print all image data to os in a grid (rounds output to int)
// \relatesalso vil_image_view
template<class T>
inline void vil_print_all(std::ostream& os,const vil_image_view<T>& view, unsigned width=0)
{
  if (!width) width = static_cast<unsigned>(os.width());
  os<<view.is_a()<<' '<<view.nplanes()<<" planes, each "<<view.ni()<<" x "<<view.nj()
    <<" istep: "<<(int)view.istep()<<' '
    <<" jstep: "<<(int)view.jstep()<<' '
    <<" planestep: "<<(int)view.planestep()<<'\n' << std::flush;
  for (unsigned int p=0;p<view.nplanes();++p)
  {
    if (view.nplanes()>1) os<<"Plane "<<p<<":\n" << std::flush;
    for (unsigned int j=0;j<view.nj();++j)
    {
      for (unsigned int i=0;i<view.ni();++i)
      {
        os<<' '<<std::setw(width);
        vil_print_value(os,view(i,j,p), width);
      }
      os<<'\n'<<std::flush;
    }
  }
}

//: Print all image data to os in a grid
// \relatesalso vil_image_view
void vil_print_all(std::ostream& os, vil_image_view_base_sptr const& view);

#endif // vil_print_h_
