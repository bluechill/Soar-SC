#pragma once
#include <BWAPI/Type.h>

#define BWAPI_UNITSIZETYPEDEF(x) static const UnitSizeType x(Enum::x) /** ref x */

namespace BWAPI
{
  namespace UnitSizeTypes
  {
    namespace Enum
    {
      enum Enum
      {
        Independent = 0,
        Small,
        Medium,
        Large,
        None,
        Unknown,
        MAX
      };
    };
  };
  class UnitSizeType : public Type<UnitSizeType, UnitSizeTypes::Enum::Unknown>
  {
  public:
    UnitSizeType(int id = UnitSizeTypes::Enum::None);
  };
  namespace UnitSizeTypes
  {
    /** Returns the set of all the sizes, which are listed below: */
    const UnitSizeType::const_set& allUnitSizeTypes();
    
    extern const UnitSizeType Independent;
    extern const UnitSizeType Small;
    extern const UnitSizeType Medium;
    extern const UnitSizeType Large;
    extern const UnitSizeType None;
    extern const UnitSizeType Unknown;
  }
}
