#include "QvTexture2.h"

#include <vcl_iostream.h>

QV_NODE_SOURCE(QvTexture2);

void (*QvTexture2::freeTexture_) (int) = 0;


QvTexture2::QvTexture2()
{
    QV_NODE_CONSTRUCTOR(QvTexture2);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(filename);
    QV_NODE_ADD_FIELD(image);
    QV_NODE_ADD_FIELD(wrapS);
    QV_NODE_ADD_FIELD(wrapT);

    filename.value = "";
    image.size[0] = image.size[1] = 0;
    image.numComponents = 0;
    image.bytes = NULL;
    wrapS.value = REPEAT;
    wrapT.value = REPEAT;

    QV_NODE_DEFINE_ENUM_VALUE(Wrap, REPEAT);
    QV_NODE_DEFINE_ENUM_VALUE(Wrap, CLAMP);

    QV_NODE_SET_SF_ENUM_TYPE(wrapS, Wrap);
    QV_NODE_SET_SF_ENUM_TYPE(wrapT, Wrap);

    // mpichler, 19950905
    handle_ = -1;  // < 0: unset;  = 0: failed;  > 0: allocated
    hasalpha_ = 0;
    // mpichler, 19960502
    requested_ = 0;  // external texture not yet requested
    hgtexture_ = 0;  // Hyper-G textures requested via Harmony, others via HTTP
}


void QvTexture2::setHandle (int newhandle, int alpha)
{
  // free old handle
  if (handle_ > 0)  // mpichler, 19950905
  { // function pointer to avoid having to link test programs to ge3d
    if (freeTexture_)
      freeTexture_ (handle_);
    else
      vcl_cerr << "QvTexture2: error: handle_ " << handle_ << " could not freed!" << vcl_endl;
  }
  // mpichler, 19960506
  handle_ = newhandle;
  hasalpha_ = alpha;
}


QvTexture2::~QvTexture2()
{
  setHandle (-1, 0);
}

QvBool
QvTexture2::readInstance(QvInput *in)
{
    QvBool readOK = QvNode::readInstance(in);

    if (readOK && ! filename.isDefault()) {
        if (! readImage())
        { readOK = FALSE;
          image.setDefault(TRUE);
        }
    }

    return readOK;
}

QvBool
QvTexture2::readImage()
{
    // ???
    // ??? Read image from filename and store results in image field.
    // ???

    return TRUE;
}
