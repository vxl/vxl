// This is gel/vmal/vmal_convert_vtol.h
#ifndef vmal_convert_vtol_h_
#define vmal_convert_vtol_h_
//:
// \file
// \brief Toolbox to make conversion from vtol to vnl, vil1_image to vil1_memory_image
// \author L. Guichard
//------------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>

#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>

#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h>

void convert_lines_double_3(std::vector<vtol_edge_2d_sptr> in,
                            vnl_double_3 * &outp,
                            vnl_double_3 * &outq);

void convert_points_vect_double_3(std::vector<vtol_vertex_2d_sptr> & in,
                                  std::vector<vnl_double_3> & out);

void convert_points_double_3(std::vector<vtol_vertex_2d_sptr> in,
                             vnl_double_3 * &out);

//Convert a vtol_edge_2d to two vnl_double_3 representing its end-points.
void convert_line_double_3(const vtol_edge_2d_sptr& in,
                           vnl_double_3 &outp,
                           vnl_double_3 &outq);

void convert_line_double_2(const vtol_edge_2d_sptr& in,
                           vnl_double_2 &outp,
                           vnl_double_2 &outq);

void convert_point_double_3(const vtol_vertex_2d_sptr& in,
                            vnl_double_3 &out);

void convert_grey_memory_image(const vil1_image & image,
                               vil1_memory_image_of<vxl_byte> &ima_mono);

#endif // vmal_convert_vtol_h_
