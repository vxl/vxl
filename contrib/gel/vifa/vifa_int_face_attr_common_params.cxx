// This is contrib/gel/vifa/vifa_int_face_attr_common_params.cxx
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vifa/vifa_int_face_attr_common_params.h>


//: Default constructor
vifa_int_face_attr_common_params::
vifa_int_face_attr_common_params(sdet_fit_lines_params*		fitter_params, 
								 vifa_group_pgram_params*	gpp_s,
								 vifa_group_pgram_params*	gpp_w,
								 vifa_coll_lines_params*	cpp,
								 vifa_norm_params*			np
								)
{
	init_params(fitter_params,
				gpp_s,
				gpp_w,
				cpp,
				np);
}

vifa_int_face_attr_common_params::
vifa_int_face_attr_common_params(const vifa_int_face_attr_common_params&	ifap)
{
	init_params(ifap._fitter_params.ptr(),
				ifap._gpp_s.ptr(),
				ifap._gpp_w.ptr(),
				ifap._cpp.ptr(),
				ifap._np.ptr());
}

vifa_int_face_attr_common_params::
~vifa_int_face_attr_common_params()
{
	// Smart-pointer parameter blocks self-destruct upon destruction
}

//: Publicly available setting of parameters
void vifa_int_face_attr_common_params::
set_params(const vifa_int_face_attr_common_params&	ifap)
{
	init_params(ifap._fitter_params.ptr(),
				ifap._gpp_s.ptr(),
				ifap._gpp_w.ptr(),
				ifap._cpp.ptr(),
				ifap._np.ptr());
}

//: Checks that parameters are within acceptable bounds
bool vifa_int_face_attr_common_params::SanityCheck()
{
	vcl_stringstream	msg;
	bool				valid = true;

	if (!_fitter_params.ptr() ||
		!_gpp_s.ptr() ||
		!_gpp_w.ptr() ||
		!_cpp.ptr())
	{
		msg << "ERROR: {FitLines, GroupParallelogram, CollinearLines} params not set."
			<< ends;
		valid = false;
	}
	else
	{
		// Result is result of contained parameters' SanityCheck()'s
		valid = _fitter_params->SanityCheck() &&
				_gpp_s->SanityCheck() &&
				_gpp_w->SanityCheck() &&
				_cpp->SanityCheck();
	}

	SetErrorMsg(msg.str().c_str());
	return valid;
}

void vifa_int_face_attr_common_params::
print_info(void)
{
	vcl_cout << "vifa_int_face_attr_common_params:" << vcl_endl;
	vcl_cout << "  line fitting params: ";
	if (_fitter_params.ptr())
	{
		vcl_cout << *_fitter_params;
	}
	else
	{
		vcl_cout << "NULL" << vcl_endl;
	}
	vcl_cout << "  strong group parallelogram params: ";
	if (_gpp_s.ptr())
	{
		_gpp_s->print_info();
	}
	else
	{
		vcl_cout << "NULL" << vcl_endl;
	}
	vcl_cout << "  weak group parallelogram params: ";
	if (_gpp_w.ptr())
	{
		_gpp_w->print_info();
	}
	else
	{
		vcl_cout << "NULL" << vcl_endl;
	}
	vcl_cout << "  collinear lines params: ";
	if (_cpp.ptr())
	{
		_cpp->print_info();
	}
	else
	{
		vcl_cout << "NULL" << vcl_endl;
	}
	vcl_cout << "  normalization params: ";
	if (_np.ptr())
	{
		_np->print_info();
	}
	else
	{
		vcl_cout << "NULL" << vcl_endl;
	}
}

//: Assign internal parameter blocks
void vifa_int_face_attr_common_params::
init_params(sdet_fit_lines_params*		fitter_params,
			vifa_group_pgram_params*	gpp_s,
			vifa_group_pgram_params*	gpp_w,
			vifa_coll_lines_params*		cpp,
			vifa_norm_params*			np
		   )
{
	if (fitter_params)
	{
		_fitter_params = new sdet_fit_lines_params(*fitter_params);
	}
	else
	{
		const int	min_fit_length = 6;
		_fitter_params = new sdet_fit_lines_params(min_fit_length);
	}
	if (gpp_s)
	{
		_gpp_s = new vifa_group_pgram_params(*gpp_s);
	}
	else
	{
		const float	angle_increment = 5.0;
		_gpp_s = new vifa_group_pgram_params(angle_increment);
	}
	if (gpp_w)
	{
		_gpp_w = new vifa_group_pgram_params(*gpp_w);
	}
	else
	{
		const float	angle_increment = 20.0;
		_gpp_w = new vifa_group_pgram_params(angle_increment);
	}
	if (cpp)
	{
		_cpp = new vifa_coll_lines_params(*cpp);
	}
	else
	{
		_cpp = new vifa_coll_lines_params();
	}
	if (np)
	{
		_np = new vifa_norm_params(*np);
	}
	else
	{
		_np = new vifa_norm_params();
	}
}


//: Assign internal parameter blocks
void vifa_int_face_attr_common_params::
init_params(const sdet_fit_lines_params&	fitter_params,
			const vifa_group_pgram_params&	gpp_s,
			const vifa_group_pgram_params&	gpp_w,
			const vifa_coll_lines_params&	cpp,
			const vifa_norm_params&			np
		   )
{
	_fitter_params = new sdet_fit_lines_params(fitter_params);
	_gpp_s = new vifa_group_pgram_params(gpp_s);
	_gpp_w = new vifa_group_pgram_params(gpp_w);
	_cpp = new vifa_coll_lines_params(cpp);
	_np = new vifa_norm_params(np);
}

