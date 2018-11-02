// This is core/vpgl/algo/vpgl_ray_intersect.hxx
#ifndef vpgl_ray_intersect_hxx_
#define vpgl_ray_intersect_hxx_

#include <iostream>
#include <utility>
#include "vpgl_ray_intersect.h"
//:
// \file
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_3.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>

template<typename T>
class vpgl_ray_intersect_lsqr : public vnl_least_squares_function
{
public:
    //: Constructor
    vpgl_ray_intersect_lsqr(std::vector<const vpgl_camera<T>* >  cams,
                            std::vector<vgl_point_2d<T> >  image_pts,
                            unsigned num_residuals);

    //: Destructor
    ~vpgl_ray_intersect_lsqr() override = default;

    //: The main function.
    //  Given the parameter vector x, compute the vector of residuals fx.
    //  fx has been sized appropriately before the call.
    void f(vnl_vector<double> const& intersection_point,
                   vnl_vector<double>& image_errors) override;

#if 0
    //: Called after each LM iteration to print debugging etc.
    virtual void trace(int iteration, vnl_vector<double> const& x, vnl_vector<double> const& fx);
#endif

protected:
    vpgl_ray_intersect_lsqr();//not valid
    std::vector<const vpgl_camera<T>* > f_cameras_; //cameras
    std::vector<vgl_point_2d<T> > f_image_pts_; //image points
};

template<typename T>
vpgl_ray_intersect_lsqr<T>::
vpgl_ray_intersect_lsqr(std::vector<const vpgl_camera<T>* > cams,
                        std::vector<vgl_point_2d<T> > image_pts,
                        unsigned num_residuals) :
vnl_least_squares_function(3, num_residuals,
                           vnl_least_squares_function::no_gradient ),
f_cameras_(std::move(cams)),
f_image_pts_(std::move(image_pts))
{}

// Define virtual function for the LeastSquaresFunction class.  Given
// a conjectured point (intersection_vert) in space, this determines the
// error vector formed by appending all <u - conjecture_u, v - conjecture_v>
// error tuples for each image.  Here <u, v> is the actual image point, and
// <conjecture_u, conjecture_v> is the point corresponding to intersection_vert.
template<typename T>
void vpgl_ray_intersect_lsqr<T>::f(vnl_vector<double> const& intersection_point,
                                vnl_vector<double>& image_errors)
{
    // Get the size of the error vector
    std::size_t dim = static_cast<unsigned int>(image_errors.size() / 2);

    // Initialize huge error
    double huge = vnl_numeric_traits<double>::maxval;
    for (unsigned i=0; i<image_errors.size(); i++)
        image_errors.put(i, huge);

    // Compute the error in each image
    double intersection_point_x =  intersection_point[0];
    double intersection_point_y =  intersection_point[1];
    double intersection_point_z =  intersection_point[2];
#ifdef RAY_INT_DEBUG
    std::cout << "Error Vector (" << intersection_point_x << ", "
    << intersection_point_y << ", " << intersection_point_z << ") = ";
#endif

    for (unsigned image_no = 0; image_no < dim; image_no++)
    {
        // Get this camera and corresponding image point
        const vpgl_camera<T>* cam = f_cameras_[image_no];
        double image_u = f_image_pts_[image_no].x(),
        image_v = f_image_pts_[image_no].y();
        // Compute the image of the intersection vert through this camera
        T cur_image_u, cur_image_v;
        cam->project(intersection_point_x, intersection_point_y,
                     intersection_point_z, cur_image_u, cur_image_v);
        // Compute and store the error with respect to actual image
        image_errors.put(2*image_no,
                         (cur_image_u - image_u));
        image_errors.put(2*image_no+1,
                         (cur_image_v - image_v));
#ifdef RAY_INT_DEBUG
        std::cout << " x_err = " << cur_image_u << '-' << image_u << '='
        << image_errors[2*image_no]
        << " y_err = " << cur_image_v << '-' << image_v << '='
        << image_errors[2*image_no+1];
#endif


#ifdef RAY_INT_DEBUG
        std::cout << '\n';
#endif
    }
}

// Constructor.
template<typename T>
vpgl_ray_intersect<T>::vpgl_ray_intersect(unsigned dim): dim_(dim)
{}

// A function to get the point closest to the rays coming out of image_pts
// in images, whose cameras (cams) are known.  Point is stored in intersection.
// Returns true if successful, else false
template<typename T>
bool vpgl_ray_intersect<T>::
intersect(std::vector<const vpgl_camera<T>* > const& cams,
          std::vector<vgl_point_2d<T> > const& image_pts,
          vgl_point_3d<T> const& initial_intersection,
          vgl_point_3d<T>& intersection)
{
  // Make sure the dimension is at least 2
  if (dim_ < 2)
  {
    std::cerr << "The dimension is too small.  There must be at least 2 images"
             << '\n';
    return false;
  }

  // Make sure there are correct number of cameras
  if (cams.size() != dim_)
  {
    std::cerr << "Please provide correct number of cameras" << '\n';
    return false;
  }

  // Make sure there are correct number of image points
  if (image_pts.size() != dim_)
  {
    std::cerr << "Please provide correct number of image points" << '\n';
    return false;
  }

  // cache the image points and camera points
  f_cameras_ = cams;
  f_image_pts_ = image_pts;

  // Create the Levenberg Marquardt minimizer
  vpgl_ray_intersect_lsqr<T> lqf(cams, image_pts, 2*dim_);
  vnl_levenberg_marquardt levmarq(lqf);
#ifdef RAY_INT_DEBUG
  std::cout << "Created LevenbergMarquardt minimizer ... setting tolerances\n";
  levmarq.set_verbose(true);
#endif
  // Set the x-tolerance.  When the length of the steps taken in X (variables)
  // are no longer than this, the minimization terminates.
  levmarq.set_x_tolerance(1e-10);

  // Set the epsilon-function.  This is the step length for FD Jacobian.
  levmarq.set_epsilon_function(1.0);

  // Set the f-tolerance.  When the successive RMS errors are less than this,
  // minimization terminates.
  levmarq.set_f_tolerance(1e-10);

  // Set the maximum number of iterations
  levmarq.set_max_function_evals(10000);
  vnl_double_3 intersection_pt;
  intersection_pt[0]=initial_intersection.x();
  intersection_pt[1]=initial_intersection.y();
  intersection_pt[2]=initial_intersection.z();

#ifdef RAY_INT_DEBUG
  std::cout << "Initialized the intersection point " << intersection_pt
           << " ... minimizing\n";
#endif

  // Minimize the error and get the best intersection point
  levmarq.minimize(intersection_pt);

  // Summarize the results
#ifdef RAY_INT_DEBUG
  std::cout << "Min error of " << levmarq.get_end_error() << " at "
           << '(' << intersection_pt[0] << ", " << intersection_pt[1]
           << ", " << intersection_pt[2] << ")\n"
           << "Iterations: " << levmarq.get_num_iterations() << "    "
           << "Evaluations: " << levmarq.get_num_evaluations() << '\n';
  levmarq.diagnose_outcome();
#endif
  // Assign the intersection
  intersection.set(intersection_pt[0],
                   intersection_pt[1],
                   intersection_pt[2]);
  // Return success
  return true;
}

#define VPGL_RAY_INTERSECT_INSTANTIATE(T) \
template class vpgl_ray_intersect<T >;


#endif // vpgl_ray_intersect_hxx_
