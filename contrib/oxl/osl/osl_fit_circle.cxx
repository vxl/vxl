// This is oxl/osl/osl_fit_circle.cxx

#include <iostream>
#include <cmath>
#include "osl_fit_circle.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_2.h>
#include <vnl/algo/vnl_svd.h>

// The well-known square function
static inline double square(double x) { return x * x; }

osl_fit_circle::osl_fit_circle(const std::list<vgl_point_2d<double> > &points)
{
    calculate(points);
}

osl_fit_circle::osl_fit_circle(const osl_edgel_chain &chain)
{
    // Extract points from edgel chain. I agree that this
    // may cause overhead.
    std::list<vgl_point_2d<double> > points;

    for (unsigned int i = 0; i < chain.size(); i++)
    {
        points.emplace_back(
            chain.GetX(i), chain.GetY(i));
    }

    calculate(points);
}

void osl_fit_circle::calculate(const std::list<vgl_point_2d<double> > &points)
{
    int rows = points.size();

    error_ = false;

    // must have at least 3 points to find circle
    if (rows < 3)
    {
        error_ = true;
        return;
    }

    vnl_vector<double> col1(rows);
    vnl_vector<double> col2(rows);
    vnl_vector<double> col3(rows);
    vnl_vector<double> col4(rows);

    auto it = points.begin();
    for (int i = 0; it != points.end(); ++it, ++i)
    {
        col2.put(i, (*it).y());
        col3.put(i, (*it).x());
    }

    for (int i = 0; i < rows; ++i)
    {
        col1.put(i, square(col2.get(i)) + square(col3.get(i)));
        col4.put(i, 1.0);
    }

    vnl_matrix<double> m(rows, 4);

    m.set_column(0, col1);
    m.set_column(1, col2);
    m.set_column(2, col3);
    m.set_column(3, col4);

    vnl_svd<double> svd(m);

    // singular values are stored by vnl_svd in decreasing
    // order, so get last column to get optimal solution
    vnl_vector<double> u = svd.V().get_column(3);

    double a = u(0);
    vnl_double_2 b(u(1),u(2));

    double c = u(3);

    center_.set(-b(1) / 2 / a, -b(0) / 2 / a);
    radius_ = std::sqrt(square(center_.x()) + square(center_.y()) - c / a);

    max_diff_ = avg_diff_ = 0;

    // calculate maximum and average error. this is different
    // to the original routine, which calculates an error sum
    for (it = points.begin(); it != points.end(); it++)
    {
        double cur_diff= std::fabs(std::sqrt(square((*it).x() - center_.x()) +
                                           square((*it).y() - center_.y())) - radius_);

        if (cur_diff > max_diff_)
            max_diff_ = cur_diff;

        avg_diff_ += cur_diff;
    }

    avg_diff_ /= points.size();
}
