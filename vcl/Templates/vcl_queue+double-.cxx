#include <vcl_queue.txx>
#include <vcl_vector.h>
#include <vcl_functional.h>

VCL_QUEUE_INSTANTIATE(double);
VCL_PRIORITY_QUEUE_INSTANTIATE(vcl_vector<double>, double, vcl_less<double>);
VCL_PRIORITY_QUEUE_INSTANTIATE(vcl_vector<double>, double, vcl_greater<double>);
