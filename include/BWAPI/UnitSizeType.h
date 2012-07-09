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
    /** Returns the string corresponding to the UnitSizeType object. For example,
     * UnitSizeTypes::Medium.getName() returns std::string("Medium")*/
    const std::string &getName() const;
    const char *c_str() const;
  };
  namespace UnitSizeTypes
  {
    /** Given a string, this function returns the size type it refers to. For example,
     * UnitSizeTypes::getUnitSizeType("Small") returns UnitSizeTypes::Small. */
    UnitSizeType getUnitSizeType(std::string name);

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
