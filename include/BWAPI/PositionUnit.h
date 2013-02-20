#pragma once
#include <BWAPI/Position.h>

namespace BWAPI
{
  class Unit;

  
  /// PositionOrUnit is a class that is either 
  /// a Position OR a Unit. The purpose of
  /// this class is to reduce interface
  /// overhead by combining two commonly
  /// passed values into one.
  
  class PositionOrUnit
  {
  public:
    // constructors
    PositionOrUnit(Unit *unit = nullptr);
    PositionOrUnit(Position pos);

    // operators
    PositionOrUnit &operator =(Unit *pUnit);
    PositionOrUnit &operator =(Position pos);

    // members (unit)
    bool isUnit() const;
    Unit *getUnit() const;

    // members (position)
    bool isPosition() const;
    Position getPosition() const;
  private:
    union
    {
      struct
      {
        int x, y;
      } position;
      struct
      {
        Unit *pUnit;
        int y;
      } unit;
    };
  };

}
