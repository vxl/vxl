#include <vcl_utility.h>
#include <vcl_vector.txx>
#include <vil1/vil1_image.h>
#include <vgui/vgui_gl.h>

typedef vcl_pair<vil1_image, vcl_vector<GLuint> *> T;
VCL_VECTOR_INSTANTIATE(T *);
