// \file 
// \brief collection class for various reconstruction functions
// \author Kongbin Kang (Kongbin_Kang@Brown.edu)
// \date   4/24/2003
// \verbatim
// Modifications
// \endverbatim

#if !defined(AFX_BRCT_ALGOS_H__CBA35400_4B3C_4ABE_A0CA_4D1803BF38BC__INCLUDED_)
#define AFX_BRCT_ALGOS_H__CBA35400_4B3C_4ABE_A0CA_4D1803BF38BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_double_3x4.h>
#include <vsol/vsol_box_3d_sptr.h>

class brct_algos  
{
public:
	brct_algos();
	virtual ~brct_algos();

  //operators
public:
	static  add_box_vrml(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax);
	static vsol_box_3d_sptr get_bounding_box(vcl_vector<vgl_point_3d<double> > &pts_3d);
	static vnl_vector_fixed<double, 3> bundle_reconstruct_3d_point(vcl_vector<vnl_vector_fixed<double, 2> > &pts, vcl_vector<vnl_double_3x4>  &P);
  static vgl_point_2d<double> projection_3d_point(const vgl_point_3d<double> & x, const vnl_double_3x4& P);
  //: pointwised reconstruction
  static vgl_point_3d<double> triangulate_3d_point(const vgl_point_2d<double>& x1, const vnl_double_3x4& P1, \
                                                   const vgl_point_2d<double>& x2, const vnl_double_3x4& P2);
};

#endif // !defined(AFX_BRCT_ALGOS_H__CBA35400_4B3C_4ABE_A0CA_4D1803BF38BC__INCLUDED_)
