#ifndef vdgl_fit_line_h
#define vdgl_fit_line_h
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vdgl_fitline - Fits  a line to a set of points defined in vdgl_edgel_chain
// .INCLUDE vgl/vdgl_fit_line.h
// .FILE vdgl_fitline.txx
//
// .SECTION Description
//  Fits a line to 2D edgel chain
//
// .SECTION Author
//   
// Created: xxx xx xxxx

#include <vcl/vcl_list.h>
#include <vdgl/vdgl_edgel_chain.h>
#include  <vsol/vsol_line_2d_ref.h>
#include <vdgl/vdgl_ortho_regress.h>
#include  <vdgl/vdgl_OrthogRegress.h>


vsol_line_2d_ref vdgl_fit_line(vdgl_edgel_chain &chain);



#endif // vdgl_fit_lines_h
