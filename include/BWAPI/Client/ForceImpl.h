#pragma once
#include <BWAPI.h>
#include "ForceData.h"
#include <string>

namespace BWAPI
{
  class Game;
  class Player;
  class Unit;
  class ForceImpl : public Force
  {
    private:
      const ForceData* self;
      int id;
    public:
      ForceImpl(int id);
      virtual int getID() const;
      virtual std::string getName() const;
      virtual Playerset getPlayers() const;
  };
}