#pragma once
#include <BWAPI/Type.h>

namespace BWAPI
{
  namespace UnitCommandTypes
  {
    namespace Enum
    {
      enum Enum
      {
        Attack_Move = 0,
        Attack_Unit,
        Build,
        Build_Addon,
        Train,
        Morph,
        Research,
        Upgrade,
        Set_Rally_Position,
        Set_Rally_Unit,
        Move,
        Patrol,
        Hold_Position,
        Stop,
        Follow,
        Gather,
        Return_Cargo,
        Repair,
        Burrow,
        Unburrow,
        Cloak,
        Decloak,
        Siege,
        Unsiege,
        Lift,
        Land,
        Load,
        Unload,
        Unload_All,
        Unload_All_Position,
        Right_Click_Position,
        Right_Click_Unit,
        Halt_Construction,
        Cancel_Construction,
        Cancel_Addon,
        Cancel_Train,
        Cancel_Train_Slot,
        Cancel_Morph,
        Cancel_Research,
        Cancel_Upgrade,
        Use_Tech,
        Use_Tech_Position,
        Use_Tech_Unit,
        Place_COP,
        None,
        Unknown,
        MAX
      };
    };
  };
  class UnitCommandType : public Type<UnitCommandType, UnitCommandTypes::Enum::Unknown>
  {
  public:
    UnitCommandType(int id = UnitCommandTypes::Enum::None);
    /** Returns the string corresponding to the UnitCommandType object. For example,
     * UnitCommandTypes::Set_Rally_Position.getName() returns std::string("Set Rally Position")*/
    const std::string &getName() const;
    const char *c_str() const;
  };
  namespace UnitCommandTypes
  {
    /** Given a string, this function returns the command type it refers to. For example,
     * UnitCommandTypes::getUnitCommandType("Attack Position") returns UnitCommandTypes::Attack_Position. */
    UnitCommandType getUnitCommandType(std::string name);

    /** Returns the set of all the sizes, which are listed below: */
    const UnitCommandType::const_set& allUnitCommandTypes();
    
    extern const UnitCommandType Attack_Move;
    extern const UnitCommandType Attack_Unit;
    extern const UnitCommandType Build;
    extern const UnitCommandType Build_Addon;
    extern const UnitCommandType Train;
    extern const UnitCommandType Morph;
    extern const UnitCommandType Research;
    extern const UnitCommandType Upgrade;
    extern const UnitCommandType Set_Rally_Position;
    extern const UnitCommandType Set_Rally_Unit;
    extern const UnitCommandType Move;
    extern const UnitCommandType Patrol;
    extern const UnitCommandType Hold_Position;
    extern const UnitCommandType Stop;
    extern const UnitCommandType Follow;
    extern const UnitCommandType Gather;
    extern const UnitCommandType Return_Cargo;
    extern const UnitCommandType Repair;
    extern const UnitCommandType Burrow;
    extern const UnitCommandType Unburrow;
    extern const UnitCommandType Cloak;
    extern const UnitCommandType Decloak;
    extern const UnitCommandType Siege;
    extern const UnitCommandType Unsiege;
    extern const UnitCommandType Lift;
    extern const UnitCommandType Land;
    extern const UnitCommandType Load;
    extern const UnitCommandType Unload;
    extern const UnitCommandType Unload_All;
    extern const UnitCommandType Unload_All_Position;
    extern const UnitCommandType Right_Click_Position;
    extern const UnitCommandType Right_Click_Unit;
    extern const UnitCommandType Halt_Construction;
    extern const UnitCommandType Cancel_Construction;
    extern const UnitCommandType Cancel_Addon;
    extern const UnitCommandType Cancel_Train;
    extern const UnitCommandType Cancel_Train_Slot;
    extern const UnitCommandType Cancel_Morph;
    extern const UnitCommandType Cancel_Research;
    extern const UnitCommandType Cancel_Upgrade;
    extern const UnitCommandType Use_Tech;
    extern const UnitCommandType Use_Tech_Position;
    extern const UnitCommandType Use_Tech_Unit;
    extern const UnitCommandType Place_COP;
    extern const UnitCommandType None;
    extern const UnitCommandType Unknown;
  }
}
