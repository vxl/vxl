#ifndef  _QV_SF_ENUM_
#define  _QV_SF_ENUM_

#include "QvString.h"
#include "QvSubField.h"

class QvSFEnum : public QvSField
{
public:
  int value;
  QV_SFIELD_HEADER(QvSFEnum);

  // Sets up value/name correspondances
  void setEnums(int num, const int vals[], const QvName names[])
      { numEnums = num; enumValues = vals; enumNames = names; }

  int           numEnums;       // Number of enumeration values
  const int     *enumValues;    // Enumeration values
  const QvName  *enumNames;     // Mnemonic names of values

  // Looks up enum name, returns value. Returns FALSE if not found.
  QvBool        findEnumValue(const QvName &name, int &val) const;
};

#define QV_NODE_SET_SF_ENUM_TYPE(fieldName, enumType) \
  do {                                                \
    int _so_sf_enum_num;                              \
    const int *_so_sf_enum_vals;                      \
    const QvName *_so_sf_enum_names;                  \
    fieldData->getEnumData(QV__QUOTE(enumType),       \
                           _so_sf_enum_num,           \
                           _so_sf_enum_vals,          \
                           _so_sf_enum_names);        \
    fieldName.setEnums( _so_sf_enum_num,              \
                        _so_sf_enum_vals,             \
                        _so_sf_enum_names);           \
  } while (0)

#endif /* _QV_SF_ENUM_ */
