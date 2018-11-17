//-----------------------------------------------------------------------------
// .DESCRIPTION:
//   See vmal_convert_osl.h
//-----------------------------------------------------------------------------

#include "vmal_convert_osl.h"
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_edge_2d.h>

vtol_vertex_2d_sptr convert_vertex_2d(osl_vertex & in)
{
  return new vtol_vertex_2d(in.GetX(),in.GetY());
}

vtol_edge_2d_sptr convert_edge_2d(osl_edge & in,const std::string& type)
{
  vtol_edge_2d_sptr out;

  if (type=="CURVE_NO_TYPE")
  {
    float *x=in.GetX();
    float *y=in.GetY();
    std::vector<vtol_vertex_sptr> new_vertices;

    for (unsigned int i=0; i<in.size(); ++i)
      new_vertices.push_back(new vtol_vertex_2d(x[i],y[i]));
    out=new vtol_edge_2d(new vtol_zero_chain(new_vertices));
  }
  else if (type=="LINE")
  {
    osl_vertex* v1=in.GetV1();
    osl_vertex* v2=in.GetV2();
    vtol_vertex_2d_sptr new_v1=convert_vertex_2d(*v1);
    vtol_vertex_2d_sptr new_v2=convert_vertex_2d(*v2);
    out=new vtol_edge_2d(new_v1,new_v2,nullptr);
  }

  return out;
}

std::vector<vtol_edge_2d_sptr>* convert_vector_edge_2d(std::list<osl_edge*> & in,const std::string& type)
{
  std::list<osl_edge*>::iterator iter;
  auto* out=new std::vector<vtol_edge_2d_sptr>();
  for (iter=in.begin();iter!=in.end();iter++)
  {
    vtol_edge_2d_sptr temp_edge_2d=convert_edge_2d(*(*iter),type);
    out->push_back(temp_edge_2d);
  }
  return out;
}

std::vector<std::vector<vtol_edge_2d_sptr>*>* convert_array_edge_2d(std::list<std::list<osl_edge *>*> & in,
                                                                  const std::string& type)
{
  std::list<std::list<osl_edge *>*>::iterator iter;
  auto* out=new std::vector<std::vector<vtol_edge_2d_sptr>*>;
  for (iter = in.begin();iter!=in.end();iter++)
  {
    std::vector<vtol_edge_2d_sptr>* vtol_lines=convert_vector_edge_2d(*(*iter),type);
    out->push_back(vtol_lines);
  }
  return out;
}

void convert_pointarray(std::vector<vtol_vertex_2d_sptr>& in,
                        std::vector<HomgPoint2D> & out)
{
  std::vector<vtol_vertex_2d_sptr>::iterator iter;
  for (iter=in.begin();iter!=in.end();iter++)
  {
    HomgPoint2D temp((*iter)->x(),(*iter)->y(),1);
    out.push_back(temp);
  }
}
