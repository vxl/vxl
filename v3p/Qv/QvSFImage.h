#ifndef  _QV_SF_IMAGE_
#define  _QV_SF_IMAGE_

#include "QvSubField.h"

class QvSFImage : public QvSField {
  public:
    short               size[2];        // Width and height of image
    int                 numComponents;  // Number of components per pixel
    unsigned char *     bytes;          // Array of pixels
    QV_SFIELD_HEADER(QvSFImage);
};

#endif /* _QV_SF_IMAGE_ */
