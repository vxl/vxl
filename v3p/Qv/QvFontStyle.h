#ifndef  _QV_FONT_STYLE_
#define  _QV_FONT_STYLE_

#include "QvSFBitMask.h"
#include "QvSFEnum.h"
#include "QvSFFloat.h"
#include "QvSubNode.h"

class QvFontStyle : public QvNode
{
  QV_NODE_HEADER(QvFontStyle);

public:

  enum Family {         // Font family:
    SERIF,                      // Serif style (such as TimesRoman)
    SANS,                       // Sans-serif style (such as Helvetica)
    TYPEWRITER                  // Fixed pitch style (such as Courier)
  };

  enum Style {          // Font style modifications:
    NONE        = 0x00,         // No modifications to family
    BOLD        = 0x01,         // Embolden family
    ITALIC      = 0x02          // Italicize or slant family
  };

  // Fields
  QvSFFloat     size;           // Font size
  QvSFEnum      family;         // Font family
  QvSFBitMask   style;          // Font style modifications to family
};

#endif // _QV_FONT_STYLE_
