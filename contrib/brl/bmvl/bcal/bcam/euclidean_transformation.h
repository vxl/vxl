// euclidean_transformation.h: interface for the euclidean_transformation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EUCLIDEAN_TRANSFORMATION_H__F0345CDE_C4EB_47BA_9A60_25A77056C5EB__INCLUDED_)
#define AFX_EUCLIDEAN_TRANSFORMATION_H__F0345CDE_C4EB_47BA_9A60_25A77056C5EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vcl_vector.h>
#include <vcsl/vcsl_spatial_transformation.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

class euclidean_transformation : public vcsl_spatial_transformation  
{
private:
  vcl_vector<vgl_h_matrix_3d<double>*> trans_;

public: // constructor and deconstructor
	euclidean_transformation();
	virtual ~euclidean_transformation();

public: // operators
	vnl_vector<double> inverse(const vnl_vector<double> &v, double time) const;
	virtual vnl_vector<double> execute(const vnl_vector<double> &v, double tims) const;
	virtual bool is_invertible(double time) const; // for abstract interface
  virtual void set_beat(vcl_vector<double> const& new_beat);

protected:
  	int remove();
};

#endif // !defined(AFX_EUCLIDEAN_TRANSFORMATION_H__F0345CDE_C4EB_47BA_9A60_25A77056C5EB__INCLUDED_)
