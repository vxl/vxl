#ifndef __CARDINAL_SPLINE_D
#define __CARDINAL_SPLINE_D 1

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vcl_vector.h>

/*
 * A 3D cardinal spline class. See Hearn and Baker, "Computer
 * Graphics", C Version, Second Edition, page 325. Cardinal splines
 * are cubic interpolating splines where the tangents are set equal to
 * the chord passing through the points on either side of the current
 * control point.
 *
 * This spline could possibly be templated to allow for plane or space
 * curves. At the moment, if you want a space curve, just set the 3rd
 * coordinate of all the control points to some constant value (or
 * more generally, ensure all control points lie on a plane).
 *
 * Actually, at the moment, these are Catmull-Rom splines (t=0.5)
 *
 * @author Brendan McCane
 * @status Under Development
 */

class CardinalSpline
{
public:
    typedef vnl_vector_fixed<double, 3> Vector3D;
	CardinalSpline(vcl_vector<Vector3D> &cPoints): 
		controlPoints(cPoints), Mc(4,4,0.0), s(0.25)
	{
		setMc(s);
	};
	~CardinalSpline(){};
	Vector3D getPoint(double t);
	vcl_vector<Vector3D> getPoints(int num_points);
	void setT(double t){setMc((1-t)/2.0);};
	double closest_point_t(const Vector3D &point);
	Vector3D firstDerivative(double t);
	Vector3D secondDerivative(double t);
private:
	double distanceFunctionFirstDerivative(double t, const Vector3D &point);
	double distanceFunctionSecondDerivative(double t, const Vector3D &point);
	Vector3D getVal(const vnl_matrix_fixed<double, 1, 4> &uvec, int pk);
	double convert_t(double t){
		if (t<0.0) 
			while (t<0.0) t+=1.0;
		else if (t>1.0)
			while (t>1.0) t-=1.0;
		return t;
	};
		
	void setMc(double _s)
	{
		s = _s;
		Mc(0,0)=-s; Mc(0,1)=2-s; Mc(0,2)=s-2; Mc(0,3)=s;
		Mc(1,0)=2*s; Mc(1,1)=s-3; Mc(1,2)=3-2*s; Mc(1,3)=-s;
		Mc(2,0)=-s; Mc(2,1)=0; Mc(2,2)=s; Mc(2,3)=0;
		Mc(3,0)=0; Mc(3,1)=1; Mc(3,2)=0; Mc(3,3)=0;
	};
	vcl_vector<Vector3D> controlPoints;
	vnl_matrix<double> Mc;
	double s;
};

#endif
