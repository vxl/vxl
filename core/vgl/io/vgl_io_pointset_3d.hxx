// This is core/vgl/io/vgl_io_pointset_3d.hxx
#ifndef vgl_io_pointset_3d_hxx_
#define vgl_io_pointset_3d_hxx_
//:
// \file

#include "vgl_io_pointset_3d.h"
#include "vgl/vgl_export.h"

//============================================================================
//: Binary save self to stream.
template <class T>
void
vsl_b_write(vsl_b_ostream & os, const vgl_pointset_3d<T> & ptset)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, ptset.has_normals());
  vsl_b_write(os, ptset.has_scalars());
  vsl_b_write(os, ptset.points());
  if (ptset.has_normals())
  {
    vsl_b_write(os, ptset.normals());
  }
  if (ptset.has_scalars())
  {
    vsl_b_write(os, ptset.scalars());
  }
}

//============================================================================
//: Binary load self from stream.
template <class T>
void
vsl_b_read(vsl_b_istream & is, vgl_pointset_3d<T> & ptset)
{
  if (!is)
    return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
    case 1:
    {

      // normal, scalar boolean flags
      bool has_normals, has_scalars;
      vsl_b_read(is, has_normals);
      vsl_b_read(is, has_scalars);

      // points
      std::vector<vgl_point_3d<T>> points;
      vsl_b_read(is, points);

      // normals
      std::vector<vgl_vector_3d<T>> normals;
      if (has_normals)
      {
        vsl_b_read(is, normals);
      }

      // scalars
      std::vector<T> scalars;
      if (has_scalars)
      {
        vsl_b_read(is, scalars);
      }

      if (has_normals && has_scalars)
        ptset.set_points_with_normals_and_scalars(points, normals, scalars);
      else if (has_normals)
        ptset.set_points_with_normals(points, normals);
      else if (has_scalars)
        ptset.set_points_with_scalars(points, scalars);
      else
        ptset.set_points(points);

      break;
    }

    default:
    {
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_pointset_3d<T>&)\n"
                << "           Unknown version number " << v << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
}


//============================================================================
//: Output a human readable summary to the stream
template <class T>
void
vsl_print_summary(std::ostream & os, const vgl_pointset_3d<T> & ptset)
{
  os << "vgl_pointset_3d (size: " << ptset.size() << ", normals: " << ptset.has_normals()
     << ", scalars: " << ptset.has_scalars() << ")";
}


#define VGL_IO_POINTSET_3D_INSTANTIATE(T)                                                 \
  template VGL_EXPORT void vsl_print_summary(std::ostream &, const vgl_pointset_3d<T> &); \
  template VGL_EXPORT void vsl_b_read(vsl_b_istream &, vgl_pointset_3d<T> &);             \
  template VGL_EXPORT void vsl_b_write(vsl_b_ostream &, const vgl_pointset_3d<T> &)

#endif // vgl_io_pointset_3d_hxx_
