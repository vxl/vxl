// This is contrib/gel/vifa/vifa_int_face_attr.h
#ifndef _VIFA_INT_FACE_ATTR_H_
#define _VIFA_INT_FACE_ATTR_H_

//-----------------------------------------------------------------------------
//:
// \file
// \brief Collects attribute information about an individual intensity face.
//
// \author Roddy Collins, from DDB in TargetJr
//
// \verbatim
//  Modifications:
//   MPP May 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vbl/vbl_smart_ptr.h>
#include <sdet/sdet_fit_lines_params.h>
#include <vdgl/vdgl_intensity_face.h>
#include <vdgl/vdgl_intensity_face_sptr.h>
#include <vifa/vifa_histogram.h>
#include <vifa/vifa_int_face_attr_common.h>
#include <vifa/vifa_parallel.h>
#include <vifa/vifa_typedefs.h>


class vifa_int_face_attr: public vifa_int_face_attr_common
{
protected:

	//: The face whose attributes we store
	vdgl_intensity_face_sptr	_face;

	float						_cached_min;
	float						_cached_max;
	float						_cached_mean;
	float						_cached_var;
	float						_cached_2_parallel;
	float						_cached_4_parallel;
	int							_cached_80_parallel;
	vifa_parallel*				_npobj;

public:

	vifa_int_face_attr(sdet_fit_lines_params*	fitter_params = NULL,
					   vifa_group_pgram_params*	gpp = NULL,
					   vifa_group_pgram_params*	gpp_w = NULL,
					   vifa_norm_params*		np = NULL
					  );
	vifa_int_face_attr(vdgl_intensity_face_sptr	f,
					   sdet_fit_lines_params*	fitter_params = NULL,
					   vifa_group_pgram_params*	gpp = NULL,
					   vifa_group_pgram_params*	gpp_w = NULL,
					   vifa_norm_params*		np = NULL
					  );
	virtual ~vifa_int_face_attr();

	// ---
	// Public functional methods
	// ---

	bool		ComputeAttributes();
	bool		GetAttributes(vcl_vector<float>&	attrs);
	static void	GetAttributeNames(vcl_vector<string>&	names);
	bool		GetNativeAttributes(vcl_vector<float>&	attrs);

	// Data access for non-attributes
	vdgl_intensity_face_sptr	GetFace() const
		{ return _face; }
	void						SetFace(vdgl_intensity_face_sptr	f);
	edge_2d_list&				GetEdges();

	//: Centroid X
	float						Xo()
		{ return _face->Xo(); }

	//: Centroid Y
	float						Yo()
		{ return _face->Yo(); }

	//: Centroid Z
	float						Zo()
		{ return _face->Zo(); }

	// ---
	// Intensity attribute computations
	// ---

	//: Min intensity
	float	IntMin()
		{ return _cached_min; }

	//: Max intensity
	float	IntMax()
		{ return _cached_max; }

	//: Mean intensity
	float	IntMean()
		{ return _cached_mean; };

	//: Intensity variance
	float	IntVar()
		{ return _cached_var; }

	// ---
	// Geometric attribute computations
	// ---

	//: Area
	float	Area()
		{ return GetFace() ? _face->Npix() :  -1; }

	//: Ratio of major moments
	float	AspectRatio();

	//: Length of boundary, in pixels
	float	PerimeterLength();

	float	WeightedPerimeterLength();
	float	Complexity();

	//: Edge length^2 / detection area
	float	WeightedComplexity();

	float	TwoPeakParallel();
	float	FourPeakParallel();
	float	EightyPercentParallel();

protected:

	void	ComputeCacheValues();
	void	SetNP();
};

typedef vbl_smart_ptr<vifa_int_face_attr>	vifa_int_face_attr_sptr;

typedef vcl_vector<vifa_int_face_attr_sptr>				attr_list;
typedef vcl_vector<vifa_int_face_attr_sptr>::iterator	attr_iterator;


#endif	// VIFA_INT_FACE_ATTR_H
