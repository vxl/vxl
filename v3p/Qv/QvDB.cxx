#include "QvDB.h"
#include "QvInput.h"
#include "QvReadError.h"
#include "QvNode.h"
#include "QvGroup.h"

const char *QvDB::versionString = "Reference VRML Parser 1.0";
int QvDB::warn_ignorechar = 1;

void
QvDB::init()
{
    QvNode::init();
}

QvBool
QvDB::read(QvInput *in, QvNode *&node)
{
  QvBool ret;

  // mpichler, 19950712: flags for warnings that will be given only once
  // (set to 0 to suppress warnings)
  warn_ignorechar = 1;

  // mpichler, 19950712: possibly multiple nodes in file
  // ret = QvNode::read(in, node);

  // create a root group (less overhead than a separator)
  QvGroup* root = new QvGroup;
  // as there are no valid fields on global stage (and Group does not have any anyway)
  // we call readChildren instead of readInstance
  ret = root->readChildren (in);

  // even when there was only a single node in the file (root->getNumChildren () == 1)
  // we cannot delete the root group (~QvGroup deletes its children)

  node = root;

  if (ret && node == NULL && ! in->eof()) {
      char c;
      in->get(c);
      QvReadError::post(in, "Extra characters ('%c') found in input", c);
      ret = FALSE;
  }

  return ret;
}
