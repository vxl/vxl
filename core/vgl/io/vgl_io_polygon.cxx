// This is core/vgl/io/vgl_io_polygon.cxx
#include <iostream>
#include "vgl_io_polygon.h"
//:
// \file

//====================================================================================
//: Binary save self to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, vgl_polygon<T> const& p)
{
    constexpr short io_version_no = 1;
    vsl_b_write(os, io_version_no);
    vsl_b_write(os, p.num_sheets());
    for (unsigned int i=0;i<p.num_sheets();i++)
    {
        vsl_b_write(os,p[i].size());
        for (unsigned int j=0;j<p[i].size();j++)
            vsl_b_write(os, p[i][j]);
    }
}

template void vsl_b_write(vsl_b_ostream&, vgl_polygon<float> const&);
template void vsl_b_write(vsl_b_ostream&, vgl_polygon<double> const&);

//====================================================================================
//: Binary load self from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_polygon<T> & p)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
    unsigned int num_sheets;
    vsl_b_read(is, num_sheets);
    p.clear();
    for (unsigned int i=0;i<num_sheets;i++)
    {
      p.new_sheet();
      typedef typename vgl_polygon<T>::sheet_t::size_type size_type;
      size_type npoints;
      vsl_b_read(is, npoints);
      vgl_point_2d<T> point;
      for (size_type j=0;j<npoints;j++)
      {
        vsl_b_read(is, point);
        p.push_back(point);
      }
    }
    break;

   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_polygon<T>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

template void vsl_b_read(vsl_b_istream&, vgl_polygon<float>&);
template void vsl_b_read(vsl_b_istream&, vgl_polygon<double>&);

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(std::ostream& os, vgl_polygon<T>const& p)
{
  os<<"Polygon with points defined by sheets :\n";
  for (unsigned int i=0;i<p.num_sheets();i++)
  {
      for (unsigned int j=0;j<p[i].size();j++)
          os << p[i][j].x()<<','<<p[i][j].y()<<std::endl;
      os<<std::endl;
  }
}

template void vsl_print_summary(std::ostream&, vgl_polygon<float>const&);
template void vsl_print_summary(std::ostream&, vgl_polygon<double>const&);
