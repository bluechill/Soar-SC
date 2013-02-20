#pragma once
#include <BWAPI/Type.h>
#include <BWAPI/UnitType.h>

namespace BWAPI
{
  class WeaponType;
  class Order;
  class Race;

  namespace TechTypes
  {
    /// Enumeration of Tech Types
    namespace Enum
    {
      enum Enum
      {
        Stim_Packs = 0,
        Lockdown,
        EMP_Shockwave,
        Spider_Mines,
        Scanner_Sweep,
        Tank_Siege_Mode,
        Defensive_Matrix,
        Irradiate,
        Yamato_Gun,
        Cloaking_Field,
        Personnel_Cloaking,
        Burrowing,
        Infestation,
        Spawn_Broodlings,
        Dark_Swarm,
        Plague,
        Consume,
        Ensnare,
        Parasite,
        Psionic_Storm,
        Hallucination,
        Recall,
        Stasis_Field,
        Archon_Warp,
        Restoration,
        Disruption_Web,
        Unused_26,
        Mind_Control,
        Dark_Archon_Meld,
        Feedback,
        Optical_Flare,
        Maelstrom,
        Lurker_Aspect,
        Unused_33,
        Healing,

        None = 44,
        Nuclear_Strike,
        Unknown,
        MAX
      };
    };
  }
  class TechType : public Type<TechType, TechTypes::Enum::Unknown>
  {
    public:
      TechType(int id = TechTypes::Enum::None);

      /** Returns the race that uses the TechType. For example, TechTypes::Scanner_Sweep?.getRace() will
       * return Races::Terran. */
      Race getRace() const;

      /** Returns the mineral cost of the tech type. */
      int mineralPrice() const;

      /** Returns the vespene gas price of the tech type. */
      int gasPrice() const;

      /** Returns the number of frames needed to research the tech type. */
      int researchTime() const;

      /** Returns the amount of energy used each time this tech type is used. */
      int energyCost() const;

      /** Returns the type of unit that researches this tech type. If this tech type is available for free
       * (does not need to be researched), then this method will return UnitTypes::None. */
      UnitType whatResearches() const;

      /** Returns the corresponding weapon for this tech type, or TechTypes::None if no corresponding weapon
       * exists. For example, TechTypes::Dark_Swarm.getWeapon() will return a pointer to
       * WeaponTypes::Dark_Swarm. */
      WeaponType getWeapon() const;

      /** Returns true if this tech type must be used on another unit (i.e. Irradiate) */
      bool targetsUnit() const;

      /** Returns true if this tech type must be specified a position (i.e. Dark Swarm) */
      bool targetsPosition() const;

      /** Returns the set of units that can use this tech type. Usually this will just be a set of one unit
       * type, however in some cases, such as TechTypes::Burrowing, several unit types will be returned. */
      const UnitType::const_set& whatUses() const;

      /** Returns the order used to execute this tech type as an action. */
      Order getOrder() const;
  };
  namespace TechTypes
  {
    /** Returns the set of all the TechTypes. */
    const TechType::const_set& allTechTypes();

    extern const TechType Stim_Packs;
    extern const TechType Lockdown;
    extern const TechType EMP_Shockwave;
    extern const TechType Spider_Mines;
    extern const TechType Scanner_Sweep;
    extern const TechType Tank_Siege_Mode;
    extern const TechType Defensive_Matrix;
    extern const TechType Irradiate;
    extern const TechType Yamato_Gun;
    extern const TechType Cloaking_Field;
    extern const TechType Personnel_Cloaking;
    extern const TechType Burrowing;
    extern const TechType Infestation;
    extern const TechType Spawn_Broodlings;
    extern const TechType Dark_Swarm;
    extern const TechType Plague;
    extern const TechType Consume;
    extern const TechType Ensnare;
    extern const TechType Parasite;
    extern const TechType Psionic_Storm;
    extern const TechType Hallucination;
    extern const TechType Recall;
    extern const TechType Stasis_Field;
    extern const TechType Archon_Warp;
    extern const TechType Restoration;
    extern const TechType Disruption_Web;
    extern const TechType Mind_Control;
    extern const TechType Dark_Archon_Meld;
    extern const TechType Feedback;
    extern const TechType Optical_Flare;
    extern const TechType Maelstrom;
    extern const TechType Lurker_Aspect;
    extern const TechType Healing;
    extern const TechType None;
    extern const TechType Nuclear_Strike;
    extern const TechType Unknown;
  };
}
