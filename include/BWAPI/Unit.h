#pragma once
#include <BWAPI/Position.h>
#include <BWAPI/PositionUnit.h>
#include <BWAPI/UnitType.h>
#include <BWAPI/Filters.h>
#include <BWAPI/UnaryFilter.h>
#include <BWAPI/Interface.h>

namespace BWAPI
{
  // Forwards
  class Player;
  class Order;
  class TechType;
  class UpgradeType;
  class Region;
  class UnitCommand;
  class UnitCommandType;
  class Unitset;
  class WeaponType;

  /// The Unit class is used to get information about individual units as well as issue orders to units. Each
  /// unit in the game has a unique Unit object, and Unit objects are not deleted until the end of the match
  /// (so you don't need to worry about unit pointers becoming invalid).
  ///
  /// Every Unit in the game is either accessible or inaccessible. To determine if an AI can access a
  /// particular unit, BWAPI checks to see if Flag::CompleteMapInformation is enabled. So there are two cases
  /// to consider - either the flag is enabled, or it is disabled:
  ///
  /// If Flag::CompleteMapInformation is disabled, then a unit is accessible if and only if it is visible.
  /// @note Some properties of visible enemy units will not be made available to the AI (such as the
  /// contents of visible enemy dropships). If a unit is not visible, Unit::exists will return false,
  /// regardless of whether or not the unit exists. This is because absolutely no state information on
  /// invisible enemy units is made available to the AI. To determine if an enemy unit has been destroyed, the
  /// AI must watch for AIModule::onUnitDestroy messages from BWAPI, which is only called for visible units
  /// which get destroyed.
  ///
  /// If Flag::CompleteMapInformation is enabled, then all units that exist in the game are accessible, and
  /// Unit::exists is accurate for all units. Similarly AIModule::onUnitDestroy messages are generated for all
  /// units that get destroyed, not just visible ones.
  ///
  /// If a Unit is not accessible, then only the getInitial__ functions will be available to the AI.
  /// However for units that were owned by the player, getPlayer and getType will continue to work for units
  /// that have been destroyed.
  class Unit : public Interface<Unit>
  {
  protected:
    virtual ~Unit() {};
  public:
    /// Retrieves a unique identifier for this unit.
    ///
    /// @returns An integer containing the unit's identifier.
    ///
    /// @see getReplayID
    virtual int getID() const = 0;

    /// Checks if the Unit exists in the view of the BWAPI player.
    ///
    /// This is used primarily to check if BWAPI has access to a specific unit, or if the
    /// unit is alive. This function is more general and would be synonymous to an isAlive
    /// function if such a function were necessary.
    ///
    /// @retval true If the unit exists on the map and is visible according to BWAPI.
    /// @retval false If the unit is not accessible or the unit is dead.
    ///
    /// In the event that this function returns false, there are two cases to consider:
    ///   1. You own the unit. This means the unit is dead.
    ///   2. Another player owns the unit. This could either mean that you don't have access
    ///      to the unit or that the unit has died. You can specifically identify dead units
    ///      by polling onUnitDestroy.
    ///
    /// @see isVisible, isCompleted
    virtual bool exists() const = 0;

    /// Retrieves the unit identifier for this unit as seen in replay data.
    ///
    /// @note This is only available if Flag::CompleteMapInformation is enabled.
    ///
    /// @returns An integer containing the replay unit identifier.
    ///
    /// @see getID
    virtual int getReplayID() const = 0;

    /// Retrieves the player that owns this unit.
    ///
    /// @retval Game::neutral() If the unit is a neutral unit or inaccessible.
    ///
    /// @returns A pointer to the owning Player object.
    virtual Player* getPlayer() const = 0;

    /// Retrieves the unit's type.
    ///
    /// @retval UnitTypes::Unknown if this unit is inaccessible or cannot be determined.
    ///
    /// @returns A UnitType objects representing the unit's type.
    ///
    /// @see getInitialType
    virtual UnitType getType() const = 0;

    /// Retrieves the unit's position from the upper left corner of the map in pixels.
    ///
    /// @retval Positions::Unknown if this unit is inaccessible.
    ///
    /// @returns Position object representing the unit's current position.
    ///
    /// @see getTilePosition, getInitialPosition
    virtual Position getPosition() const = 0;

    /// Retrieves the unit's build position from the upper left corner of the map in tiles.
    ///
    /// @note: This tile position is the tile that is at the top left corner of the structure.
    ///
    /// @retval TilePositions::Unknown if this unit is inaccessible.
    ///
    /// @returns TilePosition object representing the unit's current tile position.
    ///
    /// @see getPosition, getInitialTilePosition
    TilePosition getTilePosition() const;

    /// Retrieves the unit's facing direction in radians.
    ///
    /// @note A value of 0.0 means the unit is facing east.
    ///
    /// @returns A double with the angle measure in radians.
    virtual double getAngle() const = 0;

    /// Retrieves the x component of the unit's velocity, measured in pixels per frame.
    ///
    /// @returns A double that represents the velocity's x component.
    ///
    /// @see getVelocityY
    virtual double getVelocityX() const = 0;

    /// Retrieves the y component of the unit's velocity, measured in pixels per frame.
    ///
    /// @returns A double that represents the velocity's y component.
    ///
    /// @see getVelocityX
    virtual double getVelocityY() const = 0;

    /// Retrieves the Region that the center of the unit is in.
    ///
    /// @retval nullptr If the unit is inaccessible.
    ///
    /// @returns A pointer to a Region object that contains this unit.
    BWAPI::Region *getRegion() const;

    /// Retrieves the X coordinate of the unit's left boundry, measured in pixels from the left
    /// side of the map.
    ///
    /// @returns An integer representing the position of the left side of the unit.
    ///
    /// @see getTop, getRight, getBottom
    int getLeft() const;

    /// Retrieves the Y coordinate of the unit's top boundry, measured in pixels from the top of
    /// the map.
    ///
    /// @returns An integer representing the position of the top side of the unit.
    ///
    /// @see getLeft, getRight, getBottom
    int getTop() const;

    /// Retrieves the X coordinate of the unit's right boundry, measured in pixels from the left
    /// side of the map.
    ///
    /// @returns An integer representing the position of the right side of the unit.
    ///
    /// @see getLeft, getTop, getBottom
    int getRight() const;

    /// Retrieves the Y coordinate of the unit's bottom boundry, measured in pixels from the top
    /// of the map.
    ///
    /// @returns An integer representing the position of the bottom side of the unit.
    ///
    /// @see getLeft, getTop, getRight
    int getBottom() const;

    /// Retrieves the unit's current Hit Points (HP) as seen in the game.
    ///
    /// @returns An integer representing the amount of hit points a unit currently has.
    ///
    /// @note In Starcraft, a unit usually dies when its HP reaches 0. It is possible however, to
    /// have abnormal HP values in the Use Map Settings game type and as the result of a hack over
    /// Battle.net. Such values include units that have 0 HP (can't be killed conventionally)
    /// or even negative HP (death in one hit).
    ///
    /// @see UnitType::maxHitPoints, getShields, getInitialHitPoints
    virtual int getHitPoints() const = 0;

    /// Retrieves the unit's current Shield Points (Shields) as seen in the game.
    ///
    /// @returns An integer representing the amount of shield points a unit currently has.
    ///
    /// @see UnitType::maxShields, getHitPoints
    virtual int getShields() const = 0;

    /// Retrieves the unit's current Energy Points (Energy) as seen in the game.
    ///
    /// @returns An integer representing the amount of energy points a unit currently has.
    ///
    /// @note Energy is required in order for units to use abilities.
    ///
    /// @see UnitType::maxEnergy
    virtual int getEnergy() const = 0;

    /// Retrieves the resource amount from a resource container, such as a Mineral Field and
    /// Vespene Geyser.
    ///
    /// @returns An integer representing the amount of resources remaining in this resource.
    ///
    /// @see getInitialResources
    virtual int getResources() const = 0;

    /// Retrieves a grouping index from a resource container. Other resource containers of the
    /// same value are considered part of one expansion location (group of resources that are
    /// close together).
    ///
    /// @note This grouping method is explicitly determined by Starcraft itself and is used only
    /// by the internal AI.
    ///
    /// @returns An integer with an identifier between 0 and 250 that determine which resources
    /// are grouped together to form an expansion.
    virtual int getResourceGroup() const = 0;

    /// Retrieves the distance between this unit and a target.
    ///
    /// @note Distance is calculated from the edge of this unit, using Starcraft's own distance
    /// algorithm.
    ///
    /// @param target
    ///   A Position or a Unit to calculate the distance to. If it is a unit, then it will
    ///   calculate the distance to the edge of the target unit.
    ///
    /// @returns An integer representation of the number of pixels between this unit and the
    /// \p target.
    int getDistance(PositionOrUnit target) const;

    /// Using data provided by Starcraft, checks if there is a path available from this unit to
    /// the given target.
    ///
    /// @note This function only takes into account the terrain data, and does not include
    /// buildings when determining if a path is available. However, the time it takes to execute
    /// this function is O(1), and no extensive calculations are necessary.
    ///
    /// @note If the current unit is an air unit, then this function will always return true.
    ///
    /// @param target
    ///   A Position or a Unit that is used to determine if this unit has a path to the target.
    ///
    /// @retval true If there is a path between this unit and the target.
    /// @retval false If the target is on a different piece of land than this one (such as an
    /// island).
    bool hasPath(PositionOrUnit target) const;

    /** Returns the frame of the last successful command. Frame is comparable to Game::getFrameCount(). */
    virtual int getLastCommandFrame() const = 0;

    /** Returns the last successful command. */
    virtual UnitCommand getLastCommand() const = 0;

    /** Returns the player that last attacked this unit. */
    virtual BWAPI::Player *getLastAttackingPlayer() const = 0;

    /** Returns the initial type of the unit or Unknown if it wasn't a neutral unit at the beginning of the
     * game. */
    virtual UnitType getInitialType() const = 0;

    /** Returns the initial position of the unit on the map, or Positions::Unknown if the unit wasn't a
     * neutral unit at the beginning of the game. */
    virtual Position getInitialPosition() const = 0;

    /** Returns the initial build tile position of the unit on the map, or TilePositions::Unknown if the
     * unit wasn't a neutral unit at the beginning of the game. The tile position is of the top left corner
     * of the building. */
    virtual TilePosition getInitialTilePosition() const = 0;

    /** Returns the unit's initial amount of hit points, or 0 if it wasn't a neutral unit at the beginning
     * of the game. */
    virtual int getInitialHitPoints() const = 0;

    /** Returns the unit's initial amount of containing resources, or 0 if the unit wasn't a neutral unit
     * at the beginning of the game. */
    virtual int getInitialResources() const = 0;

    /** Returns the unit's current kill count. */
    virtual int getKillCount() const = 0;

    /** Returns the unit's acid spore count. */
    virtual int getAcidSporeCount() const = 0;

    /** Returns the number of interceptors the Protoss Carrier has. */
    virtual int getInterceptorCount() const = 0;

    /** Returns the number of scarabs in the Protoss Reaver. */
    virtual int getScarabCount() const = 0;

    /** Returns the number of spider mines in the Terran Vulture. */
    virtual int getSpiderMineCount() const = 0;

    /** Returns unit's ground weapon cooldown. It is 0 if the unit is ready to attack. */
    virtual int getGroundWeaponCooldown() const = 0;

    /** Returns unit's air weapon cooldown. It is 0 if the unit is ready to attack. */
    virtual int getAirWeaponCooldown() const = 0;

    /** Returns unit's ground weapon cooldown. It is 0 if the unit is ready cast a spell. */
    virtual int getSpellCooldown() const = 0;

    /** Returns the remaining hit points of the defense matrix. Initially a defense Matrix has 250 points.
     * \see Unit::getDefenseMatrixTimer, Unit::isDefenseMatrixed. */
    virtual int getDefenseMatrixPoints() const = 0;

    /** Returns the time until the defense matrix wears off. 0 -> No defense Matrix present. */
    virtual int getDefenseMatrixTimer() const = 0;

    /** Returns the time until the ensnare effect wears off. 0 -> No ensnare effect present. */
    virtual int getEnsnareTimer() const = 0;

    /** Returns the time until the radiation wears off. 0 -> No radiation present. */
    virtual int getIrradiateTimer() const = 0;

    /** Returns the time until the lockdown wears off. 0 -> No lockdown present. */
    virtual int getLockdownTimer() const = 0;

    /** Returns the time until the maelstrom wears off. 0 -> No maelstrom present. */
    virtual int getMaelstromTimer() const = 0;

    // TODO: add doc
    virtual int getOrderTimer() const = 0;

    /** Returns the time until the plague wears off. 0 -> No plague present. */
    virtual int getPlagueTimer() const = 0;

    /** Returns the amount of time until the unit is removed, or 0 if the unit does not have a remove timer.
     * Used to determine how much time remains before hallucinated units, dark swarm, etc have until they
     * are removed. */
    virtual int getRemoveTimer() const = 0;

    /** Returns the time until the stasis field wears off. 0 -> No stasis field present. */
    virtual int getStasisTimer() const = 0;

    /** Returns the time until the stimpack wears off. 0 -> No stimpack boost present. */
    virtual int getStimTimer() const = 0;

    /** Returns the building type a worker is about to construct. If the unit is a morphing Zerg unit or an
     * incomplete building, this returns the UnitType the unit is about to become upon completion.*/
    virtual UnitType getBuildType() const = 0;

    /** Returns the list of units queued up to be trained.
     * \see Unit::train, Unit::cancelTrain, Unit::isTraining. */
    virtual UnitType::set getTrainingQueue() const = 0;

    /** Returns the tech that the unit is currently researching. If the unit is not researching anything,
     * TechTypes::None is returned.
     * \see Unit::research, Unit::cancelResearch, Unit::isResearching, Unit::getRemainingResearchTime. */
    virtual TechType getTech() const = 0;

    /** Returns the upgrade that the unit is currently upgrading. If the unit is not upgrading anything,
     * UpgradeTypes::None is returned.
     * \see Unit::upgrade, Unit::cancelUpgrade, Unit::isUpgrading, Unit::getRemainingUpgradeTime. */
    virtual UpgradeType getUpgrade() const = 0;

    /** Returns the remaining build time of a unit/building that is being constructed. */
    virtual int getRemainingBuildTime() const = 0;

    /** Returns the remaining time of the unit that is currently being trained. If the unit is a Hatchery,
     * Lair, or Hive, this returns the amount of time until the next larva spawns, or 0 if the unit already
     * has 3 larva. */
    virtual int getRemainingTrainTime() const = 0;

    /** Returns the amount of time until the unit is done researching its current tech. If the unit is not
     * researching anything, 0 is returned.
     * \see Unit::research, Unit::cancelResearch, Unit::isResearching, Unit::getTech. */
    virtual int getRemainingResearchTime() const = 0;

    /** Returns the amount of time until the unit is done upgrading its current upgrade. If the unit is not
     * upgrading anything, 0 is returned.
     * \see Unit::upgrade, Unit::cancelUpgrade, Unit::isUpgrading, Unit::getUpgrade. */
    virtual int getRemainingUpgradeTime() const = 0;

    /** If the unit is an SCV that is constructing a building, this will return the building it is
     * constructing. If the unit is a Terran building that is being constructed, this will return the SCV
     * that is constructing it. */
    virtual Unit* getBuildUnit() const = 0;

    /** Generally returns the appropriate target unit after issuing an order that accepts a target unit
     * (i.e. attack, repair, gather, follow, etc.). To check for a target that has been acquired
     * automatically (without issuing an order) see getOrderTarget. */
    virtual Unit* getTarget() const = 0;

    /** Returns the target position the unit is moving to (provided a valid path to the target position
     * exists). */
    virtual Position getTargetPosition() const = 0;

    // TODO: add doc
    virtual Order getOrder() const = 0;
    virtual Order getSecondaryOrder() const = 0;

    /** This is usually set when the low level unit AI acquires a new target automatically. For example if
     * an enemy probe comes in range of your marine, the marine will start attacking it, and getOrderTarget
     * will be set in this case, but not getTarget. */
    virtual Unit* getOrderTarget() const = 0;

  /** Returns the target position for the units order. For example for the move order getTargetPosition
   * returns the end of the units path but this returns the location the unit is trying to move to.  */
    virtual Position getOrderTargetPosition() const = 0;

    /** Returns the position the building is rallied to. If the building does not produce units,
     * Positions::None is returned.
     * \see Unit::setRallyPoint, Unit::getRallyUnit. */
    virtual Position getRallyPosition() const = 0;

    /** Returns the unit the building is rallied to. If the building is not rallied to any unit, NULL is
     * returned.
     * \see Unit::setRallyPoint, Unit::getRallyPosition. */
    virtual Unit* getRallyUnit() const = 0;

    /** Returns the add-on of this unit, or NULL if the unit doesn't have an add-on. */
    virtual Unit* getAddon() const = 0;

    /// Retrieves the @Nydus_Canal that is attached to this one. Every @Nydus_Canal can place a
    /// "Nydus Exit" which, when connected, can be travelled through by @Zerg units.
    ///
    /// @returns Unit interface representing the @Nydus_Canal connected to this one.
    /// @retval nullptr if the unit is not a @Nydus_Canal, is not owned, or has not placed a Nydus
    /// Exit.
    virtual Unit* getNydusExit() const = 0;

    /// Retrieves the power-up that the worker unit is holding. Power-ups are special units such
    /// as the flag in the Capture the Flag game type, which can be picked up by worker units.
    ///
    /// @note If your bot is strictly melee/1v1, then this method is not necessary.
    ///
    /// @return The Unit interface that represents the power-up.
    /// @retval nullptr If the unit is not carrying anything.
    virtual Unit* getPowerUp() const = 0;

    /// Retrieves the @Transport or @Bunker unit that has this unit loaded inside of it.
    ///
    /// @returns @Transport containing this unit.
    /// @retval nullptr if this unit is not in a @Transport.
    virtual Unit* getTransport() const = 0;

    /// Retrieves the set of units that are contained within this @Bunker or @Transport .
    ///
    /// @returns A Unitset object containing all of the units that are loaded inside of the
    /// current unit.
    virtual Unitset getLoadedUnits() const = 0;

    /// Retrieves the remaining unit-space available for @Bunker s and transports.
    ///
    /// @returns The number of spots available to transport a unit.
    ///
    /// @see getLoadedUnits
    int getSpaceRemaining() const;

    /** For Protoss Interceptors, this returns the Carrier unit this Interceptor is controlled by. For all
     * other unit types this function returns NULL. */
    virtual Unit* getCarrier() const = 0;

    /** Returns the set of interceptors controlled by this unit. If the unit has no interceptors, or is not
     * a Carrier, this function returns an empty set. */
    virtual Unitset getInterceptors() const = 0;

    /** For Zerg Larva, this returns the Hatchery, Lair, or Hive unit this Larva was spawned from. For all
     * other unit types this function returns NULL. */
    virtual Unit* getHatchery() const = 0;

    /** Returns the set of larva spawned by this unit. If the unit has no larva, or is not a Hatchery, Lair,
     * or Hive, this function returns an empty set. Equivalent to clicking "Select Larva" from the Starcraft
     * GUI. */
    virtual Unitset getLarva() const = 0;

    /// Retrieves the set of all units in a given radius of the current unit.
    ///
    /// Takes into account this unit's dimensions. Can optionally specify a filter that is composed using
    /// BWAPI Filter semantics to include only specific units (such as only ground units, etc.)
    ///
    /// @param radius
    ///   The radius, in pixels, to search for units.
    /// @param pred (optional)
    ///   The composed function predicate to include only specific (desired) units in the set. Defaults to
    ///   nullptr, which means no filter.
    ///
    /// @returns A Unitset containing the set of units that match the given criteria.
    ///
    /// @code
    ///   // Get main building closest to start location.
    ///   Unit *pMain = Broodwar->getClosestUnit( Broodwar->self()->getStartLocation(), IsResourceDepot );
    ///   if ( pMain != nullptr ) // check if pMain is valid
    ///   {
    ///     // Get sets of resources and workers
    ///     Unitset myResources = pMain->getUnitsInRadius(1024, IsMineralField);
    ///     if ( !myResources.empty() ) // check if we have resources nearby
    ///     {
    ///       Unitset myWorkers = pMain->getUnitsInRadius(512, IsWorker && IsIdle && IsOwned );
    ///       while ( !myWorkers.empty() ) // make sure we command all nearby idle workers, if any
    ///       {
    ///         for ( auto u = myResources.begin(); u != myResources.end() && !myWorkers.empty(); ++u )
    ///         {
    ///           myWorkers.back()->gather(*u);
    ///           myWorkers.pop_back();
    ///         }
    ///       }
    ///     } // myResources not empty
    ///   } // pMain != nullptr
    /// @endcode
    ///
    /// @see getClosestUnit, getUnitsInWeaponRange, Game::getUnitsInRadius, Game::getUnitsInRectangle
    Unitset getUnitsInRadius(int radius, const UnitFilter &pred = nullptr) const;

    /** Returns the set of units within weapon range of this unit. */
    Unitset getUnitsInWeaponRange(WeaponType weapon, const UnitFilter &pred = nullptr) const;

    // @TODO
    Unit *getClosestUnit(const UnitFilter &pred = nullptr, int radius = 999999) const;

    /* Returns true if the Nuclear Missile Silo has a nuke */
    virtual bool hasNuke() const = 0;

    /** Returns true if the unit is currently accelerating. */
    virtual bool isAccelerating() const = 0;

    // @TODO: add doc
    virtual bool isAttacking() const = 0;

    // @TODO: add doc
    virtual bool isAttackFrame() const = 0;

    /// Checks if the current unit is being constructed. This is mostly applicable to Terran
    /// structures which require an SCV to be constructing a structure.
    ///
    /// @retval true if this is either a Protoss structure, Zerg structure, or Terran structure
    /// being constructed by an attached SCV.
    /// @retval false if this is either completed, not a structure, or has no SCV constructing it
    ///
    /// @see Unit::build, Unit::cancelConstruction, Unit::haltConstruction, Unit::isConstructing
    bool isBeingConstructed() const;

    /** Returns true if the unit is a mineral patch or refinery that is being gathered. */
    virtual bool isBeingGathered() const = 0;

    /** Returns true if the unit is currently being healed by a Terran Medic, or repaired by a Terran SCV. */
    virtual bool isBeingHealed() const = 0;

    /** Returns true if the unit is currently blind from a Medic's Optical Flare. */
    virtual bool isBlind() const = 0;

    /** Returns true if the unit is currently braking/slowing down. */
    virtual bool isBraking() const = 0;

    /** Returns true if the unit is a Zerg unit that is current burrowed.
     * \see Unit::burrow, Unit::unburrow. */
    virtual bool isBurrowed() const = 0;

    /** Returns true if the unit is a worker that is carrying gas.
     * \see Unit::returnCargo, Unit::isGatheringGas. */
    virtual bool isCarryingGas() const = 0;

    /** Returns true if the unit is a worker that is carrying minerals.
     * \see Unit::returnCargo, Unit::isGatheringMinerals. */
    virtual bool isCarryingMinerals() const = 0;

    /** Returns true if the unit is cloaked.
     * \see Unit::cloak, Unit::decloak. */
    virtual bool isCloaked() const = 0;

    /** Returns true if the unit has been completed. */
    virtual bool isCompleted() const = 0;

    /** Returns true when a unit has been issued an order to build a structure and is moving to the build
     * location. Also returns true for Terran SCVs while they construct a building.
     * \see Unit::build, Unit::cancelConstruction, Unit::haltConstruction, Unit::isBeingConstructed. */
    virtual bool isConstructing() const = 0;

    /** Returns true if the unit has a defense matrix from a Terran Science Vessel. */
    bool isDefenseMatrixed() const;

    /** Returns true if the unit is detected. */
    virtual bool isDetected() const = 0;

    /// Checks if the @Queen ability @Ensnare has been used on this unit.
    ///
    /// @retval true if the unit is ensnared
    /// @retval false if the unit is not ensnared
    bool isEnsnared() const;

    /// This macro function checks if the units is in the air. That is, the unit is either a flyer
    /// or a flying building.
    ///
    /// @retval true if it is in the air
    /// @retval false if it is on the ground
    /// @see UnitType::isFlyer, Unit::isLifted
    bool isFlying() const;

    /** Returns true if the unit is following another unit.
     * \see Unit::follow, Unit::getTarget. */
    bool isFollowing() const;

    /** Returns true if the unit is in one of the four states for gathering gas (MoveToGas, WaitForGas,
     * HarvestGas, ReturnGas).
     * \see Unit::isCarryingGas. */
    virtual bool isGatheringGas() const = 0;

    /** Returns true if the unit is in one of the four states for gathering minerals (MoveToMinerals,
     * WaitForMinerals, MiningMinerals, ReturnMinerals).
     * \see Unit::isCarryingMinerals. */
    virtual bool isGatheringMinerals() const = 0;

    /** Returns true for hallucinated units, false for normal units. Returns true for hallucinated enemy
     * units only if Complete Map Information is enabled.
     * \see Unit::getRemoveTimer. */
    virtual bool isHallucination() const = 0;

    /** Returns true if the unit is holding position
     * \see Unit::holdPosition. */
    bool isHoldingPosition() const;

    /** Returns true if the unit is not doing anything.
     * \see Unit::stop. */
    /// Checks if this unit is not doing anything. This function is particularly useful when
    /// checking for units that aren't doing any tasks.
    ///
    /// @note This implies that the unit is completed, so if this returns true, then
    /// Unit::isCompleted should also return true.
    ///
    /// @code
    ///   BWAPI::Unitset myUnits = BWAPI::Broodwar->self()->getUnits();
    ///   for ( auto u = myUnits.begin(); u != myUnits.end(); ++u )
    ///   {
    ///     // Order idle worker to gather from closest mineral field
    ///     if ( u->getType().isWorker() && u->isIdle() )
    ///       u->gather( u->getClosestUnit( BWAPI::IsMineralField ) );
    ///   }
    /// @endcode
    ///
    /// @retval true if this unit is idle
    /// @retval false if this unit is performing some action such as moving or attacking
    /// @see Unit::stop
    virtual bool isIdle() const = 0;

    /** Returns true if the unit can be interrupted. */
    virtual bool isInterruptible() const = 0;

    /** Returns true if the unit is invincible. */
    virtual bool isInvincible() const = 0;

    /** Returns true if the unit can attack a specified target from its current position. */
    bool isInWeaponRange(Unit *target) const;

    /** Returns true if the unit is being irradiated by a Terran Science Vessel.
     * \see Unit::getIrradiateTimer. */
    bool isIrradiated() const;

    /** Returns true if the unit is a Terran building that is currently lifted off the ground.
     * \see Unit::lift,Unit::land. */
    virtual bool isLifted() const = 0;

    /** Return true if the unit is loaded into a Terran Bunker, Terran Dropship, Protoss Shuttle, or Zerg
     * Overlord.
     * \see Unit::load, Unit::unload, Unit::unloadAll. */
    bool isLoaded() const;

    /** Returns true if the unit is locked down by a Terran Ghost.
     *  \see Unit::getLockdownTimer. */
    bool isLockedDown() const;

    /** Returns true if the unit is being maelstrommed.
     * \see Unit::getMaelstromTimer. */
    bool isMaelstrommed() const;

    /// Finds out if the current unit is morphing or not. Zerg units and structures often have
    /// the ability to morph into different types of units. This function allows you to identify
    /// when this process is occurring.
    ///
    /// @retval true if the unit is currently morphing.
    /// @retval false if the unit is not morphing
    ///
    /// @see Unit::morph, Unit::cancelMorph, Unit::getBuildType, Unit::getRemainingBuildTime
    virtual bool isMorphing() const = 0;

    /** Returns true if the unit is moving.
     * \see Unit::attack, Unit::stop. */
    virtual bool isMoving() const = 0;

    /** Returns true if the unit has been parasited by some other player. */
    virtual bool isParasited() const = 0;

    /** Returns true if the unit is patrolling between two positions.
     * \see Unit::patrol. */
    bool isPatrolling() const;

    /** Returns true if the unit has been plagued by a Zerg Defiler.
     * \see Unit::getPlagueTimer. */
    bool isPlagued() const;

    /** Returns true if the unit is a Terran SCV that is repairing or moving to repair another unit. */
    bool isRepairing() const;

    /** Returns true if the unit is a building that is researching tech. See TechTypes for the complete list
     * of available techs in Broodwar.
     * \see Unit::research, Unit::cancelResearch, Unit::getTech, Unit::getRemainingResearchTime. */
    bool isResearching() const;

    /** Returns true if the unit has been selected by the user via the starcraft GUI. Only available if you
     * enable Flag::UserInput during AIModule::onStart.
     * \see Game::getSelectedUnits. */
    virtual bool isSelected() const = 0;

    /** Returns true if the unit is a Terran Siege Tank that is currently in Siege mode.
     * \see Unit::siege, Unit::unsiege. */
    bool isSieged() const;

    /** Returns true if the unit is starting to attack.
     * \see Unit::attackUnit, Unit::getGroundWeaponCooldown, Unit::getAirWeaponCooldown. */
    virtual bool isStartingAttack() const = 0;

    /** Returns true if the unit has been stasised by a Protoss Arbiter.
     * \see Unit::getStasisTimer. */
    bool isStasised() const;

    /** Returns true if the unit is currently stimmed.
     * \see Unit::getStimTimer. */
    bool isStimmed() const;

    /** Returns true if the unit is being pushed off of another unit */
    virtual bool isStuck() const = 0;

    /** Returns true if the unit is training units (i.e. a Barracks training Marines).
     * \see Unit::train, Unit::getTrainingQueue, Unit::cancelTrain, Unit::getRemainingTrainTime. */
    virtual bool isTraining() const = 0;

    /** Returns true if the unit was recently attacked. */
    virtual bool isUnderAttack() const = 0;

    /** Returns true if the unit is under a Dark Swarm. */
    virtual bool isUnderDarkSwarm() const = 0;

    /** Returns true if the unit is under a Disruption Web. */
    virtual bool isUnderDisruptionWeb() const = 0;

    /** Returns true if the unit is under a Protoss Psionic Storm. */
    virtual bool isUnderStorm() const = 0;

    /** Returns true if the unit is a Protoss building that is unpowered because no pylons are in range. */
    virtual bool isUnpowered() const = 0;

    /** Returns true if the unit is a building that is upgrading. See UpgradeTypes for the complete list
     * of available upgrades in Broodwar.
     * \see Unit::upgrade, Unit::cancelUpgrade, Unit::getUpgrade, Unit::getRemainingUpgradeTime. */
    bool isUpgrading() const;

    /** Returns true if the unit is visible. If the CompleteMapInformation?  cheat flag is enabled, existing
     * units hidden by the fog of war will be accessible, but isVisible will still return false.
     * \see Unit::exists. */
    virtual bool isVisible(Player* player = nullptr) const = 0;

    /// This function issues a command to the unit(s), however it is used for interfacing only,
    /// and is recommended to use one of the more specific command functions when writing an AI.
    ///
    /// @param command
    ///   A UnitCommand containing command parameters such as the type, position, target, etc.
    ///
    /// @retval true if BWAPI determined that the command was valid and passed it to Starcraft.
    /// @retval false if an error occured and the command could not be executed.
    ///
    /// @see UnitCommandTypes, Game::getLastError, Unit::canIssueCommand
    virtual bool issueCommand(UnitCommand command) = 0;

    /// Orders the unit(s) to attack move to the specified location.
    ///
    /// @param target
    ///   A Position or a Unit to designate as the target. If a Position is used, the unit will
    ///   perform an Attack Move command.
    /// @param shiftQueueCommand
    ///   If this value is true, then the order will be queued instead of immediately executed.
    ///
    /// @retval true if BWAPI determined that the command was valid and passed it to Starcraft.
    /// @retval false if an error occured and the command could not be executed.
    ///
    /// @note A @Medic will use Heal Move instead of attack.
    ///
    /// @see Game::getLastError, Unit::canAttackMove, Unit::canAttackUnit
    bool attack(PositionOrUnit target, bool shiftQueueCommand = false);

    /// Orders the worker unit(s) to construct a structure at a target position.
    ///
    /// @param type
    ///   The UnitType to build.
    /// @param target
    ///   A TilePosition to specify the build location, specifically the upper-left corner of the
    ///   location. If the target is not specified, then the function call will be redirected to
    ///   the train command.
    ///
    /// @retval true if BWAPI determined that the command was valid and passed it to Starcraft.
    /// @retval false if an error occured and the command could not be executed.
    ///
    /// @note You must have sufficient resources and meet the necessary requirements in order to
    /// build a structure.
    ///
    /// @see Game::getLastError, Unit::train, Unit::cancelConstruction, Unit::canBuild
    bool build(UnitType type, TilePosition target = TilePositions::None);

    /// Orders the @Terran structure(s) to construct an add-on.
    ///
    /// @param type
    ///   The add-on UnitType to construct.
    ///
    /// @retval true if BWAPI determined that the command was valid and passed it to Starcraft.
    /// @retval false if an error occured and the command could not be executed.
    ///
    /// @note You must have sufficient resources and meet the necessary requirements in order to
    /// build a structure.
    ///
    /// @see Game::getLastError, Unit::build, Unit::cancelAddon, Unit::canBuildAddon
    bool buildAddon(UnitType type);

    /// Orders the unit(s) to add a UnitType to its training queue, or morphs into the UnitType if
    /// it is @Zerg.
    ///
    /// @param type
    ///   The UnitType to train.
    ///
    /// @retval true if BWAPI determined that the command was valid and passed it to Starcraft.
    /// @retval false if an error occured and the command could not be executed.
    ///
    /// @note You must have sufficient resources, supply, and meet the necessary requirements in
    /// order to train a unit.
    /// @note This command is also used for training @Interceptors and @Scarabs.
    /// @note If you call this using a @Hatchery, @Lair, or @Hive, then it will automatically
    /// pass the command to one of its @Larvae.
    ///
    /// @see Game::getLastError, Unit::build, Unit::morph, Unit::cancelTrain, Unit::isTraining,
    /// Unit::canTrain
    bool train(UnitType type = UnitTypes::None);

    /// Orders the unit(s) to morph into a different UnitType.
    ///
    /// @param type
    ///   The UnitType to morph into.
    ///
    /// @retval true if BWAPI determined that the command was valid and passed it to Starcraft.
    /// @retval false if an error occured and the command could not be executed.
    ///
    /// @see Game::getLastError, Unit::build, Unit::morph, Unit::canMorph
    bool morph(UnitType type);

    /** Orders the unit to research the given tech type.
     * \see Unit::cancelResearch, Unit::Unit#isResearching, Unit::getRemainingResearchTime, Unit::getTech,
     * Unit::canResearch. */
    bool research(TechType tech);

    /** Orders the unit to upgrade the given upgrade type.
     * \see Unit::cancelUpgrade, Unit::Unit#isUpgrading, Unit::getRemainingUpgradeTime, Unit::getUpgrade,
     * Unit::canUpgrade. */
    bool upgrade(UpgradeType upgrade);

    /** Orders the unit to set its rally position to the specified position.
     * \see Unit::getRallyPosition, Unit::getRallyUnit, Unit::canSetRallyPosition, Unit::canSetRallyUnit. */
    bool setRallyPoint(PositionOrUnit target);

    /** Orders the unit to move from its current position to the specified position.
     * \see Unit::isMoving, Unit::canMove. */
    bool move(Position target, bool shiftQueueCommand = false);

    /** Orders the unit to patrol between its current position and the specified position.
     * \see Unit::isPatrolling, Unit::canPatrol. */
    bool patrol(Position target, bool shiftQueueCommand = false);

    /** Orders the unit to hold its position.
     * \see Unit::canHoldPosition. */
    bool holdPosition(bool shiftQueueCommand = false);

    /** Orders the unit to stop.
     * \see Unit::canStop. */
    bool stop(bool shiftQueueCommand = false);

    /** Orders the unit to follow the specified unit.
     * \see Unit::isFollowing, Unit::canFollow. */
    bool follow(Unit* target, bool shiftQueueCommand = false);

    /** Orders the unit to gather the specified unit (must be mineral or refinery type).
     * \see Unit::isGatheringGas, Unit::isGatheringMinerals, Unit::canGather. */
    bool gather(Unit* target, bool shiftQueueCommand = false);

    /** Orders the unit to return its cargo to a nearby resource depot such as a Command Center. Only
     * workers that are carrying minerals or gas can be ordered to return cargo.
     * \see Unit::isCarryingGas, Unit::isCarryingMinerals, Unit::canReturnCargo. */
    bool returnCargo(bool shiftQueueCommand = false);

    /** Orders the unit to repair the specified unit. Only Terran SCVs can be ordered to repair, and the
     * target must be a mechanical Terran unit or building.
     * \see Unit::isRepairing, Unit::canRepair. */
    bool repair(Unit* target, bool shiftQueueCommand = false);

    /** Orders the unit to burrow. Either the unit must be a Zerg Lurker, or the unit must be a Zerg ground
     * unit and burrow tech must be researched.
     * \see: Unit::unburrow, Unit::isBurrowed, Unit::canBurrow. */
    bool burrow();

    /** Orders the burrowed unit to unburrow.
     * \see: Unit::burrow, Unit::isBurrowed, Unit::canUnburrow. */
    bool unburrow();

    /** Orders the unit to cloak.
     * \see: Unit::decloak, Unit::isCloaked, Unit::canCloak. */
    bool cloak();

    /** Orders the unit to decloak.
     * \see: Unit::cloak, Unit::isCloaked, Unit::canDecloak. */
    bool decloak();

    /** Orders the unit to siege. Note: unit must be a Terran siege tank.
     * \see Unit::unsiege, Unit::isSieged, Unit::canSiege. */
    bool siege();

    /** Orders the unit to unsiege. Note: unit must be a Terran siege tank.
     * \see: Unit::unsiege, Unit::isSieged, Unit::canUnsiege. */
    bool unsiege();

    /** Orders the unit to lift. Note: unit must be a Terran building that can be lifted.
     * \see Unit::land, Unit::isLifted, Unit::canLift. */
    bool lift();

    /** Orders the unit to land. Note: unit must be a Terran building that is currently lifted.
     * \see Unit::lift, Unit::isLifted, Unit::canLand. */
    bool land(TilePosition target);

    /** Orders the unit to load the target unit.
     * \see Unit::unload, Unit::unloadAll, Unit::getLoadedUnits, Unit:isLoaded. */
    bool load(Unit* target, bool shiftQueueCommand = false);

    /** Orders the unit to unload the target unit.
     * \see Unit::load, Unit::unloadAll, Unit::getLoadedUnits, Unit:isLoaded, Unit::canUnload,
     * Unit::canUnloadAtPosition. */
    bool unload(Unit* target);

    /** Orders the unit to unload all loaded units at the unit's current position.
     * \see Unit::load, Unit::unload, Unit::unloadAll, Unit::getLoadedUnits, Unit:isLoaded,
     * Unit::canUnloadAll, Unit::canUnloadAtPosition. */
    bool unloadAll(bool shiftQueueCommand = false);

    /** Orders the unit to unload all loaded units at the specified location. Unit should be a Terran
     * Dropship, Protoss Shuttle, or Zerg Overlord. If the unit is a Terran Bunker, the units will be
     * unloaded right outside the bunker, like in the first version of unloadAll.
     * \see Unit::load, Unit::unload, Unit::unloadAll, Unit::getLoadedUnits, Unit:isLoaded,
     * Unit:canUnloadAllPosition, Unit::canUnloadAtPosition. */
    bool unloadAll(Position target, bool shiftQueueCommand = false);

    /** Works like the right click in the GUI.
     * \see Unit::canRightClickPosition, Unit::canRightClickUnit. */
    bool rightClick(PositionOrUnit target, bool shiftQueueCommand = false);

    /** Orders the SCV to stop constructing the building, and the building is left in a partially complete
     * state until it is canceled, destroyed, or completed.
     * \see Unit::isConstructing, Unit::canHaltConstruction. */
    bool haltConstruction();

    /** Orders the building to stop being constructed.
     * \see Unit::beingConstructed, Unit::canCancelConstruction. */
    bool cancelConstruction();

    /** Orders the unit to stop making the addon.
     * \see Unit::canCancelAddon. */
    bool cancelAddon();

    /** Orders the unit to remove the specified unit from its training queue.
     * \see Unit::train, Unit::cancelTrain, Unit::isTraining, Unit::getTrainingQueue.
     * Unit::canCancelTrain, Unit::canCancelTrainSlot. */
    bool cancelTrain(int slot = -2);

    /** Orders the unit to stop morphing.
     * \see Unit::morph, Unit::isMorphing, Unit::canCancelMorph. */
    bool cancelMorph();

    /** Orders the unit to cancel a research in progress.
     * \see Unit::research, Unit::isResearching, Unit::getTech, Unit::canCancelResearch. */
    bool cancelResearch();

    /** Orders the unit to cancel an upgrade in progress.
     * \see Unit::upgrade, Unit::isUpgrading, Unit::getUpgrade, Unit::canCancelUpgrade. */
    bool cancelUpgrade();
    
    /** Orders the unit to use a tech requiring a position target (eg Dark Swarm). Returns true if it is a
     * valid tech.
     * \see Unit::canUseTechWithOrWithoutTarget, Unit::canUseTechWithoutTarget, Unit::canUseTechUnit,
     * Unit::canUseTechPosition. */
    bool useTech(TechType tech, PositionOrUnit target = nullptr);

    /** Moves a Flag Beacon to the target location.
     * \see Unit::canPlaceCOP. */
    bool placeCOP(TilePosition target);

    /// Checks whether the unit is able to execute the given command. If you are calling this
    /// function repeatedly (e.g. to generate a collection of valid commands), you can avoid
    /// repeating the same kinds of checks by specifying false for some of the optional boolean
    /// arguments. Make sure that the state hasn't changed since the check was done though
    /// (eg a new frame/event, or a command issued). Also see the more specific functions.
    ///
    /// @param command
    ///   A UnitCommand to check.
    /// @param checkCanUseTechUnitOnUnits
    ///   Only used if the command type is UnitCommandTypes::Enum::Use_Tech_Unit. A boolean for
    ///   whether to perform cheap checks for whether the unit is unable to target any units using
    ///   the command's TechType (i.e. regardless of what the other command parameters are). You
    ///   can set this to false if you know this check has already just been performed.
    /// @param checkCanBuildUnitType
    ///   Only used if the command type is UnitCommandTypes::Build. A boolean for whether to
    ///   perform cheap checks for whether the unit is unable to build the specified UnitType
    ///   (i.e. regardless of what the other command parameters are). You can set this to false if
    ///   you know this check has already just been performed.
    /// @param checkCanTargetUnit
    ///   Only used for command types that can target a unit. A boolean for whether to perform
    ///   Unit::canTargetUnit as a check. You can set this to false if you know this check has
    ///   already just been performed.
    /// @param checkCanIssueCommandType
    ///   A boolean for whether to perform Unit::canIssueCommandType as a check. You can set this
    ///   to false if you know this check has already just been performed.
    /// @param checkCommandibility
    ///   A boolean for whether to perform Unit::canCommand as a check. You can set this to false
    ///   if you know this check has already just been performed.
    ///
    /// @retval true if BWAPI determined that the command is valid.
    /// @retval false if an error occurred and the command is invalid.
    ///
    /// @see UnitCommandTypes, Game::getLastError, Unit::canCommand, Unit::canIssueCommandType,
    /// Unit::canTargetUnit
    virtual bool canIssueCommand(UnitCommand command, bool checkCanUseTechUnitOnUnits = true, bool checkCanBuildUnitType = true, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Performs some cheap checks to attempt to quickly detect whether the unit is unable to
    /// execute any commands (eg the unit is stasised).
    ///
    /// @retval true if BWAPI was unable to determine whether the unit can be commanded.
    /// @retval false if an error occurred and the unit can not be commanded.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand
    virtual bool canCommand() const = 0;

    /// Performs some cheap checks to attempt to quickly detect whether the unit is unable to
    /// execute the given command type (i.e. regardless of what other possible command parameters
    /// could be).
    ///
    /// @param ct
    ///   A UnitCommandType.
    /// @param checkCommandibility
    ///   A boolean for whether to perform Unit::canCommand as a check. You can set this to false
    ///   if you know this check has already just been performed.
    ///
    /// @retval true if BWAPI was unable to determine whether the command type is invalid.
    /// @retval false if an error occurred and the command type is invalid.
    ///
    /// @see UnitCommandTypes, Game::getLastError, Unit::canIssueCommand
    virtual bool canIssueCommandType(UnitCommandType ct, bool checkCommandibility = true) const = 0;

    /// Performs some cheap checks to attempt to quickly detect whether the unit is unable to
    /// use the given unit as the target unit of an unspecified command.
    ///
    /// @param targetUnit
    ///   A target unit for an unspecified command.
    /// @param checkCommandibility
    ///   A boolean for whether to perform Unit::canCommand as a check. You can set this to false
    ///   if you know this check has already just been performed.
    ///
    /// @retval true if BWAPI was unable to determine whether the unit can target the given unit.
    /// @retval false if an error occurred and the unit can not target the given unit.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand
    virtual bool canTargetUnit(const Unit* targetUnit, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute an attack command to attack-move.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::attack
    virtual bool canAttackMove(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute an attack command to attack a unit.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::attack
    virtual bool canAttackUnit(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute an attack command to attack a unit.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::attack
    virtual bool canAttackUnit(Unit* targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a build command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::build
    virtual bool canBuild(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a build command for the given
    /// UnitType.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::build
    virtual bool canBuild(UnitType uType, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a build command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::build
    virtual bool canBuild(UnitType uType, BWAPI::TilePosition tilePos, bool checkTargetUnitType = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a buildAddon command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::buildAddon
    virtual bool canBuildAddon(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a buildAddon command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::buildAddon
    virtual bool canBuildAddon(UnitType uType, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a train command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::train
    virtual bool canTrain(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a train command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::train
    virtual bool canTrain(UnitType uType, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a morph command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::morph
    virtual bool canMorph(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a morph command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::morph
    virtual bool canMorph(UnitType uType, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a research command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::research
    virtual bool canResearch(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a research command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::research
    virtual bool canResearch(TechType type, bool checkCanIssueCommandType = true) const = 0;

    /// Cheap checks for whether the unit is able to execute an upgrade command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::upgrade
    virtual bool canUpgrade(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute an upgrade command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::upgrade
    virtual bool canUpgrade(UpgradeType type, bool checkCanIssueCommandType = true) const = 0;

    /// Checks whether the unit is able to execute a setRallyPoint command to a position.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::setRallyPoint
    virtual bool canSetRallyPosition(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a setRallyPoint command to a unit.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::setRallyPoint
    virtual bool canSetRallyUnit(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a setRallyPoint command to a unit.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::setRallyPoint
    virtual bool canSetRallyUnit(const Unit* targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a move command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::move
    virtual bool canMove(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a patrol command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::patrol
    virtual bool canPatrol(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a follow command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::follow
    virtual bool canFollow(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a follow command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::follow
    virtual bool canFollow(const Unit* targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a gather command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::gather
    virtual bool canGather(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a gather command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::gather
    virtual bool canGather(const Unit* targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a returnCargo command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::returnCargo
    virtual bool canReturnCargo(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a holdPosition command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::holdPosition
    virtual bool canHoldPosition(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a stop command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::stop
    virtual bool canStop(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a repair command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::repair
    virtual bool canRepair(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a repair command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::repair
    virtual bool canRepair(const Unit* targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a burrow command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::burrow
    virtual bool canBurrow(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute an unburrow command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::unburrow
    virtual bool canUnburrow(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a cloak command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::cloak
    virtual bool canCloak(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a decloak command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::decloak
    virtual bool canDecloak(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a siege command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::siege
    virtual bool canSiege(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute an unsiege command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::unsiege
    virtual bool canUnsiege(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a lift command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::lift
    virtual bool canLift(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a land command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::land
    virtual bool canLand(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a land command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::land
    virtual bool canLand(TilePosition target, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a load command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::load
    virtual bool canLoad(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a load command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::load
    virtual bool canLoad(const Unit* targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute an unload command or unloadAll at
    /// current position command or unloadAll at a different position command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::unload, Unit::unloadAll
    virtual bool canUnloadWithOrWithoutTarget(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute an unload command or unloadAll at
    /// current position command or unloadAll at a different position command, for a given
    /// position.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::unload, Unit::unloadAll
    virtual bool canUnloadAtPosition(Position targDropPos, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute an unload command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::unload
    virtual bool canUnload(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute an unload command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::unload, Unit::canUnloadAtPosition
    virtual bool canUnload(const Unit* targetUnit, bool checkCanTargetUnit = true, bool checkPosition = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute an unloadAll command for the current position.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::unloadAll
    virtual bool canUnloadAll(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute an unloadAll command for a different
    /// position.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::unloadAll
    virtual bool canUnloadAllPosition(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute an unloadAll command for a different position.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::unloadAll
    virtual bool canUnloadAllPosition(Position targDropPos, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a rightClick command for a position.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::rightClick
    virtual bool canRightClickPosition(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a rightClick command to a unit.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::rightClick
    virtual bool canRightClickUnit(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a rightClick command to a unit.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::rightClick
    virtual bool canRightClickUnit(Unit* targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a haltConstruction command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::haltConstruction
    virtual bool canHaltConstruction(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a cancelConstruction command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::cancelConstruction
    virtual bool canCancelConstruction(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a cancelAddon command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::cancelAddon
    virtual bool canCancelAddon(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a cancelTrain command for any slot.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::cancelTrain
    virtual bool canCancelTrain(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a cancelTrain command for an
    /// unspecified slot.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::cancelTrain
    virtual bool canCancelTrainSlot(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a cancelTrain command for a specified slot.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::cancelTrain
    virtual bool canCancelTrainSlot(int slot, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a cancelMorph command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::cancelMorph
    virtual bool canCancelMorph(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a cancelResearch command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::cancelResearch
    virtual bool canCancelResearch(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a cancelUpgrade command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::cancelUpgrade
    virtual bool canCancelUpgrade(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a useTech command without a target or
    /// or a useTech command with a target position or a useTech command with a target unit.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::useTech
    virtual bool canUseTechWithOrWithoutTarget(bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a useTech command without a target or
    /// or a useTech command with a target position or a useTech command with a target unit, for a
    /// given TechType.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::useTech
    virtual bool canUseTechWithOrWithoutTarget(BWAPI::TechType tech, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a useTech command without a target.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::useTech
    virtual bool canUseTechWithoutTarget(BWAPI::TechType tech, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a useTech command with an unspecified
    /// target unit.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::useTech
    virtual bool canUseTechUnit(BWAPI::TechType tech, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a useTech command with a target unit.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::useTech
    virtual bool canUseTechUnit(BWAPI::TechType tech, const Unit* targetUnit, bool checkCanTargetUnit = true, bool checkTargetsUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a useTech command with a target position.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::useTech
    virtual bool canUseTechPosition(BWAPI::TechType tech, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;

    /// Cheap checks for whether the unit is able to execute a placeCOP command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::placeCOP
    virtual bool canPlaceCOP(bool checkCommandibility = true) const = 0;

    /// Checks whether the unit is able to execute a placeCOP command.
    ///
    /// @see Game::getLastError, Unit::canIssueCommand, Unit::placeCOP
    virtual bool canPlaceCOP(TilePosition target, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const = 0;
  };
}
