// Georg Meszaros, 19960314

#include "QvMFFace.h"
class Face;

QV_MFIELD_SOURCE(QvMFFace, Face*, 1, FALSE);

QvBool
QvMFFace::read1Value(QvInput * /*in*/, int /*index*/)
{
  return FALSE;
}


void QvMFFace::setValue (int index, Face* face_ptr)
{
  values [index] = face_ptr;
}


Face* QvMFFace::getValue (int index)
{
  return values [index];
}
