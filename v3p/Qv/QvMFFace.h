// Georg Meszaros, 19960314
// gmes-TODO: derive from QvPList instead!!!

#ifndef  _QV_MF_FACE_
#define  _QV_MF_FACE_

#include "QvSubField.h"
class Face;

class QvMFFace : public QvMField {
  public:
    Face** values;

    void setValue (int index, Face* face_ptr);
    Face* getValue (int index);

    QV_MFIELD_HEADER(QvMFFace);
};

#endif /* _QV_MF_FACE_ */
