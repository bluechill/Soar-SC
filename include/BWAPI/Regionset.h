#pragma once
#include <BWAPI/Vectorset.h>
#include <BWAPI/Position.h>
#include <BWAPI/Filters.h>

namespace BWAPI
{
  // Forward Declarations
  class Region;
  class Unitset;

  class Regionset : public Vectorset<BWAPI::Region*>
  {
  public:
    Regionset(size_t initialSize = 16);
    Regionset(const Regionset &other);
    Regionset(Regionset &&other);

    /// @copydoc Region::getCenter
    Position getCenter() const;

    /// @copydoc Region::getUnits
    Unitset getUnits(const UnitFilter &pred = nullptr) const;
  };
}

