#pragma once
#include <BWAPI/Position.h>

namespace BWAPI
{
  class BaseLocation : public TilePosition
  {
  public:
    BaseLocation(const TilePosition &location);

    bool isOccupied() const;
  };
}
