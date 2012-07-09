#pragma once
#include <map>

#include <BWAPI/Position.h>
#include <BWAPI/Type.h>

namespace BWAPI
{
  // Forward Declarations
  class TechType;
  class UpgradeType;
  class WeaponType;
  class UnitSizeType;
  class Race;

  namespace UnitTypes
  {

    /// Enumeration of unit types
    namespace Enum
    {
      enum Enum
      {
        Terran_Marine = 0,
        Terran_Ghost,
        Terran_Vulture,
        Terran_Goliath,
        Terran_Goliath_Turret,
        Terran_Siege_Tank_Tank_Mode,
        Terran_Siege_Tank_Tank_Mode_Turret,
        Terran_SCV,
        Terran_Wraith,
        Terran_Science_Vessel,
        Hero_Gui_Montag,
        Terran_Dropship,
        Terran_Battlecruiser,
        Terran_Vulture_Spider_Mine,
        Terran_Nuclear_Missile,
        Terran_Civilian,
        Hero_Sarah_Kerrigan,
        Hero_Alan_Schezar,
        Hero_Alan_Schezar_Turret,
        Hero_Jim_Raynor_Vulture,
        Hero_Jim_Raynor_Marine,
        Hero_Tom_Kazansky,
        Hero_Magellan,
        Hero_Edmund_Duke_Tank_Mode,
        Hero_Edmund_Duke_Tank_Mode_Turret,
        Hero_Edmund_Duke_Siege_Mode,
        Hero_Edmund_Duke_Siege_Mode_Turret,
        Hero_Arcturus_Mengsk,
        Hero_Hyperion,
        Hero_Norad_II,
        Terran_Siege_Tank_Siege_Mode,
        Terran_Siege_Tank_Siege_Mode_Turret,
        Terran_Firebat,
        Spell_Scanner_Sweep,
        Terran_Medic,
        Zerg_Larva,
        Zerg_Egg,
        Zerg_Zergling,
        Zerg_Hydralisk,
        Zerg_Ultralisk,
        Zerg_Broodling,
        Zerg_Drone,
        Zerg_Overlord,
        Zerg_Mutalisk,
        Zerg_Guardian,
        Zerg_Queen,
        Zerg_Defiler,
        Zerg_Scourge,
        Hero_Torrasque,
        Hero_Matriarch,
        Zerg_Infested_Terran,
        Hero_Infested_Kerrigan,
        Hero_Unclean_One,
        Hero_Hunter_Killer,
        Hero_Devouring_One,
        Hero_Kukulza_Mutalisk,
        Hero_Kukulza_Guardian,
        Hero_Yggdrasill,
        Terran_Valkyrie,
        Zerg_Cocoon,
        Protoss_Corsair,
        Protoss_Dark_Templar,
        Zerg_Devourer,
        Protoss_Dark_Archon,
        Protoss_Probe,
        Protoss_Zealot,
        Protoss_Dragoon,
        Protoss_High_Templar,
        Protoss_Archon,
        Protoss_Shuttle,
        Protoss_Scout,
        Protoss_Arbiter,
        Protoss_Carrier,
        Protoss_Interceptor,
        Hero_Dark_Templar,
        Hero_Zeratul,
        Hero_Tassadar_Zeratul_Archon,
        Hero_Fenix_Zealot,
        Hero_Fenix_Dragoon,
        Hero_Tassadar,
        Hero_Mojo,
        Hero_Warbringer,
        Hero_Gantrithor,
        Protoss_Reaver,
        Protoss_Observer,
        Protoss_Scarab,
        Hero_Danimoth,
        Hero_Aldaris,
        Hero_Artanis,
        Critter_Rhynadon,
        Critter_Bengalaas,
        Special_Cargo_Ship,
        Special_Mercenary_Gunship,
        Critter_Scantid,
        Critter_Kakaru,
        Critter_Ragnasaur,
        Critter_Ursadon,
        Zerg_Lurker_Egg,
        Hero_Raszagal,
        Hero_Samir_Duran,
        Hero_Alexei_Stukov,
        Special_Map_Revealer,
        Hero_Gerard_DuGalle,
        Zerg_Lurker,
        Hero_Infested_Duran,
        Spell_Disruption_Web,
        Terran_Command_Center,
        Terran_Comsat_Station,
        Terran_Nuclear_Silo,
        Terran_Supply_Depot,
        Terran_Refinery,
        Terran_Barracks,
        Terran_Academy,
        Terran_Factory,
        Terran_Starport,
        Terran_Control_Tower,
        Terran_Science_Facility,
        Terran_Covert_Ops,
        Terran_Physics_Lab,
        Unused_Terran1,
        Terran_Machine_Shop,
        Unused_Terran2,
        Terran_Engineering_Bay,
        Terran_Armory,
        Terran_Missile_Turret,
        Terran_Bunker,
        Special_Crashed_Norad_II,
        Special_Ion_Cannon,
        Powerup_Uraj_Crystal,
        Powerup_Khalis_Crystal,
        Zerg_Infested_Command_Center,
        Zerg_Hatchery,
        Zerg_Lair,
        Zerg_Hive,
        Zerg_Nydus_Canal,
        Zerg_Hydralisk_Den,
        Zerg_Defiler_Mound,
        Zerg_Greater_Spire,
        Zerg_Queens_Nest,
        Zerg_Evolution_Chamber,
        Zerg_Ultralisk_Cavern,
        Zerg_Spire,
        Zerg_Spawning_Pool,
        Zerg_Creep_Colony,
        Zerg_Spore_Colony,
        Unused_Zerg1,
        Zerg_Sunken_Colony,
        Special_Overmind_With_Shell,
        Special_Overmind,
        Zerg_Extractor,
        Special_Mature_Chrysalis,
        Special_Cerebrate,
        Special_Cerebrate_Daggoth,
        Unused_Zerg2,
        Protoss_Nexus,
        Protoss_Robotics_Facility,
        Protoss_Pylon,
        Protoss_Assimilator,
        Unused_Protoss1,
        Protoss_Observatory,
        Protoss_Gateway,
        Unused_Protoss2,
        Protoss_Photon_Cannon,
        Protoss_Citadel_of_Adun,
        Protoss_Cybernetics_Core,
        Protoss_Templar_Archives,
        Protoss_Forge,
        Protoss_Stargate,
        Special_Stasis_Cell_Prison,
        Protoss_Fleet_Beacon,
        Protoss_Arbiter_Tribunal,
        Protoss_Robotics_Support_Bay,
        Protoss_Shield_Battery,
        Special_Khaydarin_Crystal_Form,
        Special_Protoss_Temple,
        Special_XelNaga_Temple,
        Resource_Mineral_Field,
        Resource_Mineral_Field_Type_2,
        Resource_Mineral_Field_Type_3,
        Unused_Cave,
        Unused_Cave_In,
        Unused_Cantina,
        Unused_Mining_Platform,
        Unused_Independant_Command_Center,
        Special_Independant_Starport,
        Unused_Independant_Jump_Gate,
        Unused_Ruins,
        Unused_Khaydarin_Crystal_Formation,
        Resource_Vespene_Geyser,
        Special_Warp_Gate,
        Special_Psi_Disrupter,
        Unused_Zerg_Marker,
        Unused_Terran_Marker,
        Unused_Protoss_Marker,
        Special_Zerg_Beacon,
        Special_Terran_Beacon,
        Special_Protoss_Beacon,
        Special_Zerg_Flag_Beacon,
        Special_Terran_Flag_Beacon,
        Special_Protoss_Flag_Beacon,
        Special_Power_Generator,
        Special_Overmind_Cocoon,
        Spell_Dark_Swarm,
        Special_Floor_Missile_Trap,
        Special_Floor_Hatch,
        Special_Upper_Level_Door,
        Special_Right_Upper_Level_Door,
        Special_Pit_Door,
        Special_Right_Pit_Door,
        Special_Floor_Gun_Trap,
        Special_Wall_Missile_Trap,
        Special_Wall_Flame_Trap,
        Special_Right_Wall_Missile_Trap,
        Special_Right_Wall_Flame_Trap,
        Special_Start_Location,
        Powerup_Flag,
        Powerup_Young_Chrysalis,
        Powerup_Psi_Emitter,
        Powerup_Data_Disk,
        Powerup_Khaydarin_Crystal,
        Powerup_Mineral_Cluster_Type_1,
        Powerup_Mineral_Cluster_Type_2,
        Powerup_Protoss_Gas_Orb_Type_1,
        Powerup_Protoss_Gas_Orb_Type_2,
        Powerup_Zerg_Gas_Sac_Type_1,
        Powerup_Zerg_Gas_Sac_Type_2,
        Powerup_Terran_Gas_Tank_Type_1,
        Powerup_Terran_Gas_Tank_Type_2,

        None,
        AllUnits,
        Men,
        Buildings,
        Factories,
        Unknown,
        MAX
      };
    };
  }
  /** The UnitType class is used to get information about a particular type of unit, such as the build time
   * of a Lurker, or the mineral price of an Ultralisk. TODO Add the unittype table from the wiki*/
  class UnitType : public Type<UnitType, UnitTypes::Enum::Unknown>
  {
  public:
    UnitType(int id = UnitTypes::Enum::None);

    /** Returns the name of the unit. */
    const std::string &getName() const;
    const char *c_str() const;

    /** Returns the race that the unit belongs to. For example UnitTypes::Terran_SCV.getRace() will return
     * Races::Terran. */
    Race getRace() const;

    /** Returns what builds this unit type. The second number will usually be 1 unless the unit type is
     * Protoss_Archon or Protoss_Dark_Archon. Units that cannot be created, such as critters and mineral
     * fields, will return a pair where the unit type is UnitTypes::None, and the second component is 0.
     *
     * Example: UnitTypes::Terran_Marine.whatBuilds() will return an std::pair, where the first component
     * is UnitTypes::Terran_Barracks. */
    const std::pair< UnitType, int > whatBuilds() const;

    /** Returns the units the player is required to have before it can train or build the given unit type.
     *
     * Example: UnitTypes::Terran_Battlecruiser.requiredUnits() will return a map of three keys:
     * UnitTypes::Terran_Starport, UnitTypes::Terran_Control_Tower, and UnitTypes::Terran_Physics_Lab. */
    const std::map< UnitType, int >& requiredUnits() const;

    /** Included in the API for completeness, since the only units that actually needs tech to be trained
     * are the Zerg_Lurker and Zerg_Lurker_Egg. The tech type needed is TechTypes::Lurker_Aspect. */
    TechType requiredTech() const;

    /** Returns the tech used to cloak the unit, or TechTypes::None if the unit cannot cloak or is
        permanently cloaked */
    TechType cloakingTech() const;

    /** Returns the set of tech types this unit can use, provided the tech types have been researched and
     * the unit has enough energy. */
    const Typeset<TechType>& abilities() const;

    /** Returns the set of upgrade types that can affect this unit. */
    const Typeset<UpgradeType>& upgrades() const;

    /** Returns the upgrade that increase's the unit's armor, or UpgradeTypes::None if no upgrade
     * increase's this unit's armor. For example UnitTypes::Terran_Marine.armorUpgrade() will return a
     * pointer to UpgradeTypes::Terran_Infantry_Armor. */
    UpgradeType armorUpgrade() const;

    /** Returns the maximum amount of hit points the unit type can have. */
    int maxHitPoints() const;

    /** Returns the maximum amount of shields the unit type can have. */
    int maxShields() const;

    /** Returns the maximum amount of energy the unit type can have. */
    int maxEnergy() const;

    /** Returns the amount of armor the non-upgraded unit type has. */
    int armor() const;

    /** Returns the mineral price of the unit.
     *
     * Example: UnitTypes::Siege_Tank_Tank_Mode.mineralPrice() returns 150. */
    int mineralPrice() const;

    /** UnitTypes::Siege_Tank_Tank_Mode.gasPrice() returns 100. */
    int gasPrice() const;

    /** Returns the number of frames needed to make this unit type. */
    int buildTime() const;

    /** Returns the amount of supply used by this unit. Supply counts returned by BWAPI are double what you
     *  would expect to see from playing the game. This is because zerglings take up 0.5 in-game supply. */
    int supplyRequired() const;

    /** Returns the amount of supply produced by this unit (i.e. for a Protoss_Pylon). Supply counts
     * returned by BWAPI are double what you would expect to see from playing the game. This is because
     * zerglings take up 0.5 in-game supply. */
    int supplyProvided() const;

    /** Returns the amount of space this unit type takes up inside a bunker or transport unit. */
    int spaceRequired() const;

    /** Returns the amount of space this unit type provides. */
    int spaceProvided() const;

    /** Returns the score which is used to determine the total scores in the after-game stats screen. */
    int buildScore() const;

    /** Returns the score which is used to determine the total scores in the after-game stats screen. */
    int destroyScore() const;

    /** Returns the size of the unit - either Small, Medium, Large, or Independent. */
    UnitSizeType size() const;

    /** Returns the tile width of the unit. Useful for determining the size of buildings. For example
     * UnitTypes::Terran_Supply_Depot.tileWidth() will return 3. */
    int tileWidth() const;

    /** Returns the tile height of the unit. Useful for determining the size of buildings. For example
     * UnitTypes::Terran_Supply_Depot.tileHeight() will return 2. */
    int tileHeight() const;

    TilePosition tileSize() const;

    /** Distance from the center of the unit to the left edge of the unit, measured in pixels. */
    int dimensionLeft() const;

    /** Distance from the center of the unit to the top edge of the unit, measured in pixels. */
    int dimensionUp() const;

    /** Distance from the center of the unit to the right edge of the unit, measured in pixels. */
    int dimensionRight() const;

    /** Distance from the center of the unit to the bottom edge of the unit, measured in pixels. */
    int dimensionDown() const;

    /** Returns the range at which the unit will start targeting enemy units, measured in pixels. */
    int seekRange() const;

    /** Returns how far the un-upgraded unit type can see into the fog of war, measured in pixels. */
    int sightRange() const;

    /** Returns the unit's ground weapon. */
    WeaponType groundWeapon() const;

    // TODO: add doc
    int maxGroundHits() const;

    /** Returns the unit's air weapon. */
    WeaponType airWeapon() const;

    // TODO: add doc
    int maxAirHits() const;

    /** Returns the unit's non-upgraded top speed in pixels per frame. For Terran buildings that can lift
     * off and the Zerg Infested Command Center, this returns how fast the building moves when it is
     * lifted. */
    double topSpeed() const;

    /** Returns how fast the unit can accelerate to its top speed. What units this quantity is measured in
     * is currently unknown. */
    int acceleration() const;

    /** Related to how fast the unit can halt. What units this quantity is measured in is currently
     * unknown. */
    int haltDistance() const;

    /** Related to how fast the unit can turn. What units this quantity is measured in is currently
     * unknown. */
    int turnRadius() const;

    /** Returns true if the unit can train other units. For example, UnitTypes::Terran_Barracks.canProduce()
     * will return true, while UnitTypes::Terran_Marine?.canProduce() will return false. This is also true
     * for two non-building units: Protoss Carrier (can produce interceptors) and Protoss Reaver
     * (can produce scarabs). */
    bool canProduce() const;

    /** Returns true if the unit can attack (either ground or air). Returns false for units that can only
     * inflict damage via special abilities (such as Protoss High Templar). */
    bool canAttack() const;

    /** Returns true if the unit can move. Note that buildings will return false, even Terran buildings
     * which can move once lifted. */
    bool canMove() const;

    /** Returns true for flying/air units. */
    bool isFlyer() const;

    /** Returns true for units that regenerate health (i.e. zerg units). */
    bool regeneratesHP() const;

    /** Returns true if the unit type is capable of casting spells / using technology. */
    bool isSpellcaster() const;

    /** Returns true for the two units that are permanently cloaked - Protoss Observer and Protoss Dark
     * Templar. */
    bool hasPermanentCloak() const;

    /** Returns true for units that cannot be destroyed (i.e. Terran Nuclear Missile, Mineral Field,
     * Vespene Geyser, etc) */
    bool isInvincible() const;

    /** Returns true if the unit is organic, such as a Terran Marine. */
    bool isOrganic() const;

    /** Returns true if the unit is mechanical such as a Terran Vulture. */
    bool isMechanical() const;

    /** Returns true for the four robotic Protoss units - Probe, Shuttle, Reaver, and Observer. */
    bool isRobotic() const;

    /** Returns true for the seven units that can detect cloaked units - Terran Science Vessel, Spell
     * Scanner Sweep, Zerg Overlord, Protoss Observer, Terran Missile Turret, Zerg Spore Colony, and Protoss
     * Photon Cannon. */
    bool isDetector() const;

    /** Returns true for the five units that hold resources - Mineral Field, Vespene Geyser,
     * Terran Refinery, Zerg Extractor, and Protoss Assimilator. */
    bool isResourceContainer() const;

    /** Returns true for the five units that can accept resources - Terran Command Center, Protoss Nexus,
     * Zerg Hatchery, Zerg Lair, and Zerg Hive. */
    bool isResourceDepot() const;

    /** Returns true for Terran Refinery, Zerg Extractor, and Protoss Assimilator. */
    bool isRefinery() const;

    /** Returns true for Protoss Probe, Terran SCV, and Zerg Drone. */
    bool isWorker() const;

    /** Returns true for buildings that must be near a pylon to be constructed. */
    bool requiresPsi() const;

    /** Returns true for buildings that can only be built on zerg creep. */
    bool requiresCreep() const;

    /** Returns true for Zergling and Scourge. */
    bool isTwoUnitsInOneEgg() const;

    /** Returns true for Zerg Lurker and units that can burrow when burrow tech is researched. */
    bool isBurrowable() const;

    /** Returns true for units that can be cloaked - Terran Ghost and Terran Wraith. Does not include units
     * which have permanent cloak (Protoss Observer and Protoss Dark Templar). */
    bool isCloakable() const;

    /** Returns true if the unit is a building (also true for mineral field and vespene geyser). */
    bool isBuilding() const;

    /** Returns true if the unit is an add-on, such as a Terran Comsat Station. */
    bool isAddon() const;

    /** Returns true for Terran buildings that can lift off (i.e. Barracks). */
    bool isFlyingBuilding() const;

    /** Returns true if the unit is neutral, such as a critter or mineral field. */
    bool isNeutral() const;

    /** Returns true if the unit is a Hero unit. */
    bool isHero() const;

    /** Returns true if the unit is a Powerup unit. */
    bool isPowerup() const;

    /** Returns true if the unit is a regular Beacon. */
    bool isBeacon() const;

    /** Returns true if the unit is a flag Beacon. */
    bool isFlagBeacon() const;

    /** Returns true if the unit is a special building. */
    bool isSpecialBuilding() const;

    /** Returns true if the unit is a spell unit. */
    bool isSpell() const;

    /** Returns true if the unit produces larva. */
    bool producesLarva() const;

    /** Returns true if the unit is one of the three mineral field types. */
    bool isMineralField() const;

    /** Returns true if the unit is capable of constructing an addon.
        This consists of Command Center, Factory, Starport, and Science Facility. */
    bool canBuildAddon() const;
  };

  /// Namespace containing unit types
  namespace UnitTypes
  {
    /** Given the name of a unit type, this function will return the unit type.
     * For example, UnitTypes::getUnitType("Terran Marine") will return UnitTypes::Terran_Marine. */
    UnitType getUnitType(std::string name);

    /// max unit width
    int maxUnitWidth();
    
    /// max unit height
    int maxUnitHeight();

    /** Returns the set of all the UnitTypes. */
    const UnitType::set& allUnitTypes();

    /** Returns the set of all the MacroTypes. */
    const UnitType::set& allMacroTypes();
    void init();

    extern const UnitType Terran_Marine;
    extern const UnitType Terran_Ghost;
    extern const UnitType Terran_Vulture;
    extern const UnitType Terran_Goliath;
    // goliath turret 4
    extern const UnitType Terran_Siege_Tank_Tank_Mode;
    // siege tank turret 6
    extern const UnitType Terran_SCV;
    extern const UnitType Terran_Wraith;
    extern const UnitType Terran_Science_Vessel;
    extern const UnitType Hero_Gui_Montag;
    extern const UnitType Terran_Dropship;
    extern const UnitType Terran_Battlecruiser;
    extern const UnitType Terran_Vulture_Spider_Mine;
    extern const UnitType Terran_Nuclear_Missile;
    extern const UnitType Terran_Civilian;
    extern const UnitType Hero_Sarah_Kerrigan;
    extern const UnitType Hero_Alan_Schezar;
    // alan turret 18
    extern const UnitType Hero_Jim_Raynor_Vulture;
    extern const UnitType Hero_Jim_Raynor_Marine;
    extern const UnitType Hero_Tom_Kazansky;
    extern const UnitType Hero_Magellan;
    extern const UnitType Hero_Edmund_Duke_Tank_Mode;
    // edmund duke turret 24
    extern const UnitType Hero_Edmund_Duke_Siege_Mode;
    // edmund duke turret siege mode 26
    extern const UnitType Hero_Arcturus_Mengsk;
    extern const UnitType Hero_Hyperion;
    extern const UnitType Hero_Norad_II;
    extern const UnitType Terran_Siege_Tank_Siege_Mode;
    // siege tank siege mode turret 31
    extern const UnitType Terran_Firebat;
    extern const UnitType Spell_Scanner_Sweep;
    extern const UnitType Terran_Medic;
    extern const UnitType Zerg_Larva;
    extern const UnitType Zerg_Egg;
    extern const UnitType Zerg_Zergling;
    extern const UnitType Zerg_Hydralisk;
    extern const UnitType Zerg_Ultralisk;
    extern const UnitType Zerg_Broodling;
    extern const UnitType Zerg_Drone;
    extern const UnitType Zerg_Overlord;
    extern const UnitType Zerg_Mutalisk;
    extern const UnitType Zerg_Guardian;
    extern const UnitType Zerg_Queen;
    extern const UnitType Zerg_Defiler;
    extern const UnitType Zerg_Scourge;
    extern const UnitType Hero_Torrasque;
    extern const UnitType Hero_Matriarch;
    extern const UnitType Zerg_Infested_Terran;
    extern const UnitType Hero_Infested_Kerrigan;
    extern const UnitType Hero_Unclean_One;
    extern const UnitType Hero_Hunter_Killer;
    extern const UnitType Hero_Devouring_One;
    extern const UnitType Hero_Kukulza_Mutalisk;
    extern const UnitType Hero_Kukulza_Guardian;
    extern const UnitType Hero_Yggdrasill;
    extern const UnitType Terran_Valkyrie;
    extern const UnitType Zerg_Cocoon;
    extern const UnitType Protoss_Corsair;
    extern const UnitType Protoss_Dark_Templar;
    extern const UnitType Zerg_Devourer;
    extern const UnitType Protoss_Dark_Archon;
    extern const UnitType Protoss_Probe;
    extern const UnitType Protoss_Zealot;
    extern const UnitType Protoss_Dragoon;
    extern const UnitType Protoss_High_Templar;
    extern const UnitType Protoss_Archon;
    extern const UnitType Protoss_Shuttle;
    extern const UnitType Protoss_Scout;
    extern const UnitType Protoss_Arbiter;
    extern const UnitType Protoss_Carrier;
    extern const UnitType Protoss_Interceptor;
    extern const UnitType Hero_Dark_Templar;
    extern const UnitType Hero_Zeratul;
    extern const UnitType Hero_Tassadar_Zeratul_Archon;
    extern const UnitType Hero_Fenix_Zealot;
    extern const UnitType Hero_Fenix_Dragoon;
    extern const UnitType Hero_Tassadar;
    extern const UnitType Hero_Mojo;
    extern const UnitType Hero_Warbringer;
    extern const UnitType Hero_Gantrithor;
    extern const UnitType Protoss_Reaver;
    extern const UnitType Protoss_Observer;
    extern const UnitType Protoss_Scarab;
    extern const UnitType Hero_Danimoth;
    extern const UnitType Hero_Aldaris;
    extern const UnitType Hero_Artanis;
    extern const UnitType Critter_Rhynadon;
    extern const UnitType Critter_Bengalaas;
    extern const UnitType Special_Cargo_Ship;
    extern const UnitType Special_Mercenary_Gunship;
    extern const UnitType Critter_Scantid;
    extern const UnitType Critter_Kakaru;
    extern const UnitType Critter_Ragnasaur;
    extern const UnitType Critter_Ursadon;
    extern const UnitType Zerg_Lurker_Egg;
    extern const UnitType Hero_Raszagal;
    extern const UnitType Hero_Samir_Duran;
    extern const UnitType Hero_Alexei_Stukov;
    extern const UnitType Special_Map_Revealer;
    extern const UnitType Hero_Gerard_DuGalle;
    extern const UnitType Zerg_Lurker;
    extern const UnitType Hero_Infested_Duran;
    extern const UnitType Spell_Disruption_Web;
    extern const UnitType Terran_Command_Center;
    extern const UnitType Terran_Comsat_Station;
    extern const UnitType Terran_Nuclear_Silo;
    extern const UnitType Terran_Supply_Depot;
    extern const UnitType Terran_Refinery;
    extern const UnitType Terran_Barracks;
    extern const UnitType Terran_Academy;
    extern const UnitType Terran_Factory;
    extern const UnitType Terran_Starport;
    extern const UnitType Terran_Control_Tower;
    extern const UnitType Terran_Science_Facility;
    extern const UnitType Terran_Covert_Ops;
    extern const UnitType Terran_Physics_Lab;
    // starbase 119
    extern const UnitType Terran_Machine_Shop;
    // repair bay 121
    extern const UnitType Terran_Engineering_Bay;
    extern const UnitType Terran_Armory;
    extern const UnitType Terran_Missile_Turret;
    extern const UnitType Terran_Bunker;
    extern const UnitType Special_Crashed_Norad_II;
    extern const UnitType Special_Ion_Cannon;
    extern const UnitType Powerup_Uraj_Crystal;
    extern const UnitType Powerup_Khalis_Crystal;
    extern const UnitType Zerg_Infested_Command_Center;
    extern const UnitType Zerg_Hatchery;
    extern const UnitType Zerg_Lair;
    extern const UnitType Zerg_Hive;
    extern const UnitType Zerg_Nydus_Canal;
    extern const UnitType Zerg_Hydralisk_Den;
    extern const UnitType Zerg_Defiler_Mound;
    extern const UnitType Zerg_Greater_Spire;
    extern const UnitType Zerg_Queens_Nest;
    extern const UnitType Zerg_Evolution_Chamber;
    extern const UnitType Zerg_Ultralisk_Cavern;
    extern const UnitType Zerg_Spire;
    extern const UnitType Zerg_Spawning_Pool;
    extern const UnitType Zerg_Creep_Colony;
    extern const UnitType Zerg_Spore_Colony;
    // unused zerg 1 145
    extern const UnitType Zerg_Sunken_Colony;
    extern const UnitType Special_Overmind_With_Shell;
    extern const UnitType Special_Overmind;
    extern const UnitType Zerg_Extractor;
    extern const UnitType Special_Mature_Chrysalis;
    extern const UnitType Special_Cerebrate;
    extern const UnitType Special_Cerebrate_Daggoth;
    // unused zerg 2 153
    extern const UnitType Protoss_Nexus;
    extern const UnitType Protoss_Robotics_Facility;
    extern const UnitType Protoss_Pylon;
    extern const UnitType Protoss_Assimilator;
    // unused protoss 1 158
    extern const UnitType Protoss_Observatory;
    extern const UnitType Protoss_Gateway;
    // unused protoss 2 161
    extern const UnitType Protoss_Photon_Cannon;
    extern const UnitType Protoss_Citadel_of_Adun;
    extern const UnitType Protoss_Cybernetics_Core;
    extern const UnitType Protoss_Templar_Archives;
    extern const UnitType Protoss_Forge;
    extern const UnitType Protoss_Stargate;
    extern const UnitType Special_Stasis_Cell_Prison;
    extern const UnitType Protoss_Fleet_Beacon;
    extern const UnitType Protoss_Arbiter_Tribunal;
    extern const UnitType Protoss_Robotics_Support_Bay;
    extern const UnitType Protoss_Shield_Battery;
    extern const UnitType Special_Khaydarin_Crystal_Form;
    extern const UnitType Special_Protoss_Temple;
    extern const UnitType Special_XelNaga_Temple;
    extern const UnitType Resource_Mineral_Field;
    extern const UnitType Resource_Mineral_Field_Type_2;
    extern const UnitType Resource_Mineral_Field_Type_3;
    // cave 179
    // cave-in 180
    // cantina 181
    // mining platform 182
    // independant command center 183
    extern const UnitType Special_Independant_Starport;
    // independant jump gate 185
    // ruins 186
    // unused khaydarin crystal formation 187
    extern const UnitType Resource_Vespene_Geyser;
    extern const UnitType Special_Warp_Gate;
    extern const UnitType Special_Psi_Disrupter;
    // zerg marker 191
    // terran marker 192
    // protoss marker 193
    extern const UnitType Special_Zerg_Beacon;
    extern const UnitType Special_Terran_Beacon;
    extern const UnitType Special_Protoss_Beacon;
    extern const UnitType Special_Zerg_Flag_Beacon;
    extern const UnitType Special_Terran_Flag_Beacon;
    extern const UnitType Special_Protoss_Flag_Beacon;
    extern const UnitType Special_Power_Generator;
    extern const UnitType Special_Overmind_Cocoon;
    extern const UnitType Spell_Dark_Swarm;
    extern const UnitType Special_Floor_Missile_Trap;
    extern const UnitType Special_Floor_Hatch;
    extern const UnitType Special_Upper_Level_Door;
    extern const UnitType Special_Right_Upper_Level_Door;
    extern const UnitType Special_Pit_Door;
    extern const UnitType Special_Right_Pit_Door;
    extern const UnitType Special_Floor_Gun_Trap;
    extern const UnitType Special_Wall_Missile_Trap;
    extern const UnitType Special_Wall_Flame_Trap;
    extern const UnitType Special_Right_Wall_Missile_Trap;
    extern const UnitType Special_Right_Wall_Flame_Trap;
    extern const UnitType Special_Start_Location;
    extern const UnitType Powerup_Flag;
    extern const UnitType Powerup_Young_Chrysalis;
    extern const UnitType Powerup_Psi_Emitter;
    extern const UnitType Powerup_Data_Disk;
    extern const UnitType Powerup_Khaydarin_Crystal;
    extern const UnitType Powerup_Mineral_Cluster_Type_1;
    extern const UnitType Powerup_Mineral_Cluster_Type_2;
    extern const UnitType Powerup_Protoss_Gas_Orb_Type_1;
    extern const UnitType Powerup_Protoss_Gas_Orb_Type_2;
    extern const UnitType Powerup_Zerg_Gas_Sac_Type_1;
    extern const UnitType Powerup_Zerg_Gas_Sac_Type_2;
    extern const UnitType Powerup_Terran_Gas_Tank_Type_1;
    extern const UnitType Powerup_Terran_Gas_Tank_Type_2;

    extern const UnitType None;
    extern const UnitType AllUnits;
    extern const UnitType Men;
    extern const UnitType Buildings;
    extern const UnitType Factories;
    extern const UnitType Unknown;
  }
}
