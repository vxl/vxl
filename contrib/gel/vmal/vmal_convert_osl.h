// This is gel/vmal/vmal_convert_osl.h
#ifndef vmal_convert_osl_h_
#define vmal_convert_osl_h_
//:
// \file
// \brief Toolbox to make conversion from osl to vtol
// \author L. Guichard
//------------------------------------------------------------------------------

#include <vector>
#include <iostream>
#include <string>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>

#include <osl/osl_vertex.h>
#include <osl/osl_edge.h>

#include <mvl/HomgPoint2D.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vtol_vertex_2d_sptr convert_vertex_2d(osl_vertex & in);

// Convert an osl_edge to a vtol_edge_2d.
// If the osl_edge is a line then set type=LINE.
// If you don't know anything about the shape of the curve, set type=CURVE_NO_TYPE (default)
vtol_edge_2d_sptr convert_edge_2d(osl_edge & in,const std::string& type="CURVE_NO_TYPE");

std::vector<vtol_edge_2d_sptr>* convert_vector_edge_2d(std::list<osl_edge*> & in,
                                                      const std::string& type="CURVE_NO_TYPE");

std::vector<std::vector<vtol_edge_2d_sptr>*>* convert_array_edge_2d(std::list<std::list<osl_edge *>*> & in,
                                                                  const std::string& type);

void convert_pointarray(std::vector<vtol_vertex_2d_sptr>& in,std::vector<HomgPoint2D> & out);

#endif // vmal_convert_osl_h_
