//: \file
//  \brief Functions to smooth and sub-sample image in one direction
//  \author Tim Cootes

#include <mil/mil_gauss_reduce_2d.h>


//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
void mil_gauss_reduce_2d(vil_byte* dest_im,
                     int d_x_step, int d_y_step,
					 const vil_byte* src_im,
					 int src_nx, int src_ny,
					 int s_x_step, int s_y_step)
{
	vil_byte* d_row = dest_im;
	const vil_byte* s_row = src_im;
	int sxs2 = s_x_step*2;
	int nx2 = (src_nx-3)/2;
	for (int y=0;y<src_ny;++y)
	{
		// Set first element of row
		*d_row = *s_row;
		vil_byte * d = d_row + d_x_step;
		const vil_byte* s = s_row + sxs2;
		for (int x=0;x<nx2;++x)
		{
		    // The 0.5 offset in the following ensures rounding
			*d = vil_byte(0.5+ 0.05*s[-sxs2] +0.05 *s[sxs2]
			               +0.25*s[-s_x_step]+0.25*s[s_x_step]
				           +0.4*s[0]);

			d += d_x_step;
			s += sxs2;
		}
		// Set last elements of row
		*d = *s;

		d_row += d_y_step;
		s_row += s_y_step;
	}
}

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
void mil_gauss_reduce_2d(float* dest_im,
                     int d_x_step, int d_y_step,
					 const float* src_im,
					 int src_nx, int src_ny,
					 int s_x_step, int s_y_step)
{
	float* d_row = dest_im;
	const float* s_row = src_im;
	int sxs2 = s_x_step*2;
	int nx2 = (src_nx-3)/2;
	for (int y=0;y<src_ny;++y)
	{
		// Set first element of row
		*d_row = *s_row;
		float * d = d_row + d_x_step;
		const float* s = s_row + sxs2;
		for (int x=0;x<nx2;++x)
		{
			*d = 0.05*(s[-sxs2] + s[sxs2])
			    +0.25*(s[-s_x_step]+ s[s_x_step])
				+0.40*s[0];

			d += d_x_step;
			s += sxs2;
		}
		// Set last elements of row
		*d = *s;

		d_row += d_y_step;
		s_row += s_y_step;
	}
}
