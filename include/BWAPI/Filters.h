#pragma once
#include <BWAPI/UnaryFilter.h>
#include <BWAPI/ComparisonFilter.h>
#include <BWAPI/BestFilter.h>

namespace BWAPI
{
  // forwards
  class Unit;
  class UpgradeType;
  class Player;
  class Order;
  class Race;
  class WeaponType;
  class UnitType;
  class UnitSizeType;

  // typedefs
  typedef UnaryFilter<Unit*> UnitFilter;
  typedef BestFilter<Unit*>  BestUnitFilter;

  // presets
  extern const UnitFilter IsTransport;

  extern const UnitFilter CanProduce;
  extern const UnitFilter CanAttack;
  extern const UnitFilter CanMove;
  extern const UnitFilter IsFlyer;
  extern const UnitFilter IsFlying;
  extern const UnitFilter RegeneratesHP;
  extern const UnitFilter IsSpellcaster;
  extern const UnitFilter HasPermanentCloak;
  extern const UnitFilter IsOrganic;
  extern const UnitFilter IsMechanical;
  extern const UnitFilter IsRobotic;
  extern const UnitFilter IsDetector;
  extern const UnitFilter IsResourceContainer;
  extern const UnitFilter IsResourceDepot;
  extern const UnitFilter IsRefinery;
  extern const UnitFilter IsWorker;
  extern const UnitFilter RequiresPsi;
  extern const UnitFilter RequiresCreep;
  extern const UnitFilter IsBurrowable;
  extern const UnitFilter IsCloakable;
  extern const UnitFilter IsBuilding;
  extern const UnitFilter IsAddon;
  extern const UnitFilter IsFlyingBuilding;
  extern const UnitFilter IsNeutral;
  extern const UnitFilter IsHero;
  extern const UnitFilter IsPowerup;
  extern const UnitFilter IsBeacon;
  extern const UnitFilter IsFlagBeacon;
  extern const UnitFilter IsSpecialBuilding;
  extern const UnitFilter IsSpell;
  extern const UnitFilter ProducesLarva;
  extern const UnitFilter IsMineralField;
  extern const UnitFilter IsCritter;
  extern const UnitFilter CanBuildAddon;
  
  extern const CompareFilter<Unit*> HP;
  extern const CompareFilter<Unit*> MaxHP;
  extern const CompareFilter<Unit*> HP_Percent;
  
  extern const CompareFilter<Unit*> Shields;
  extern const CompareFilter<Unit*> MaxShields;
  extern const CompareFilter<Unit*> Shields_Percent;
  
  extern const CompareFilter<Unit*> Energy;
  extern const CompareFilter<Unit*> MaxEnergy;
  extern const CompareFilter<Unit*> Energy_Percent;

  extern const CompareFilter<Unit*> Armor;
  extern const CompareFilter<Unit*,UpgradeType> ArmorUpgrade;

  extern const CompareFilter<Unit*> MineralPrice;
  extern const CompareFilter<Unit*> GasPrice;
  extern const CompareFilter<Unit*> BuildTime;

  extern const CompareFilter<Unit*> SupplyRequired;
  extern const CompareFilter<Unit*> SupplyProvided;

  extern const CompareFilter<Unit*> SpaceRequired;
  extern const CompareFilter<Unit*> SpaceRemaining;
  extern const CompareFilter<Unit*> SpaceProvided;

  extern const CompareFilter<Unit*> BuildScore;
  extern const CompareFilter<Unit*> DestroyScore;

  extern const CompareFilter<Unit*,double> TopSpeed;
  extern const CompareFilter<Unit*> SightRange;
  extern const CompareFilter<Unit*> WeaponCooldown;
  extern const CompareFilter<Unit*,UnitSizeType> SizeType;

  extern const CompareFilter<Unit*,WeaponType> GroundWeapon;
  extern const CompareFilter<Unit*,WeaponType> AirWeapon;

  extern const CompareFilter<Unit*,UnitType> GetType;
  extern const CompareFilter<Unit*,Race> GetRace;
  extern const CompareFilter<Unit*,Player*> GetPlayer;
  
  extern const CompareFilter<Unit*> Resources;
  extern const CompareFilter<Unit*> ResourceGroup;
  extern const CompareFilter<Unit*> AcidSporeCount;
  extern const CompareFilter<Unit*> InterceptorCount;
  extern const CompareFilter<Unit*> ScarabCount;
  extern const CompareFilter<Unit*> SpiderMineCount;
  extern const CompareFilter<Unit*> MaxWeaponCooldown;
  extern const CompareFilter<Unit*> SpellCooldown;

  extern const CompareFilter<Unit*> DefenseMatrixPoints;
  extern const CompareFilter<Unit*> DefenseMatrixTime;
  extern const CompareFilter<Unit*> EnsnareTime;
  extern const CompareFilter<Unit*> IrradiateTime;
  extern const CompareFilter<Unit*> LockdownTime;
  extern const CompareFilter<Unit*> MaelstromTime;
  extern const CompareFilter<Unit*> OrderTime;
  extern const CompareFilter<Unit*> PlagueTimer;
  extern const CompareFilter<Unit*> RemoveTime;
  extern const CompareFilter<Unit*> StasisTime;
  extern const CompareFilter<Unit*> StimTime;
  extern const CompareFilter<Unit*,UnitType> BuildType;
  extern const CompareFilter<Unit*> RemainingBuildTime;
  extern const CompareFilter<Unit*> RemainingTrainTime;
  extern const CompareFilter<Unit*,Unit*> Target;
  extern const CompareFilter<Unit*,Order> CurrentOrder;
  extern const CompareFilter<Unit*,Order> SecondaryOrder;
  extern const CompareFilter<Unit*,Unit*> OrderTarget;
  extern const CompareFilter<Unit*,void*> ClientInfo;
  extern const CompareFilter<Unit*> GetLeft;
  extern const CompareFilter<Unit*> GetTop;
  extern const CompareFilter<Unit*> GetRight;
  extern const CompareFilter<Unit*> GetBottom;

  extern const UnitFilter Exists;
  extern const UnitFilter IsAttacking;
  extern const UnitFilter IsBeingConstructed;
  extern const UnitFilter IsBeingGathered;
  extern const UnitFilter IsBeingHealed;
  extern const UnitFilter IsBlind;
  extern const UnitFilter IsBraking;
  extern const UnitFilter IsBurrowed;
  extern const UnitFilter IsCarryingGas;
  extern const UnitFilter IsCarryingMinerals;
  extern const UnitFilter IsCarryingSomething;
  extern const UnitFilter IsCloaked;
  extern const UnitFilter IsCompleted;
  extern const UnitFilter IsConstructing;
  extern const UnitFilter IsDefenseMatrixed;
  extern const UnitFilter IsDetected;
  extern const UnitFilter IsEnsnared;
  extern const UnitFilter IsFollowing;
  extern const UnitFilter IsGatheringGas;
  extern const UnitFilter IsGatheringMinerals;
  extern const UnitFilter IsHallucination;
  extern const UnitFilter IsHoldingPosition;
  extern const UnitFilter IsIdle;
  extern const UnitFilter IsInterruptible;
  extern const UnitFilter IsInvincible;
  extern const UnitFilter IsIrradiated;
  extern const UnitFilter IsLifted;
  extern const UnitFilter IsLoaded;
  extern const UnitFilter IsLockedDown;
  extern const UnitFilter IsMaelstrommed;
  extern const UnitFilter IsMorphing;
  extern const UnitFilter IsMoving;
  extern const UnitFilter IsParasited;
  extern const UnitFilter IsPatrolling;
  extern const UnitFilter IsPlagued;
  extern const UnitFilter IsRepairing;
  extern const UnitFilter IsResearching;
  extern const UnitFilter IsSieged;
  extern const UnitFilter IsStartingAttack;
  extern const UnitFilter IsStasised;
  extern const UnitFilter IsStimmed;
  extern const UnitFilter IsStuck;
  extern const UnitFilter IsTraining;
  extern const UnitFilter IsUnderAttack;
  extern const UnitFilter IsUnderDarkSwarm;
  extern const UnitFilter IsUnderDisruptionWeb;
  extern const UnitFilter IsUnderStorm;
  extern const UnitFilter IsUnpowered;
  extern const UnitFilter IsVisible;
  
  extern const UnitFilter IsEnemy;
  extern const UnitFilter IsAlly;
  extern const UnitFilter IsOwned;
}

