#pragma once
#include <BWAPI/Type.h>

namespace BWAPI
{
  namespace PlayerTypes
  {
    namespace Enum
    {
      enum Enum
      {
        None = 0,
        Computer,
        Player,
        RescuePassive,
        RescueActive,
        EitherPreferComputer,
        EitherPreferHuman,
        Neutral,
        Closed,
        Observer,
        PlayerLeft,
        ComputerLeft,
        Unknown,
        MAX
      };
    };
  };
  class PlayerType : public Type<PlayerType, PlayerTypes::Enum::Unknown>
  {
    public:
      PlayerType(int id = PlayerTypes::Enum::None);

      bool isLobbyType() const;
      bool isGameType() const;
  };
  namespace PlayerTypes
  {
    /** Returns the set of all the PlayerTypes. */
    const PlayerType::const_set& allPlayerTypes();
    
    extern const PlayerType None;
    extern const PlayerType Computer;
    extern const PlayerType Player;
    extern const PlayerType RescuePassive;
    extern const PlayerType EitherPreferComputer;
    extern const PlayerType EitherPreferHuman;
    extern const PlayerType Neutral;
    extern const PlayerType Closed;
    extern const PlayerType PlayerLeft;
    extern const PlayerType ComputerLeft;
    extern const PlayerType Unknown;
  }
}
