#ifndef mil3d_sample_grid_3d_txx_
#define mil3d_sample_grid_3d_txx_

//: \file
//  \brief Profile sampling functions for 3D images
//  \author Graham Vincent

#include <mil3d/mil3d_sample_grid_3d.h>
#include <mil3d/mil3d_trilin_interp_3d.h>
#include <vnl/vnl_vector.h>

//: True if p clearly inside the image
inline bool mil3d_point_in_image(const vgl_point_3d<double>& p, const mil3d_image_3d& image)
{
  if (p.x()<1) return false;
  if (p.y()<1) return false;
  if (p.z()<1) return false;
  if (p.x()+2>image.nx()) return false;
  if (p.y()+2>image.ny()) return false;
  if (p.z()+2>image.nz()) return false;
  return true;
}

//: True if grid of size nu * nv * nw (in steps of u,v,w) is entirely in the image.
//  p defines centre of one size.
inline bool mil3d_grid_in_image(const vgl_point_3d<double>& p,
                              const vgl_vector_3d<double>& u,
                              const vgl_vector_3d<double>& v,
                              const vgl_vector_3d<double>& w,
                              int nu, int nv, int nw,
                              const mil3d_image_3d& image)
{
  vgl_vector_3d<double> u1=(nu-1)*u;
  vgl_vector_3d<double> v1=(nv-1)*v;
  vgl_vector_3d<double> w1=(nw-1)*w;
  if (!mil3d_point_in_image(image.world2im()(p),image)) return false;
  if (!mil3d_point_in_image(image.world2im()(p+u1),image)) return false;
  if (!mil3d_point_in_image(image.world2im()(p+v1),image)) return false;
  if (!mil3d_point_in_image(image.world2im()(p+w1),image)) return false;
  if (!mil3d_point_in_image(image.world2im()(p+u1+v1),image)) return false;
  if (!mil3d_point_in_image(image.world2im()(p+u1+w1),image)) return false;
  if (!mil3d_point_in_image(image.world2im()(p+v1+w1),image)) return false;
  if (!mil3d_point_in_image(image.world2im()(p+u1+v1+w1),image)) return false;

  return true;
}


//: True if grid of size nu * nv * nw (in steps of u,v,w) is entirely in the image.
//  p defines centre of one size.
inline bool mil3d_grid_in_image_c(const vgl_point_3d<double>& im_p,
                              const vgl_vector_3d<double>& im_u,
                              const vgl_vector_3d<double>& im_v,
                              const vgl_vector_3d<double>& im_w,
                              int nu, int nv, int nw,
                              const mil3d_image_3d& image)
{
  vgl_vector_3d<double> u1=(nu-1)*im_u;
  vgl_vector_3d<double> v1=(nv-1)*im_v;
  vgl_vector_3d<double> w1=(nw-1)*im_w;
  if (!mil3d_point_in_image(im_p,image)) return false;
  if (!mil3d_point_in_image(im_p+u1,image)) return false;
  if (!mil3d_point_in_image(im_p+v1,image)) return false;
  if (!mil3d_point_in_image(im_p+w1,image)) return false;
  if (!mil3d_point_in_image(im_p+u1+v1,image)) return false;
  if (!mil3d_point_in_image(im_p+u1+w1,image)) return false;
  if (!mil3d_point_in_image(im_p+v1+w1,image)) return false;
  if (!mil3d_point_in_image(im_p+u1+v1+w1,image)) return false;

  return true;
}

//: Sample grid p+i.u+j.v+k.w using trilinear interpolation in world coordinates
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
template <class imType, class vecType>
void mil3d_sample_grid_3d(vnl_vector<vecType>& vec,
                        const mil3d_image_3d_of<imType>& image,
                        const vgl_point_3d<double>& p,
                        const vgl_vector_3d<double>& u,
                        const vgl_vector_3d<double>& v,
                        const vgl_vector_3d<double>& w,
                        int nu, int nv, int nw)
{
	// convert to image coordinates
    vgl_point_3d<double> im_p0 = image.world2im()(p);
    vgl_vector_3d<double> im_u = image.world2im()(p+u)-im_p0;
    vgl_vector_3d<double> im_v = image.world2im()(p+v)-im_p0;
    vgl_vector_3d<double> im_w = image.world2im()(p+w)-im_p0;

	// call image coordinate version of grid sampler
	mil3d_sample_grid_3d_ic(vec,image,im_p0,im_u,im_v,im_w,nu,nv,nw);
 
    return;
}


//: Sample grid p+i.u+j.v+k.w using trilinear interpolation in image coordinates
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
template <class imType, class vecType>
void mil3d_sample_grid_3d_ic(vnl_vector<vecType>& vec,
                        const mil3d_image_3d_of<imType>& image,
                        const vgl_point_3d<double>& im_p,
                        const vgl_vector_3d<double>& im_u,
                        const vgl_vector_3d<double>& im_v,
                        const vgl_vector_3d<double>& im_w,
                        int nu, int nv, int nw)
{
    if (mil3d_grid_in_image(im_p,im_u,im_v,im_w,nu,nv,nw,image))
      mil3d_sample_grid_3d_ic_no_checks(vec,image,im_p,im_u,im_v,im_w,nu,nv,nw);
    else
      mil3d_sample_grid_3d_ic_safe(vec,image,im_p,im_u,im_v,im_w,nu,nv,nw);

    return;
}

//: Sample grid p+i.u+j.v+k.w using safe trilinear interpolation.
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
//  Points outside image return zero.
template <class imType, class vecType>
void mil3d_sample_grid_3d_safe(vnl_vector<vecType>& vec,
                             const mil3d_image_3d_of<imType>& image,
                             const vgl_point_3d<double>& p0,
                             const vgl_vector_3d<double>& u,
                             const vgl_vector_3d<double>& v,
                             const vgl_vector_3d<double>& w,
                             int nu, int nv, int nw)
{
  int np = image.n_planes();
  int nx = image.nx();
  int ny = image.ny();
  int nz = image.nz();
  int xstep = image.xstep();
  int ystep = image.ystep();
  int zstep = image.zstep();

  vec.resize(nu*nv*nw*np);
  vecType* vc = vec.begin();

  vgl_point_3d<double> p1 = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<nu;++i)
    {
		vgl_point_3d<double> p2 = p1;
		for (int j=0;j<nv;++j)
		{
			vgl_point_3d<double> p = p2;
	         // Sample each row (along w)
			for (int k=0;k<nw;++k)
		   	{
				vgl_point_3d<double> im_p = image.world2im()(p);
				*vc = mil3d_safe_trilin_interp_3d(im_p.x(),im_p.y(),im_p.z(),plane0,nx,ny,nz,xstep,ystep,zstep);
				vc++;
				p+=w;
	        }
			p2+=v;
	   }
       p1+=u;
    }
  }
  else
  {
    for (int i=0;i<nu;++i)
    {
		vgl_point_3d<double> p2 = p1;
		for (int j=0;j<nv;++j)
		{
			vgl_point_3d<double> p = p2;
	         // Sample each row (along w)
			for (int k=0;k<nw;++k)
		   	{
				vgl_point_3d<double> im_p = image.world2im()(p);
		        for (int k=0;k<np;++k)
				{
					  *vc = mil3d_safe_trilin_interp_3d(im_p.x(),im_p.y(),im_p.z(),image.plane(k),nx,ny,nz,xstep,ystep,zstep);
				      vc++;
				}
				p+=w;
	        }
			p2+=v;
	   }
       p1+=u;
    }
}  
}


//: Sample grid p+i.u+j.v+k.w using safe trilinear interpolation.
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
//  Points outside image return zero.
template <class imType, class vecType>
void mil3d_sample_grid_3d_no_checks(vnl_vector<vecType>& vec,
                             const mil3d_image_3d_of<imType>& image,
                             const vgl_point_3d<double>& p0,
                             const vgl_vector_3d<double>& u,
                             const vgl_vector_3d<double>& v,
                             const vgl_vector_3d<double>& w,
                             int nu, int nv, int nw)
{
  int np = image.n_planes();
  int nx = image.nx();
  int ny = image.ny();
  int nz = image.nz();
  int xstep = image.xstep();
  int ystep = image.ystep();
  int zstep = image.zstep();

  vec.resize(nu*nv*nw*np);
  vecType* vc = vec.begin();

  vgl_point_3d<double> p1 = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<nu;++i)
    {
		vgl_point_3d<double> p2 = p1;
		for (int j=0;j<nv;++j)
		{
			vgl_point_3d<double> p = p2;
	         // Sample each row (along w)
			for (int k=0;k<nw;++k)
		   	{
				vgl_point_3d<double> im_p = image.world2im()(p);
				*vc = mil3d_trilin_interp_3d(im_p.x(),im_p.y(),im_p.z(),plane0,xstep,ystep,zstep);
				vc++;
				p+=w;
	        }
			p2+=v;
	   }
       p1+=u;
    }
  }
  else
  {
   for (int i=0;i<nu;++i)
    {
		vgl_point_3d<double> p2 = p1;
		for (int j=0;j<nv;++j)
		{
			vgl_point_3d<double> p = p2;
	         // Sample each row (along w)
			for (int k=0;k<nw;++k)
		   	{
				vgl_point_3d<double> im_p = image.world2im()(p);
		        for (int k=0;k<np;++k)
				{
					  *vc = mil3d_safe_trilin_interp_3d(im_p.x(),im_p.y(),im_p.z(),image.plane(k),nx,ny,nz,xstep,ystep,zstep);
				      vc++;
				}
				p+=w;
	        }
			p2+=v;
	   }
       p1+=u;
    }
}  
}

//: Sample grid p+i.u+j.v+k.w in image coordinates using trilinear interpolation with NO CHECKS
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
//  Points outside image return zero.
template <class imType, class vecType>
void mil3d_sample_grid_3d_ic_no_checks(vnl_vector<vecType>& vec,
                             const mil3d_image_3d_of<imType>& image,
                             const vgl_point_3d<double>& p0,
                             const vgl_vector_3d<double>& u,
                             const vgl_vector_3d<double>& v,
                             const vgl_vector_3d<double>& w,
                             int nu, int nv, int nw)
{

  int np = image.n_planes();
  int nx = image.nx();
  int ny = image.ny();
  int nz = image.nz();
  int xstep = image.xstep();
  int ystep = image.ystep();
  int zstep = image.zstep();

  vec.resize(nu*nv*nw*np);
  vecType* vc = vec.begin();

  vgl_point_3d<double> p1 = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<nu;++i)
    {
		vgl_point_3d<double> p2 = p1;
		for (int j=0;j<nv;++j)
		{
			vgl_point_3d<double> p = p2;
	         // Sample each row (along w)
			for (int k=0;k<nw;++k)
		   	{
				*vc = mil3d_trilin_interp_3d(p.x(),p.y(),p.z(),plane0,xstep,ystep,zstep);
				vc++;
				p+=w;
	        }
			p2+=v;
	   }
       p1+=u;
    }
  }
  else
  {
   for (int i=0;i<nu;++i)
    {
		vgl_point_3d<double> p2 = p1;
		for (int j=0;j<nv;++j)
		{
			vgl_point_3d<double> p = p2;
	         // Sample each row (along w)
			for (int k=0;k<nw;++k)
		   	{
		        for (int k=0;k<np;++k)
				{
					  *vc = mil3d_trilin_interp_3d(p.x(),p.y(),p.z(),image.plane(k),xstep,ystep,zstep);
				      vc++;
				}
				p+=w;
	        }
			p2+=v;
	   }
       p1+=u;
    }
}  
}

//: Sample grid p+i.u+j.v+k.w safely in image coordinates using trilinear interpolation
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
//  Points outside image return zero.
template <class imType, class vecType>
void mil3d_sample_grid_3d_ic_safe(vnl_vector<vecType>& vec,
                             const mil3d_image_3d_of<imType>& image,
                             const vgl_point_3d<double>& p0,
                             const vgl_vector_3d<double>& u,
                             const vgl_vector_3d<double>& v,
                             const vgl_vector_3d<double>& w,
                             int nu, int nv, int nw)
{

  int np = image.n_planes();
  int nx = image.nx();
  int ny = image.ny();
  int nz = image.nz();
  int xstep = image.xstep();
  int ystep = image.ystep();
  int zstep = image.zstep();

  vec.resize(nu*nv*nw*np);
  vecType* vc = vec.begin();

  vgl_point_3d<double> p1 = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<nu;++i)
    {
		vgl_point_3d<double> p2 = p1;
		for (int j=0;j<nv;++j)
		{
			vgl_point_3d<double> p = p2;
	         // Sample each row (along w)
			for (int k=0;k<nw;++k)
		   	{
				*vc = mil3d_safe_trilin_interp_3d(p.x(),p.y(),p.z(),plane0,nx,ny,nz,xstep,ystep,zstep);
				vc++;
				p+=w;
	        }
			p2+=v;
	   }
       p1+=u;
    }
  }
  else
  {
   for (int i=0;i<nu;++i)
    {
		vgl_point_3d<double> p2 = p1;
		for (int j=0;j<nv;++j)
		{
			vgl_point_3d<double> p = p2;
	         // Sample each row (along w)
			for (int k=0;k<nw;++k)
		   	{
		        for (int k=0;k<np;++k)
				{
					  *vc = mil3d_safe_trilin_interp_3d(p.x(),p.y(),p.z(),image.plane(k),nx,ny,nz,xstep,ystep,zstep);
				      vc++;
				}
				p+=w;
	        }
			p2+=v;
	   }
       p1+=u;
    }
}  
}



#define MIL3D_SAMPLE_GRID_3D_INSTANTIATE( imType, vecType ) \
template void mil3d_sample_grid_3d(vnl_vector<vecType >& vec, \
                                 const mil3d_image_3d_of<imType >& image, \
                                 const vgl_point_3d<double >& p, \
                                 const vgl_vector_3d<double >& u, \
                                 const vgl_vector_3d<double >& v, \
                                 const vgl_vector_3d<double >& w, \
                                 int nu, int nv, int nw); \
template void mil3d_sample_grid_3d_safe(vnl_vector<vecType >& vec, \
                                      const mil3d_image_3d_of<imType >& image, \
                                      const vgl_point_3d<double >& p, \
                                      const vgl_vector_3d<double >& u, \
                                      const vgl_vector_3d<double >& v, \
                                      const vgl_vector_3d<double >& w, \
                                      int nu, int nv, int nw); \
template void mil3d_sample_grid_3d_no_checks(vnl_vector<vecType >& vec, \
                                           const mil3d_image_3d_of<imType >& image, \
                                           const vgl_point_3d<double >& p, \
                                           const vgl_vector_3d<double >& u, \
                                           const vgl_vector_3d<double >& v, \
                                           const vgl_vector_3d<double >& w, \
                                           int nu, int nv, int nw); \
template void mil3d_sample_grid_3d_ic_safe(vnl_vector<vecType >& vec, \
                                         const mil3d_image_3d_of<imType >& image, \
                                         const vgl_point_3d<double >& p, \
                                         const vgl_vector_3d<double >& u, \
                                         const vgl_vector_3d<double >& v, \
                                         const vgl_vector_3d<double >& w, \
                                         int nu, int nv, int nw); \
template void mil3d_sample_grid_3d_ic_no_checks(vnl_vector<vecType >& vec, \
                                              const mil3d_image_3d_of<imType >& image, \
                                              const vgl_point_3d<double >& p0, \
                                              const vgl_vector_3d<double >& u, \
                                              const vgl_vector_3d<double >& v, \
                                              const vgl_vector_3d<double >& w, \
                                              int nu, int nv, int nw)

#endif // mil3d_sample_grid_3d_txx_
