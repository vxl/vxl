#ifndef boxm2_vecf_inverse_square_weighting_function_h_included_
#define boxm2_vecf_inverse_square_weighting_function_h_included_


//: function object taking distance squared and returning control point weight
class boxm2_vecf_inverse_square_weighting_function
{
  public:
    //: create a function of the form 1/(a*x^2 + 1/max_val)
    boxm2_vecf_inverse_square_weighting_function(double max_val, double x_at_10_percent_max);

    //: the weight function
    double operator()(double x) const { return 1.0/(a_*x + inv_max_val_); }
private:
    double inv_max_val_;
    double a_;
};

#endif
