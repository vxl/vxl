// This is core/vil/vil_print.h
#ifndef vil_print_h_
#define vil_print_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott, Tim Cootes.

#include <vil/vil_image_view.h>
#include <vcl_iostream.h>

//: How to print value in vil_print_all(image_view)
// \relates vil_image_view
template<class T>
void vil_print_value(vcl_ostream& s, const T& value);

// Specializations must be declared before use, so they need to be
// declared here.

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define vil_print_declare_specialization( T ) \
  VCL_DEFINE_SPECIALIZATION \
  void vil_print_value(vcl_ostream& os, const T & value)

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
vil_print_declare_specialization( vcl_complex<float> );
vil_print_declare_specialization( vcl_complex<double> );
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
// \relates vil_image_view
template<class T>
inline void vil_print_all(vcl_ostream& os,const vil_image_view<T>& view)
{
  os<<view.is_a()<<" "<<view.nplanes()<<" planes, each "<<view.ni()<<" x "<<view.nj()
    <<" istep: "<<(int)view.istep()<<' '
    <<" jstep: "<<(int)view.jstep()<<' '
    <<" planestep: "<<(int)view.planestep()<<'\n' << vcl_flush;
  for (unsigned int p=0;p<view.nplanes();++p)
  {
    if (view.nplanes()>1) os<<"Plane "<<p<<":\n" << vcl_flush;
    for (unsigned int j=0;j<view.nj();++j)
    {
      for (unsigned int i=0;i<view.ni();++i)
      {
        os<<' ';
        vil_print_value(os,view(i,j,p));
      }
      os<<'\n'<<vcl_flush;
    }
  }
}

//: Print all image data to os in a grid
// \relates vil_image_view
void vil_print_all(vcl_ostream& os, vil_image_view_base_sptr const& view);

#endif // vil_print_h_
