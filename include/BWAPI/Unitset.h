#pragma once
#include <BWAPI/Vectorset.h>
#include <BWAPI/Position.h>
#include <BWAPI/PositionUnit.h>
#include <BWAPI/Filters.h>

namespace BWAPI
{
  // Forward declarations
  class Unit;
  class UnitType;
  class Regionset;
  class UnitCommand;
  class TechType;

  /// @~English
  /// The Unitset is a container for a set of pointers
  /// to Unit objects. It is typically used for groups
  /// of units instead of having to manage each Unit
  /// individually.
  ///
  /// @note Because of the efficiency, it is recommended
  /// to use the Unitset over any of the STL containers.
  ///
  /// @~
  /// @see Unit, UnitType, UnitSizeType
  class Unitset : public Vectorset<BWAPI::Unit*>
  {
  public:
    /// @~English
    /// A blank Unitset containing no elements.
    /// This is typically used as a return value
    /// for BWAPI interface functions that
    /// have encountered an error.
    /// @~
    static const Unitset none;

    /// @~English
    /// The Unitset constructor.
    ///
    /// @param initialSize The initial maximum size of
    /// the Unitset before expanding it is necessary.
    /// @~
    Unitset(size_t initialSize = 16);
    
    /// @overload
    /// @~English 
    ///
    /// @param other The Unitset to construct this one
    /// from.
    /// @~
    Unitset(const Unitset &other);
    Unitset(Unitset &&other);

    /// @~English
    /// Calculates the average of all valid Unit positions
    /// in this set.
    ///
    /// @returns Average Position of all units in the set.
    /// @~
    /// @see Unit::getPosition
    Position getPosition() const;

    /// @~English
    /// Creates a single set containing all units that
    /// are loaded into units of this set.
    ///
    /// @returns The set of all loaded units.
    /// @~
    /// @see Unit::getLoadedUnits
    Unitset getLoadedUnits() const;

    /// @~English
    /// Creates a single set containing all the
    /// @Interceptors of all @Carriers in this set.
    ///
    /// @returns The set of all @Interceptors .
    /// @~
    /// @see Unit::getInterceptors
    Unitset getInterceptors() const;

    /// @~English
    /// Creates a single set containing all the
    /// @Larvae of all @Hatcheries, @Lairs, and
    /// @Hives in this set.
    ///
    /// @returns The set of all @Larvae .
    /// @~
    /// @see Unit::getLarva
    Unitset getLarva() const;

    /// @~English
    /// Sets the client info for every unit
    /// in this set.
    ///
    /// @param clientInfo A pointer to client information,
    /// managed by the AI module, or NULL if client 
    /// information is to be cleared.
    /// @~
    /// @see Unit::setClientInfo
    void setClientInfo(void *clientInfo = nullptr, int index = 0) const;
    void setClientInfo(int clientInfo = 0, int index = 0) const;

    Unitset getUnitsInRadius(int radius, const UnitFilter &pred = nullptr) const;
    Unit    *getClosestUnit(const UnitFilter &pred = nullptr, int radius = 999999) const;

    /// @copydoc Unit::issueCommand
    bool issueCommand(UnitCommand command) const;
    
    /// @copydoc Unit::attack
    bool attack(PositionOrUnit target, bool shiftQueueCommand = false) const;

    /// @copydoc Unit::build
    bool build(UnitType type, TilePosition target = TilePositions::None) const;

    /// @copydoc Unit::buildAddon
    bool buildAddon(UnitType type) const;

    /// @copydoc Unit::train
    bool train(UnitType type) const;

    /// @copydoc Unit::morph
    bool morph(UnitType type) const;

    /// @copydoc Unit::setRallyPoint
    bool setRallyPoint(PositionOrUnit target) const;

    /// @copydoc Unit::move
    bool move(Position target, bool shiftQueueCommand = false) const;

    /// @copydoc Unit::patrol
    bool patrol(Position target, bool shiftQueueCommand = false) const;

    /// @copydoc Unit::holdPosition
    bool holdPosition(bool shiftQueueCommand = false) const;

    /// @copydoc Unit::stop
    bool stop(bool shiftQueueCommand = false) const;

    /// @copydoc Unit::follow
    bool follow(Unit* target, bool shiftQueueCommand = false) const;

    /// @copydoc Unit::gather
    bool gather(Unit* target, bool shiftQueueCommand = false) const;

    /// @copydoc Unit::returnCargo
    bool returnCargo(bool shiftQueueCommand = false) const;

    /// @copydoc Unit::repair
    bool repair(Unit* target, bool shiftQueueCommand = false) const;

    /// @copydoc Unit::burrow
    bool burrow() const;

    /// @copydoc Unit::unburrow
    bool unburrow() const;

    /// @copydoc Unit::cloak
    bool cloak() const;

    /// @copydoc Unit::decloak
    bool decloak() const;

    /// @copydoc Unit::siege
    bool siege() const;

    /// @copydoc Unit::unsiege
    bool unsiege() const;

    /// @copydoc Unit::lift
    bool lift() const;

    /// @copydoc Unit::load
    bool load(Unit* target, bool shiftQueueCommand = false) const;

    /// @copydoc Unit::unloadAll(bool)
    bool unloadAll(bool shiftQueueCommand = false) const;

    /// @copydoc Unit::unloadAll(Position,bool)
    bool unloadAll(Position target, bool shiftQueueCommand = false) const;

    /// @copydoc Unit::rightClick
    bool rightClick(PositionOrUnit target, bool shiftQueueCommand = false) const;

    /// @copydoc Unit::haltConstruction
    bool haltConstruction() const;

    /// @copydoc Unit::cancelConstruction
    bool cancelConstruction() const;

    /// @copydoc Unit::cancelAddon
    bool cancelAddon() const;

    /// @copydoc Unit::cancelTrain
    bool cancelTrain(int slot = -2) const;

    /// @copydoc Unit::cancelMorph
    bool cancelMorph() const;

    /// @copydoc Unit::cancelResearch
    bool cancelResearch() const;

    /// @copydoc Unit::cancelUpgrade
    bool cancelUpgrade() const;
    
    /// @copydoc Unit::useTech
    bool useTech(TechType tech, PositionOrUnit target = nullptr) const;
  };
}

