#include <vcl_utility.h>
#include <vcl_vector.txx>
#include <vil/vil_image.h>
#include <vgui/vgui_gl.h>

typedef vcl_pair<vil_image, vcl_vector<GLuint> *> T;
VCL_VECTOR_INSTANTIATE(T *);
