#ifndef  _QV_FIELD_
#define  _QV_FIELD_

#include "QvString.h"

class QvInput;
class QvNode;

class QvField {
public:
  virtual ~QvField();

  QvBool        isDefault() const       { return flags.hasDefault; }

  QvNode *      getContainer() const    { return container; }

  void          setDefault(QvBool def)  { flags.hasDefault = def; }
  void          setContainer(QvNode *cont);
  QvBool        read(QvInput *in, const QvName &name) const;

  QvField()     { flags.hasDefault = TRUE; }

private:
  struct {
    unsigned int hasDefault             : 1; // Field is set to default value
  }                     flags;

  QvNode                *container;

  static QvField *      createInstanceFromName(const QvName &className);
  virtual QvBool        readValue(QvInput *in) = 0;

friend class QvFieldData;
};

class QvSField : public QvField {
  public:
    virtual ~QvSField();

  protected:
    QvSField();

  private:
    virtual QvBool      readValue(QvInput *in) = 0;
};

class QvMField : public QvField {

  public:
    int                 num;            // Number of values
    int                 maxNum;         // Number of values allocated

    // Destructor
    virtual ~QvMField();

  protected:
    QvMField();
    virtual void        makeRoom(int newNum);

  private:
    virtual void        allocValues(int num) = 0;
    virtual QvBool      readValue(QvInput *in);
    virtual QvBool      read1Value(QvInput *in, int index) = 0;
};

#endif /* _QV_FIELD_ */
