// This is contrib/gel/vifa/vifa_line_cover.cxx
#include <vcl_algorithm.h>
#include <vifa/vifa_line_cover.h>


//-----------------------------------------------------------
// -- Constructors

vifa_line_cover::
vifa_line_cover()
{
	_line = NULL;
	_index = NULL;
	_max_extent = NULL;
	_min_extent = NULL;
	_dim =0;
}

vifa_line_cover::
vifa_line_cover(imp_line_sptr	prototype_line,
			    int				_indexdimension)
{
	_line = prototype_line;
	_dim = _indexdimension;
	_index = new int[_dim];
	_max_extent = new double[_dim];
	_min_extent = new double[_dim];
	for (int i = 0; i < _dim; i++)
	{
		_index[i] = 0;
		_max_extent[i] = -1E6;
		_min_extent[i] = 1E6;
	}
}

//------------------------------------------------------------
// -- Destructor
vifa_line_cover::
~vifa_line_cover()
{
	delete [] _index;
	delete [] _max_extent;
	delete [] _min_extent;
}

//------------------------------------------------------------
// -- Insert a line in the index. The line is inserted in each index
//    bin it covers.
void vifa_line_cover::
InsertLine(imp_line_sptr	l)
{
	int	rdim1 = _dim - 1;

	// Find the 2-D projection of l onto the prototype line
	// That is, find the parameter bounds
	vgl_point_2d<double>	sp;
	vgl_point_2d<double>	ep;
	sp = l->point1();
	ep = l->point2();
	double	ts = _line->find_t(sp);
	double	te = _line->find_t(ep);

	// Get properly ordered and limited parameter bounds
	int ist = 0;
	int	iend = 0;
	if (ts < te)
	{
		if (ts < 0)
		{
			ts = 0;
		}
		if (te > 1.0)
		{
			te = 1.0;
		}

		ist = (int)(rdim1 * ts);
		iend = (int)(rdim1 * te);
	}
	else
	{
		if (te < 0)
		{
			te = 0;
		}
		if (ts > 1.0)
		{
			ts = 1.0;
		}

		ist = (int)(rdim1 * te);
		iend = (int)(rdim1 * ts);
	}

	// Next accumulate the bins for each line coverage
	for (int i = ist; i <= iend; i++)
	{
		_index[i]++;
		double					t = (double)i / (double)rdim1;
		vgl_point_2d<double>	pos = _line->find_at_t(t);

		vgl_point_2d<double>	p = l->project_2d_pt(pos);
		double					d = this->get_signed_distance(p);

		_min_extent[i] = vcl_min(_min_extent[i], d);
		_max_extent[i] = vcl_max(_max_extent[i], d);
	}
}

//---------------------------------------------------------------
// --Get the average coverage along the line. Normalized by the
//   full bounding box diagonal.  Coverage must be
//   greater than one line in order to indicate mutual coverage
double vifa_line_cover::
GetCoverage()
{
	int	sum = 0;
	for (int i = 0; i < _dim; i++)
	{
		int	nlines = _index[i];
		if (nlines > 1)
		{
			sum += (nlines - 1);
		}
	}

	return (double)sum / _dim;
}

//---------------------------------------------------------------
// --Get the average coverage along the line.  Coverage must be
//   greater than one line in order to indicate mutual coverage
double vifa_line_cover::
GetDenseCoverage()
{
	double	cov = this->GetCoverage();
	double	cover_extent = double(this->get_index_max() - this->get_index_min());
	if (cover_extent <= 0)
	{
		return 0.0;
	}
	else
	{
		return (_dim * cov) / cover_extent;
	}
}


//-------------------------------------------------------------
// -- Get coverage relative to the total perimeter of indexed lines
// 
double vifa_line_cover::
GetCustomCoverage(const double	norm)
{
	double	total_cover = this->GetCoverage() * _dim;
	if (norm <= 0)
	{
		return 0.0;
	}
	else
	{
		return total_cover / norm;
	}
}
//---------------------------------------------------------------
// --Get extent of image space where parallel lines
//   overlap under projection onto the prototype.
void vifa_line_cover::
GetExtent(imp_line_sptr&	lmin,
		  imp_line_sptr&	lmax)
{
	if (!_line)
	{
		lmin = NULL;
		lmax = NULL;
		return;
	}

	int		st = get_index_min();
	int		en = get_index_max();
	double	min_ex = 1E6;
	double	max_ex = -1E6;

	// Scan for the max and min lateral extents
	for (int i = st; i <= en; i++)
	{
		min_ex = vcl_min(_min_extent[i], min_ex);
		max_ex = vcl_max(_max_extent[i], max_ex);
	}

	// Construct min and max bounding lines
	lmin = get_offset_line(st, en, min_ex);
	lmax = get_offset_line(st, en, max_ex);
}

int vifa_line_cover::
get_index_min()
{
	for (int i = 1; (i < _dim); i++)
	{
		if (_index[i] > 1)
		{
			return i;
		}
	}

	return 0;
}	

int vifa_line_cover::
get_index_max()
{
	for (int i =_dim - 1; i > 0; i--)
	{
		if (_index[i] > 1)
		{
			return i;
		}
	}
	return 0;
}

// Signed distance (algebraic dist) of a point from the prototype line
double vifa_line_cover::
get_signed_distance(vgl_point_2d<double> const &	p)
{
	double	a = _line->a(), b = _line->b(), c = _line->c();
	double	d = a * p.x() + b * p.y() + c;
	return d;
}

// Get a bounded ImplicitLine which is offset from the prototype line
// by the signed distance, d.
imp_line_sptr vifa_line_cover::
get_offset_line(int		start,
				int		end,
				double	d)
{
	// The normal vector
	vgl_vector_2d<double>	n = _line->normal();

	// The end points on the prototype line
	double					ts = (double)start / (double)(_dim - 1);
	vgl_point_2d<double>	ps = _line->find_at_t(ts);
	double					te = (double)end / (double)(_dim - 1);
	vgl_point_2d<double>	pe = _line->find_at_t(te);

	n *= d; // Extend the normal
	ps += n; // The offset start point
	pe += n; // The offset end point
	imp_line_sptr	il = new imp_line(ps, pe);
	return il;
}
