// This is gel/vmal/vmal_convert_osl.h
#ifndef vmal_convert_osl_h_
#define vmal_convert_osl_h_
//:
// \file
// \brief Toolbox to make conversion from osl to vtol
// \author L. Guichard
//------------------------------------------------------------------------------

#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>

#include <osl/osl_vertex.h>
#include <osl/osl_edge.h>

#include <vsol/vsol_curve_2d.h>

#include <mvl/HomgPoint2D.h>

#include <vcl_vector.h>

vtol_vertex_2d_sptr convert_vertex_2d(osl_vertex & in);

//Convert a osl_edge to a vtol_edge_2d.
//If the osl_edge is a line then set type=LINE.
//If you don't know anything about the shape of the curve, set type=CURVE_NO_TYPE (default)
vtol_edge_2d_sptr convert_edge_2d(osl_edge & in,vcl_string type="CURVE_NO_TYPE");

vcl_vector<vtol_edge_2d_sptr>* convert_vector_edge_2d(vcl_list<osl_edge*> & in,
                                                      vcl_string type="CURVE_NO_TYPE");

vcl_vector<vcl_vector<vtol_edge_2d_sptr>*>* convert_array_edge_2d(vcl_list<vcl_list<osl_edge *>*> & in,
                                                                  vcl_string type);

void convert_pointarray(vcl_vector<vtol_vertex_2d_sptr>& in,vcl_vector<HomgPoint2D> & out);

#endif // vmal_convert_osl_h_
