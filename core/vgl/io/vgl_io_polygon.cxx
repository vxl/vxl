// This is vxl/vgl/io/vgl_io_polygon.cxx

#include <vcl_cstdlib.h> // vcl_abort()
#include <vgl/io/vgl_io_polygon.h>

//====================================================================================
//: Binary save self to stream.
void vsl_b_write(vsl_b_ostream &os, const vgl_polygon & p)
{
    const short io_version_no = 1;
    vsl_b_write(os, io_version_no);
    vsl_b_write(os, p.num_sheets());
    for (int i=0;i<p.num_sheets();i++)
    {
        vsl_b_write(os,p[i].size());
        for (int j=0;j<p[i].size();j++)
            vsl_b_write(os, p[i][j]);
    }
}

//====================================================================================
//: Binary load self from stream.
void vsl_b_read(vsl_b_istream &is, vgl_polygon & p)
{
  short v;
  vsl_b_read(is, v);
  switch(v)
  {
  case 1:
    int num_sheets;
    vsl_b_read(is, num_sheets);
    p.clear();
    for (int i=0;i<num_sheets;i++)
    {
        p.new_sheet();
        int npoints;
        vsl_b_read(is, npoints);
        vgl_point_2d<float> point;
        for (int j=0;j<npoints;j++)
        {
            vsl_b_read(is, point);
            p.push_back(point);
        }
    }
    break;

  default:
    vcl_cerr << "ERROR: vsl_b_read(s, vgl_polygon&): Unknown version number "<< v << vcl_endl;
    vcl_abort();
  }
}

//====================================================================================
//: Output a human readable summary to the stream
void vsl_print_summary(vcl_ostream& os,const vgl_polygon & p)
{
  os<<"Polygon with points defined by sheets : "<<vcl_endl;
  for (int i=0;i<p.num_sheets();i++)
  {
      for (int j=0;j<p[i].size();j++)
          os << p[i][j].x()<<","<<p[i][j].y()<<vcl_endl;
      os<<vcl_endl;
  }
}

