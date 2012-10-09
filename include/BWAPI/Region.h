#pragma once
#include <BWAPI/Position.h>
#include <BWAPI/Filters.h>
#include <BWAPI/UnaryFilter.h>
#include <BWAPI/Interface.h>

namespace BWAPI
{
  class Regionset;
  class Unitset;
  class Unit;

  class Region : public Interface
  {
  protected:
    virtual ~Region() {};
  public:
    /// @~English
    /// Retrieves a unique identifier for this region.
    ///
    /// @note This identifier is explicitly assigned by Broodwar.
    ///
    /// @returns
    ///   An integer that represents this region.
    /// @~
    /// @see Game::getRegion
    virtual int getID() const = 0;

    /** Returns an ID for the body of land that this region belongs to. */
    virtual int getRegionGroupID() const = 0;

    /** Retrieves the center of the region. */
    virtual BWAPI::Position getCenter() const = 0;

    /** Returns true if this region belongs to strategic higher ground, assigned by Broodwar. */
    virtual bool isHigherGround() const = 0;

    /** Returns a value representing the strategic advantage of this region according to Broodwar. */
    virtual int getDefensePriority() const = 0;

    /** Returns true if the region can be walked on by ground units. */
    virtual bool isWalkable() const = 0;

    /** Retrieves the set of neighbors that this region is connected to. */
    virtual const Regionset &getNeighbors() const = 0;

    /** Retrieves the approximate region boundry as specified by Broodwar. */
    virtual int getBoundsLeft() const = 0;
    virtual int getBoundsTop() const = 0;
    virtual int getBoundsRight() const = 0;
    virtual int getBoundsBottom() const = 0;

    /** Retrieves the closest accessible neighbor region. */
    virtual BWAPI::Region *getClosestAccessibleRegion() const = 0;

    /** Retrieves the closest inaccessible neighbor region. */
    virtual BWAPI::Region *getClosestInaccessibleRegion() const = 0;

    /** Retrieves the center-to-center distance between two regions. */
    int getDistance(BWAPI::Region *other) const;

    /// @~English
    /// Retrieves a Unitset containing all the units that are in this region. Also has the ability
    /// to filter the units before the creation of the Unitset.
    ///
    /// @param pred (optional)
    ///   If this parameter is used, it is a UnitFilter or function predicate that will retrieve
    ///   only the units whose attributes match the given criteria. If omitted, then a default
    ///   value of nullptr is used, in which case there is no filter.
    ///
    /// @returns A Unitset containing all units in this region that have met the requirements
    /// of \p pred.
    /// @~
    /// @see UnitFilter
    Unitset getUnits(const UnitFilter &pred = nullptr) const;
  };
};
