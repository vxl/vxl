#include "QvField.h"
#include "QvInput.h"
#include "QvReadError.h"
#include "QvFields.h"
#include "QvDB.h"

// Special characters in files
#define OPEN_BRACE_CHAR         '['
#define CLOSE_BRACE_CHAR        ']'
#define VALUE_SEPARATOR_CHAR    ','
// mpichler, 19950707
#define IGNORE_CHAR             '~'

QvField::~QvField()
{
}

void
QvField::setContainer(QvNode *cont)
{
    container = cont;
    setDefault(TRUE);
}

const char* const IgnoreErrorMsg =  // mpichler, 19950712
  "Invalid value '~' for field \"%s\"\n"
  "(further occurrences will not be reported)";

QvBool
QvField::read(QvInput *in, const QvName &name) const
{
  char c;

  setDefault(FALSE); // mutable const

  // mpichler, 19950712 (re-activated from QvLib 1.0beta)
  if (in->read(c) && c == IGNORE_CHAR)
  {
    setDefault(TRUE); // mutable const
    if (QvDB::warn_ignorechar)
    { QvReadError::post (in, IgnoreErrorMsg, name.getString ());
      QvDB::warn_ignorechar = 0;
    }
  }
  else
  {
    in->putBack(c);

    if (! (const_cast<QvField*>(this))->readValue(in)) {
        QvReadError::post(in,
                          "Couldn't read value for field \"%s\"\n"
                          "(ignoring remaining data of current node)",
                          name.getString());
        return FALSE;
    }

    if (in->read(c))
    {
      if (c == IGNORE_CHAR)
      {
        // VRML has no ignore flag - use default value
        setDefault(TRUE); // mutable const
        if (QvDB::warn_ignorechar)
        { QvReadError::post (in, IgnoreErrorMsg, name.getString ());
          QvDB::warn_ignorechar = 0;
        }
      }
      else
        in->putBack(c);
    }
  }

    return TRUE;
}

QvField *
QvField::createInstanceFromName(const QvName &className)
{
#define TRY_CLASS(name, class)                                                \
    else if (className == name)                                               \
        inst = new class

    QvField *inst = NULL;

    if (0) ;                    // So "else" works in first TRY_CLASS

    TRY_CLASS("MFColor",        QvMFColor);
    TRY_CLASS("MFFloat",        QvMFFloat);
    TRY_CLASS("MFLong",         QvMFLong);
    TRY_CLASS("MFString",       QvMFString);
    TRY_CLASS("MFVec2f",        QvMFVec2f);
    TRY_CLASS("MFVec3f",        QvMFVec3f);
    TRY_CLASS("SFBitMask",      QvSFBitMask);
    TRY_CLASS("SFBool",         QvSFBool);
    TRY_CLASS("SFColor",        QvSFColor);
    TRY_CLASS("SFEnum",         QvSFEnum);
    TRY_CLASS("SFFloat",        QvSFFloat);
    TRY_CLASS("SFImage",        QvSFImage);
    TRY_CLASS("SFLong",         QvSFLong);
    TRY_CLASS("SFMatrix",       QvSFMatrix);
    TRY_CLASS("SFRotation",     QvSFRotation);
    TRY_CLASS("SFString",       QvSFString);
    TRY_CLASS("SFVec2f",        QvSFVec2f);
    TRY_CLASS("SFVec3f",        QvSFVec3f);

    return inst;

#undef TRY_CLASS
}

QvSField::QvSField()
{
}


QvSField::~QvSField()
{
}

QvMField::QvMField()
{
    maxNum = num = 0;
}

QvMField::~QvMField()
{
}

void
QvMField::makeRoom(int newNum)
{
    if (newNum != num)
        allocValues(newNum);
}

QvBool
QvMField::readValue(QvInput *in)
{
  char  c;
  int   curIndex = 0;

  // TODO: check whether enlarging array in groups speeds up parsing

  if (in->read(c) && c == OPEN_BRACE_CHAR) {

    if (in->read(c) && c != CLOSE_BRACE_CHAR)
    {
      in->putBack(c);

      while (true) {

        if (curIndex >= num)
          makeRoom(curIndex + 1);

        if (! read1Value(in, curIndex++) || ! in->read(c)) {
          QvReadError::post(in, "Couldn't read value %d of field",
                            curIndex);
          return FALSE;
        }

        if (c == VALUE_SEPARATOR_CHAR) {
          if (in->read(c)) {
            if (c == CLOSE_BRACE_CHAR)
              break;
            else
              in->putBack(c);
          }
        }

        else if (c == CLOSE_BRACE_CHAR)
          break;

        else {
          QvReadError::post(in,
                            "Expected '%c' or '%c' but got "
                            "'%c' while reading value %d",
                            VALUE_SEPARATOR_CHAR,
                            CLOSE_BRACE_CHAR, c,
                            curIndex);
          return FALSE;
        }
      }
    }

    if (curIndex < num)
        makeRoom(curIndex);
  }

  else // c != OPEN_BRACE_CHAR
  {
    in->putBack(c);
    makeRoom(1);
    if (! read1Value(in, 0))
      return FALSE;
  }

  return TRUE;
}
