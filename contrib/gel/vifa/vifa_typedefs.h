// This is gel/vifa/vifa_typedefs.h
#ifndef VIFA_TYPEDEFS_H
#define VIFA_TYPEDEFS_H

#include <vcl_vector.h>
#include <vbl/vbl_smart_ptr.h>
#include <vdgl/vdgl_fit_lines_params.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_topology_object.h>
#include <vifa/vifa_imp_line.h>


// Intensity faces
typedef vcl_vector<vtol_intensity_face_sptr>  iface_list;
typedef iface_list::iterator          iface_iterator;

// Edges
typedef vcl_vector<vtol_edge_2d_sptr> edge_2d_list;
typedef edge_2d_list::iterator        edge_2d_iterator;

// Edge list iterator
typedef edge_list::iterator           edge_iterator;

// Face list iterator
typedef face_list::iterator           face_iterator;

// Implicit lines
typedef vifa_imp_line<double>         imp_line;
typedef vbl_smart_ptr<imp_line>       imp_line_sptr;
typedef vcl_vector<imp_line_sptr>     imp_line_list;
typedef imp_line_list::iterator       imp_line_iterator;

typedef vcl_vector<imp_line_list*>    imp_line_table;
typedef imp_line_table::iterator      imp_line_table_iterator;

// Line fitting parameters
typedef vbl_smart_ptr<vdgl_fit_lines_params>  vdgl_fit_lines_params_sptr;


#endif  // VIFA_TYPEDEFS_H
