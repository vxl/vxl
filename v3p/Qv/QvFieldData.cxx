#include "QvFieldData.h"
#include "QvInput.h"
#include "QvReadError.h"
#include "QvField.h"
#include "QvUnknownNode.h"

#include <vcl_cstddef.h>

#define OPEN_BRACE_CHAR         '['
#define CLOSE_BRACE_CHAR        ']'
#define VALUE_SEPARATOR_CHAR    ','

struct QvFieldEntry {
    QvName              name;
    vcl_ptrdiff_t       offset;
};

struct QvEnumEntry {
    QvName              typeName;
    int                 num;
    int                 arraySize;
    int                 *vals;
    QvName              *names;
    QvEnumEntry(const QvName &name);
    ~QvEnumEntry();

    static int          growSize;
};

int QvEnumEntry::growSize = 6;

QvEnumEntry::QvEnumEntry(const QvName &name)
{
    typeName    = name;
    num         = 0;
    arraySize   = growSize;
    vals        = new int[arraySize];
    names       = new QvName[arraySize];
}

QvEnumEntry::~QvEnumEntry()
{
    delete [] vals;
    delete [] names;
}

QvFieldData::~QvFieldData()
{
    struct QvFieldEntry *tmpField;
    struct QvEnumEntry  *tmpEnum;

    for (int i=0; i<fields.getLength(); i++) {
        tmpField = (struct QvFieldEntry *)fields[i];
        delete tmpField;
    }

    for (int j=0; j<enums.getLength(); j++) {
        tmpEnum = (struct QvEnumEntry *)enums[j];
        delete tmpEnum;
    }
}

void
QvFieldData::addField(QvNode *defobj, const char *fieldName,
                      const QvField *field)
{
    struct QvFieldEntry *newField = new /*struct*/ QvFieldEntry;  // mpichler (gcc)
    newField->name   = fieldName;
    newField->offset = (const char *) field - (const char *) defobj;

    fields.append((void *) newField);
}

const QvName &
QvFieldData::getFieldName(int index) const
{
    return ((QvFieldEntry *) fields[index])->name;
}

QvField const*
QvFieldData::getField(const QvNode *object, int index) const
{
    return (QvField const*) ((const char *) object +
                             ((QvFieldEntry const *) fields[index])->offset);
}

#include <vcl_cctype.h>
static QvName
stripWhite(const char *name)
{
    int firstchar = -1;
    int lastchar = -1;
    int lastwhite = -1;

    int i;  // mpichler (gcc)
    for (i=0; name[i]; i++) {
        if (vcl_isspace(name[i]))
            lastwhite = i;
        else {
            if (firstchar == -1) firstchar = i;
            lastchar = i;
        }
    }

    if (lastchar > lastwhite)
        return QvName(&name[firstchar]);

    char buf[500];
    int b;
    for (b=0, i=firstchar; i<=lastchar; i++, b++)
        buf[b] = name[i];
    buf[b] = 0;
    return QvName(buf);
}

void
QvFieldData::addEnumValue(const char *typeNameArg,
                          const char *valNameArg, int val)
{
    struct QvEnumEntry  *e = NULL;
    QvName typeName = stripWhite(typeNameArg);
    QvName valName = stripWhite(valNameArg);

    for (int i=0; i<enums.getLength(); i++) {
        e = (struct QvEnumEntry *) enums[i];
        if (e->typeName == typeName)
            break;
        else
            e = NULL;
    }
    if (e == NULL) {
        e = new QvEnumEntry(typeName);
        enums.append((void*) e);
    }
    if (e->num == e->arraySize) {
        e->arraySize += QvEnumEntry::growSize;
        int *ovals = e->vals;
        QvName *onames = e->names;
        e->vals = new int[e->arraySize];
        e->names = new QvName[e->arraySize];
        for (int i=0; i<e->num; i++) {
            e->vals[i] = ovals[i];
            e->names[i] = onames[i];
        }
        delete [] ovals;
        delete [] onames;
    }
    e->vals[e->num] = val;
    e->names[e->num] = valName;
    e->num++;
}

void
QvFieldData::getEnumData(const char *typeNameArg, int &num,
                         const int *&vals, const QvName *&names)
{
    QvName typeName = stripWhite(typeNameArg);

    for (int i=0; i<enums.getLength(); i++)
    {
        struct QvEnumEntry *e = (struct QvEnumEntry *) enums[i];
        if (e->typeName == typeName)
        {
            num         = e->num;
            vals        = e->vals;
            names       = e->names;
            return;
        }
    }
    num = 0;
    vals = NULL;
    names = NULL;
}

QvBool
QvFieldData::readFieldTypes(QvInput *in, QvNode *object)
{
    QvName fieldType, fieldName;
    char   c;

    if (! in->read(c) && c == OPEN_BRACE_CHAR)
        return FALSE;

    if (in->read(c) && c == CLOSE_BRACE_CHAR)
        return TRUE;

    in->putBack(c);

    QvBool alreadyHasFields = (object->isBuiltIn || getNumFields() != 0);

    while (TRUE) {

        if (! in->read(fieldType, TRUE) || ! in->read(fieldName, TRUE))
            return FALSE;

        if (! alreadyHasFields) {
            QvField *fld = QvField::createInstanceFromName(fieldType);
            fld->setContainer(object);
            addField(object, fieldName.getString(), fld);
        }

        if (! in->read(c))
            return FALSE;
        if (c == VALUE_SEPARATOR_CHAR) {

            if (in->read(c)) {
                if (c == CLOSE_BRACE_CHAR)
                    return TRUE;
                else
                    in->putBack(c);
            }
        }
        else if (c == CLOSE_BRACE_CHAR)
            return TRUE;
        else
            return FALSE;
    }
}

QvBool
QvFieldData::read(QvInput *in, QvNode *object,
                  QvBool errorOnUnknownField) const
{
    QvName fieldName;

    if (fields.getLength() == 0) return TRUE;

    while (TRUE) {

      if (! in->read(fieldName, TRUE) || ! fieldName)
        return TRUE;

      QvBool foundName;
      if (! read(in, object, fieldName, foundName))
        return FALSE;

      if (!foundName) {
        if (errorOnUnknownField) {
          //    QvReadError::post(in, "Unknown field \"%s\"",
          //                      fieldName.getString());

          // mpichler, 19950707: give warning on unknown fields, continue parsing
          QvReadError::post (in, "invalid field \"%s\" in context\n"
                             "(ignoring remaining data of current node)",
                             fieldName.getString () /*,object->objName->getString ()*/);

          // as we don't know the type of the wrong field we let
          // QvNode::readNode ignore the remaining data of the current node

          return FALSE;
        }
        else {
          in->putBack(fieldName.getString());
          return TRUE;
        }
      }
    }
}

QvBool
QvFieldData::read(QvInput *in, QvNode *object,
                  const QvName &fieldName, QvBool &foundName) const
{
    int i;
    for (i = 0; i < fields.getLength(); i++) {
        if (fieldName == getFieldName(i)) {
            if (! getField(object, i)->read(in, fieldName))
                return FALSE;
            break;
        }
    }
    if (i == fields.getLength())
        foundName = FALSE;
    else foundName = TRUE;

    return TRUE;
}
