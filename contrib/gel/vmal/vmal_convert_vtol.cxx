//-----------------------------------------------------------------------------
// .DESCRIPTION:
//   See vmal_convert_vtol.h
//-----------------------------------------------------------------------------

#include "vmal_convert_vtol.h"
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_image_as.h>

void convert_lines_double_3(vcl_vector<vtol_edge_2d_sptr> in,
                            vnl_double_3 * &outp,
                            vnl_double_3 * &outq)
{
  int numlines=in.size();
  outp=new vnl_double_3[numlines];
  outq=new vnl_double_3[numlines];

  for (int i=0;i<numlines;i++)
  {
    outp[i][0]=in[i]->v1()->cast_to_vertex_2d()->x();
    outp[i][1]=in[i]->v1()->cast_to_vertex_2d()->y();
    outp[i][2]=1;

    outq[i][0]=in[i]->v2()->cast_to_vertex_2d()->x();
    outq[i][1]=in[i]->v2()->cast_to_vertex_2d()->y();
    outq[i][2]=1;
  }
}

void convert_points_double_3(vcl_vector<vtol_vertex_2d_sptr> in,
                             vnl_double_3 * &out)
{
  int numpoints=in.size();
  out=new vnl_double_3[numpoints];

  for (int i=0;i<numpoints;i++)
  {
    out[i][0]=in[i]->x();
    out[i][1]=in[i]->y();
    out[i][2]=1;
  }
}

void convert_points_vect_double_3(vcl_vector<vtol_vertex_2d_sptr> & in,
                                  vcl_vector<vnl_double_3> & out)
{
  int numpoints=in.size();
  for (int i=0;i<numpoints;i++)
  {
    vnl_double_3 pt;
    pt[0]=in[i]->x();
    pt[1]=in[i]->y();
    pt[2]=1;
    out.push_back(pt);
  }
}

void convert_line_double_3(vtol_edge_2d_sptr in,
                           vnl_double_3 &outp,
                           vnl_double_3 &outq)
{
  outp[0]=in->v1()->cast_to_vertex_2d()->x();
  outp[1]=in->v1()->cast_to_vertex_2d()->y();
  outp[2]=1;

  outq[0]=in->v2()->cast_to_vertex_2d()->x();
  outq[1]=in->v2()->cast_to_vertex_2d()->y();
  outq[2]=1;
}

void convert_line_double_2(vtol_edge_2d_sptr in,
                           vnl_double_2 &outp,
                           vnl_double_2 &outq)
{
  outp[0]=in->v1()->cast_to_vertex_2d()->x();
  outp[1]=in->v1()->cast_to_vertex_2d()->y();

  outq[0]=in->v2()->cast_to_vertex_2d()->x();
  outq[1]=in->v2()->cast_to_vertex_2d()->y();
}

void convert_point_double_3(vtol_vertex_2d_sptr in,
                            vnl_double_3 &out)
{
  out[0]=in->x();
  out[1]=in->y();
  out[2]=1;
}

void convert_grey_memory_image(const vil1_image & image,
                               vil1_memory_image_of<vxl_byte> &ima_mono)
{
  int w=image.width();
  int h=image.height();

  ima_mono.resize(w,h);

  vil1_image_as_byte(image).get_section(ima_mono.get_buffer(), 0, 0, w, h);
}
