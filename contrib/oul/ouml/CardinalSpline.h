#ifndef CARDINAL_SPLINE_D_
#define CARDINAL_SPLINE_D_

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vnl/io/vnl_io_vector_fixed.txx>
#include <vnl/io/vnl_io_matrix.txx>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>

// A 3D cardinal spline class. See Hearn and Baker, "Computer
// Graphics", C Version, Second Edition, page 325. Cardinal splines
// are cubic interpolating splines where the tangents are set equal to
// the chord passing through the points on either side of the current
// control point.
//
// This spline could possibly be templated to allow for plane or space
// curves. At the moment, if you want a space curve, just set the 3rd
// coordinate of all the control points to some constant value (or
// more generally, ensure all control points lie on a plane).
//
// Actually, at the moment, these are Catmull-Rom splines (t=0.5)
//
// \author Brendan McCane
// \status Under Development

class CardinalSpline
{
public:
    typedef vnl_vector_fixed<double, 3> Vector3D;
    CardinalSpline(): Mc(4,4,0.0), s(0.25){};
	CardinalSpline(vcl_vector<Vector3D> &cPoints): 
		controlPoints(cPoints), Mc(4,4,0.0), s(0.25)
	{
		setMc(s);
	};
    CardinalSpline(const CardinalSpline &cs): 
        controlPoints(cs.controlPoints), Mc(cs.Mc), s(cs.s) {};
    CardinalSpline &operator =(const CardinalSpline &cs){
        if (&cs != this)
        {
            controlPoints = cs.controlPoints;
            Mc = cs.Mc;
            s = cs.s;
        }
        return *this;
    };
	~CardinalSpline(){};

	Vector3D getPoint(double t) const;
	vcl_vector<Vector3D> getPoints(int num_points) const;
	vcl_vector<Vector3D> getControlPoints() const {return controlPoints;};
	void setT(double t){setMc((1-t)/2.0);};
    Vector3D closest_point(const Vector3D &point) const {
        double t = closest_point_t(point);
        return getPoint(t);
    };
	double closest_point_t(const Vector3D &point) const;
	Vector3D firstDerivative(double t) const;
	Vector3D secondDerivative(double t) const;
    // return the mean of the control pts
    Vector3D mean_control_pts() const {
        Vector3D mean(0.0);
        for (uint i=0; i<controlPoints.size(); i++)
            mean += controlPoints[i];
        mean /= (double)controlPoints.size();
        return mean;
    };

    // binary IO stuff
    void b_write(vsl_b_ostream &os) const;
    void b_read(vsl_b_istream &os);
    short version() const {return 1;};
    void print_summary(vcl_ostream &os) const {
        os << "Cardinal Spline\n";
    };
    vcl_string is_a() const {return vcl_string("CardinalSpline");};
    bool is_class(const vcl_string &s){return s==is_a();};

    bool operator==(const CardinalSpline &c){
        return (controlPoints==c.controlPoints) && (Mc==c.Mc) && (s==c.s);
    }; 

    bool operator!=(const CardinalSpline &c){
        return !(*this==c);
    };
        
private:
	double distanceFunctionFirstDerivative(double t, 
                                           const Vector3D &point) const;
	double distanceFunctionSecondDerivative(double t, 
                                            const Vector3D &point) const;
	Vector3D getVal(const vnl_matrix_fixed<double, 1, 4> &uvec, int pk) const;
	double convert_t(double t) const{
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

void vsl_b_write(vsl_b_ostream &os, const CardinalSpline &e);
void vsl_b_read(vsl_b_istream &is, CardinalSpline &e);
void vsl_print_summary(vcl_ostream &is, const CardinalSpline &e);

#endif // CARDINAL_SPLINE_D_
