#ifndef  _QV_FIELD_DATA_
#define  _QV_FIELD_DATA_

#include "QvBasic.h"
#include "QvPList.h"
#include "QvString.h"

class QvField;
class QvInput;
class QvNode;

class QvFieldData {
  public:
    QvFieldData() {}
    ~QvFieldData();

    void           addField(QvNode *defObject, const char *fieldName,
                            const QvField *field);

    int            getNumFields() const { return fields.getLength(); }

    const QvName & getFieldName(int index) const;

    QvField const* getField(const QvNode *object, int index) const;

    void           addEnumValue(const char *typeName,
                                const char *valName, int val);
    void           getEnumData(const char *typeName, int &num,
                               const int *&vals, const QvName *&names);

    QvBool         read(QvInput *in, QvNode *object,
                        QvBool errorOnUnknownField = TRUE) const;

    QvBool         read(QvInput *in, QvNode *object,
                        const QvName &fieldName,
                        QvBool &foundName) const;

    QvBool         readFieldTypes(QvInput *in, QvNode *object);

  private:
    QvPList        fields;
    QvPList        enums;
};

#endif /* _QV_FIELD_DATA_ */
