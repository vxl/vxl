#ifndef vdgl_intensity_face_h_
#define vdgl_intensity_face_h_

#include <vtol/vtol_intensity_face.h>
#include <vtol/vtol_intensity_face_sptr.h>

// vdgl_intensity_face has been renamed to vtol_intensity_face,
// because that's where it properly belongs. Please include
// <vtol/vtol_intensity_face.h> and replace all instances of
// vdgl_intensity_face with vtol_intensity_face

#include <vcl_deprecated_header.h>

typedef vtol_intensity_face vdgl_intensity_face;
typedef vtol_intensity_face_sptr vdgl_intensity_face_sptr;

#endif // vdgl_intensity_face_h_
