#ifndef vmal_kl_params_h_
#define vmal_kl_params_h_
#ifdef __GNUC__
#pragma interface
#endif
//--------------------------------------------------------------------------------
// .NAME	vmal_kl_params
// .INCLUDE	vmal/vmal_kl_params.h
// .FILE	vmal_kl_params.cxx
// .SECTION Description:
//   Interface to set Kanade-Lucas parametres
// .SECTION Author
//   L. Guichard
// .SECTION Modifications:   
//--------------------------------------------------------------------------------
#include <kl/klt.h>

class vmal_kl_params {

public:

//---------------------------------------------------------------------------
//: Default constructor. Parametres set to defaults
//---------------------------------------------------------------------------
	vmal_kl_params();

//---------------------------------------------------------------------------
//: Copy constructor
//---------------------------------------------------------------------------
	vmal_kl_params(const vmal_kl_params & params);

//---------------------------------------------------------------------------
//: Destructor.
//---------------------------------------------------------------------------
	~vmal_kl_params();

//---------------------------------------------------------------------------
//: Set the parametres to default values
//---------------------------------------------------------------------------
	void set_defaults();

//---------------------------------------------------------------------------
//: Parametres
//---------------------------------------------------------------------------

	int numpoints;
	int search_range;

	int mindist;
	int window_width, window_height;
	bool sequentialMode;
	bool smoothBeforeSelecting;
	bool writeInternalImages;
	int min_eigenvalue;
	float min_determinant;
	float min_displacement;
	int max_iterations;
	float max_residue;
	float grad_sigma;
	float smooth_sigma_fact;
	float pyramid_sigma_fact;
	bool replaceLostPoints;
	int nSkippedPixels;
	int borderx;
	int bordery;
	int nPyramidLevels;
	int subsampling;
	bool verbose;


};

#endif // vmal_kl_params