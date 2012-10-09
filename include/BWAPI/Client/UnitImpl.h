#pragma once
#include <BWAPI.h>
#include "UnitData.h"
#include <string>

namespace BWAPI
{
  class Player;
  class UnitImpl : public Unit
  {
    private:
      int         id;
      UnitType    initialType;
      int         initialResources;
      int         initialHitPoints;
      Position    initialPosition;
      int         lastCommandFrame;
      UnitCommand lastCommand;
    public:
      UnitData* self;
      Unitset   connectedUnits;
      Unitset   loadedUnits;
      void      clear();
      void      saveInitialState();

      UnitImpl(int id);

      virtual int           getID() const;
      virtual bool          exists() const;
      virtual int           getReplayID() const;
      virtual Player*       getPlayer() const;
      virtual UnitType      getType() const;
      virtual Position      getPosition() const;
      virtual TilePosition  getTilePosition() const;
      virtual double        getAngle() const;
      virtual double        getVelocityX() const;
      virtual double        getVelocityY() const;
      virtual BWAPI::Region *getRegion() const;
      virtual int           getLeft() const;
      virtual int           getTop() const;
      virtual int           getRight() const;
      virtual int           getBottom() const;
      virtual int           getHitPoints() const;
      virtual int           getShields() const;
      virtual int           getEnergy() const;
      virtual int           getResources() const;
      virtual int           getResourceGroup() const;

      virtual int           getDistance(PositionOrUnit target) const;
      virtual bool          hasPath(PositionOrUnit target) const;
      virtual int           getLastCommandFrame() const;
      virtual UnitCommand   getLastCommand() const;
      virtual BWAPI::Player *getLastAttackingPlayer() const;
      virtual int           getUpgradeLevel(UpgradeType upgrade) const;

      virtual UnitType      getInitialType() const;
      virtual Position      getInitialPosition() const;
      virtual TilePosition  getInitialTilePosition() const;
      virtual int           getInitialHitPoints() const;
      virtual int           getInitialResources() const;

      virtual int getKillCount() const;
      virtual int getAcidSporeCount() const;
      virtual int getInterceptorCount() const;
      virtual int getScarabCount() const;
      virtual int getSpiderMineCount() const;
      virtual int getGroundWeaponCooldown() const;
      virtual int getAirWeaponCooldown() const;
      virtual int getSpellCooldown() const;
      virtual int getDefenseMatrixPoints() const;

      virtual int getDefenseMatrixTimer() const;
      virtual int getEnsnareTimer() const;
      virtual int getIrradiateTimer() const;
      virtual int getLockdownTimer() const;
      virtual int getMaelstromTimer() const;
      virtual int getOrderTimer() const;
      virtual int getPlagueTimer() const;
      virtual int getRemoveTimer() const;
      virtual int getStasisTimer() const;
      virtual int getStimTimer() const;

      virtual UnitType      getBuildType() const;
      virtual UnitType::set getTrainingQueue() const;
      virtual TechType      getTech() const;
      virtual UpgradeType   getUpgrade() const;
      virtual int           getRemainingBuildTime() const;
      virtual int           getRemainingTrainTime() const;
      virtual int           getRemainingResearchTime() const;
      virtual int           getRemainingUpgradeTime() const;
      virtual Unit*         getBuildUnit() const;

      virtual Unit*     getTarget() const;
      virtual Position  getTargetPosition() const;
      virtual Order     getOrder() const;
      virtual Unit*     getOrderTarget() const;
      virtual Position  getOrderTargetPosition() const;
      virtual Order     getSecondaryOrder() const;
      virtual Position  getRallyPosition() const;
      virtual Unit*     getRallyUnit() const;
      virtual Unit*     getAddon() const;
      virtual Unit*     getNydusExit() const;
      virtual Unit*     getPowerUp() const;

      virtual Unit*     getTransport() const;
      virtual Unitset   getLoadedUnits() const;
      virtual Unit*     getCarrier() const;
      virtual Unitset   getInterceptors() const;
      virtual Unit*     getHatchery() const;
      virtual Unitset   getLarva() const;

      virtual bool hasNuke() const;
      virtual bool isAccelerating() const;
      virtual bool isAttackFrame() const;
      virtual bool isAttacking() const;
      virtual bool isBeingConstructed() const;
      virtual bool isBeingGathered() const;
      virtual bool isBeingHealed() const;
      virtual bool isBlind() const;
      virtual bool isBraking() const;
      virtual bool isBurrowed() const;
      virtual bool isCarryingGas() const;
      virtual bool isCarryingMinerals() const;
      virtual bool isCloaked() const;
      virtual bool isCompleted() const;
      virtual bool isConstructing() const;
      virtual bool isDetected() const;
      virtual bool isGatheringGas() const;
      virtual bool isGatheringMinerals() const;
      virtual bool isHallucination() const;
      virtual bool isInWeaponRange(Unit *target) const;
      virtual bool isIdle() const;
      virtual bool isInterruptible() const;
      virtual bool isInvincible() const;
      virtual bool isLifted() const;
      virtual bool isMorphing() const;
      virtual bool isMoving() const;
      virtual bool isParasited() const;
      virtual bool isSelected() const;
      virtual bool isStartingAttack() const;
      virtual bool isStuck() const;
      virtual bool isTraining() const;
      virtual bool isUnderAttack() const;
      virtual bool isUnderDarkSwarm() const;
      virtual bool isUnderDisruptionWeb() const;
      virtual bool isUnderStorm() const;
      virtual bool isUnpowered() const;
      virtual bool isVisible(Player* player = nullptr) const;

      virtual bool canIssueCommand(UnitCommand command) const;
      virtual bool issueCommand(UnitCommand command);
  };
}
