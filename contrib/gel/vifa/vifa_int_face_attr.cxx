// This is contrib/gel/vifa/vifa_int_face_attr.cxx
#include <vdgl/vdgl_intensity_face.h>
#include <vifa/vifa_int_face_attr.h>


vifa_int_face_attr::
vifa_int_face_attr(sdet_fit_lines_params*	fitter_params,
				   vifa_group_pgram_params*	gpp,
				   vifa_group_pgram_params*	gpp_w,
				   vifa_norm_params*		np) :
	vifa_int_face_attr_common(fitter_params, gpp, gpp_w, 0, np),
	_face(NULL),
	_cached_min(0.0),
	_cached_max(0.0),
	_cached_mean(0.0),
	_cached_var(0.0),
	_npobj(0)
{
}

vifa_int_face_attr::
vifa_int_face_attr(vdgl_intensity_face_sptr f,
				   sdet_fit_lines_params*	fitter_params,
				   vifa_group_pgram_params*	gpp,
				   vifa_group_pgram_params*	gpp_w,
				   vifa_norm_params*		np) :
	vifa_int_face_attr_common(fitter_params, gpp, gpp_w, 0, np),
	_face(f),
	_cached_min(0.0),
	_cached_max(0.0),
	_cached_mean(0.0),
	_cached_var(0.0),
	_cached_2_parallel(-1),
	_cached_4_parallel(-1),
	_cached_80_parallel(-1),
	_npobj(0)
{
	_attributes_valid = this->ComputeAttributes();
}

vifa_int_face_attr::
~vifa_int_face_attr()
{
	delete _npobj;
}


// ------------------------------------------------------------
// Data access & computation for non-attributes
//

void vifa_int_face_attr::
SetFace(vdgl_intensity_face_sptr	f)
{
	_face = f;
	delete _npobj;
	_npobj = 0;
	_attributes_valid = this->ComputeAttributes();
}

edge_2d_list& vifa_int_face_attr::
GetEdges()
{
	// Don't compute again if already there
	if (!_edges.empty())
	{
		return _edges;
	}

	if (!_face)
	{
		vcl_cerr << "vifa_int_face_attr::GetFaces(): _face is not set" <<
			vcl_endl;
		return _edges;
	}

	edge_list*	fedges = _face->edges();
	if (fedges)
	{
		for (edge_iterator eli = fedges->begin(); eli != fedges->end(); eli++)
		{
			vtol_edge_2d_sptr	e = (*eli)->cast_to_edge_2d();

			if (e)
			{
				_edges.push_back(e);
			}
		}

		delete fedges;
	}

	return _edges;
}


// ------------------------------------------------------------
// Public functional methods
//

bool vifa_int_face_attr::
ComputeAttributes()
{
	ComputeCacheValues();
	_attributes_valid = true;
	return this->valid_p();
}

// Populate a vector containing all attributes, including inherited
// ones.
bool vifa_int_face_attr::
GetAttributes(vcl_vector<float>&	attrs)
{
	// no inherited attributes
	return this->vifa_int_face_attr::GetNativeAttributes(attrs);
}

// Append the names of the attributes in the same order as in
// GetNativeAttributes.  KEEP THEM IN SYNC!
void vifa_int_face_attr::
GetAttributeNames(vcl_vector<vcl_string>&	names)
{
	names.push_back("IntMax");
	names.push_back("IntMin");
	names.push_back("IntMean");
	names.push_back("IntVar");
	names.push_back("Area");
	names.push_back("AspectRatio");
	names.push_back("PerimeterLength");
	names.push_back("WeightedPerimeterLength");
	names.push_back("Complexity");
	names.push_back("WeightedComplexity");
	names.push_back("StrongParallel");
	names.push_back("WeakParallel");
	names.push_back("TwoPeakParallel");
	names.push_back("FourPeakParallel");
	names.push_back("EightyPercentParallel");
}

// Populate a vector containing attributes native to this class (not
// inherited).  SYNC ADDITIONS TO THIS WITH THE NAMES FUNCTION ABOVE!
bool vifa_int_face_attr::
GetNativeAttributes(vcl_vector<float>& attrs)
{
	if (!this->ComputeAttributes())
	{
		vcl_cerr << "Couldn't compute single face attributes?" << vcl_endl;
		return false;
	}

	attrs.push_back(this->IntMax());
	attrs.push_back(this->IntMin());
	attrs.push_back(this->IntMean());
	attrs.push_back(this->IntVar());
	attrs.push_back(this->Area());
	attrs.push_back(this->AspectRatio());
	attrs.push_back(this->PerimeterLength());
	attrs.push_back(this->WeightedPerimeterLength());
	attrs.push_back(this->Complexity());
	attrs.push_back(this->WeightedComplexity());
	attrs.push_back(this->StrongParallelSal());
	attrs.push_back(this->WeakParallelSal());
	attrs.push_back(this->TwoPeakParallel());
	attrs.push_back(this->FourPeakParallel());
	attrs.push_back(this->EightyPercentParallel());
	return true;
}

// ------------------------------------------------------------
// Individual attribute computation
//

float vifa_int_face_attr::
AspectRatio()
{
	if (_aspect_ratio < 0)
	{
		_aspect_ratio = _face->AspectRatio();
	}

	return _aspect_ratio;
}

float vifa_int_face_attr::
PerimeterLength()
{
	if (_peri_length < 0)
	{
		_peri_length = _face->perimeter();
	}

	return _peri_length;
}

float vifa_int_face_attr::
WeightedPerimeterLength()
{
	if (_weighted_peri_length < 0)
	{
		// Block-copied from intensity face rather than undergo the pain
		// of adding a "weighted" boolean parameter
		edge_list*	edges = _face->edges();
		float		p = 0.0;
		float		intensity_sum = 1.0;

		if (edges)
		{
			for (edge_iterator eit = edges->begin(); eit != edges->end(); eit++)
			{
				vtol_edge_2d_sptr	e = (*eit)->cast_to_edge_2d();

				if (e)
				{
					// Leave at default of 1.0 if no adjacent face
					float	int_grad =
						get_contrast_across_edge(e->cast_to_edge(), 1.0);

					p += e->curve()->length() * int_grad;
					intensity_sum += int_grad;
				}
			}

			// Clean up returned edge list
			delete edges;
		}

		_weighted_peri_length = p / intensity_sum;
	}

	return _weighted_peri_length;
}

float vifa_int_face_attr::
Complexity()
{
	float	area = this->Area();
	float	len = this->PerimeterLength();

	if ((_complexity < 0) && (len >= 0) && (area >= 0))
	{
		_complexity = len * len / area;
	}

	return _complexity;
}

float vifa_int_face_attr::
WeightedComplexity()
{
	float	area = this->Area();
	float	len = this->WeightedPerimeterLength();

	if ((_weighted_complexity < 0) && (len >= 0) && (area >= 0))
	{
		_weighted_complexity = len * len / area;
	}

	return _weighted_complexity;
}

float vifa_int_face_attr::
TwoPeakParallel()
{
	if (_cached_2_parallel < 0)
	{
		SetNP();

		float	max_angle;
		float	std_dev;
		float	scale;
		for (int i = 0; i < 1; i++)
		{
			_npobj->map_gaussian(max_angle, std_dev, scale);
			_npobj->remove_gaussian(max_angle, std_dev, scale);
		}

		_cached_2_parallel = _npobj->area();
	}

	return _cached_2_parallel;
}

float vifa_int_face_attr::
FourPeakParallel()
{
	if (_cached_4_parallel < 0)
	{
		SetNP();

		float	max_angle;
		float	std_dev;
		float	scale;
		for (int i = 0; i < 3; i++)
		{
			_npobj->map_gaussian(max_angle, std_dev, scale);
			_npobj->remove_gaussian(max_angle, std_dev, scale);
		}

		_cached_4_parallel = _npobj->area();
	}

	return _cached_4_parallel;
}

float vifa_int_face_attr::
EightyPercentParallel()
{
	if (_cached_80_parallel < 0)
	{
		SetNP();

		float	max_angle;
		float	std_dev;
		float	scale;
		int		i=0;

		while ((i < 20) && (_npobj->area() > 0.3))
		{
			i++;
			_npobj->map_gaussian(max_angle, std_dev, scale);
			_npobj->remove_gaussian(max_angle, std_dev, scale);
		}

		_cached_80_parallel = i;
	}

	return _cached_80_parallel;
}

void vifa_int_face_attr::
ComputeCacheValues()
{
	float	real_min = _face->get_min();
	float	real_max = _face->get_max();

	// Sanity check -- can happen if face has 0 pixels...
	if (real_min > real_max)
	{
		real_min = 0.0;
		real_max = 1.0;  // Superstitous, probably could also be zero?
	}

	int				max_bins = int(real_max - real_min + 1);
	int				nbins = (_face->Npix() > max_bins) ? max_bins :
														_face->Npix();
	vifa_histogram	intensity_hist(nbins, real_min, real_max);

	for (int i = 0; i < _face->Npix(); i++)
	{
		float	pval = (_face->Ij())[i];
		intensity_hist.UpCount(pval);
	}

#if 0
	vcl_cout << "vifa::ComputeCacheValues(): start dump:" << vcl_endl;
	intensity_hist.Print();
	vcl_cout << "LowClipVal: " << intensity_hist.LowClipVal(0.1) << vcl_endl;
	vcl_cout << "HighClipVal: " << intensity_hist.LowClipVal(0.9) << vcl_endl;
	vcl_cout << "_face->Io(): " << _face->Io() << vcl_endl;
	vcl_cout << "_face->Npix(): " << _face->Npix() << vcl_endl;
	_np->print_info();
	vcl_cout << "vifa::ComputeCacheValues(): end dump" << vcl_endl;
#endif

	_cached_min = normalize_intensity(intensity_hist.LowClipVal(0.1));
	_cached_max = normalize_intensity(intensity_hist.LowClipVal(0.9));
	_cached_mean = normalize_intensity(_face->Io());

	float					sum = 0.0;
	const unsigned short*	pvals = _face->Ij();

	for (int i = 0; i < _face->Npix(); i++)
	{
		float	mapped_pval = normalize_intensity(pvals[i]);
		float	delta = mapped_pval - _cached_mean;
		sum += delta * delta;
	}

	_cached_var = sum / _face->Npix();
}

void vifa_int_face_attr::
SetNP()
{
	if (_npobj)
	{
		_npobj->reset();
	}
	else
	{
		vcl_vector<vdgl_intensity_face_sptr>	faces;
		faces.push_back(_face);

		const bool	contrast = true;
		_npobj = new vifa_parallel(faces, contrast);
	}
}
