#pragma once
#include <string>

#include <BWAPI/Position.h>
#include <BWAPI/Race.h>
#include <BWAPI/Filters.h>

namespace BWAPI
{
  // Forwards
  class Color;
  class Force;
  class PlayerType;
  class TechType;
  class Unit;
  class Unitset;
  class UnitType;
  class UpgradeType;
  class WeaponType;

  struct PlayerData;

  /// @~English
  /// The Player represents a unique controller in the game.
  /// Each player in a match will have his or her own player
  /// instance. There is also a neutral player which owns 
  /// all the neutral units (such as mineral patches and
  /// vespene geysers).
  ///
  /// @~
  /// @see Playerset, PlayerType, Race
  class Player
  {
  protected:
    virtual ~Player() {};
  public :
    /// @~English
    /// Retrieves a unique ID that represents the player.
    ///
    /// @returns The ID of the player.
    ///
    /// @~
    virtual int getID() const = 0;

    /// @~English
    /// Retrieves the name of the player as a string object.
    ///
    /// @returns The player name.
    ///
    /// @note Don't forget to use std::string::c_str() when 
    /// passing this parameter to Game::sendText and other
    /// variadic functions.
    ///
    /// @~
    /// @Ex player.cpp getName
    virtual std::string getName() const = 0;

    /// @~English
    /// Retrieves the set of all units that the player owns.
    /// This also includes incomplete units.
    ///
    /// @param pred An optional predicate that can filter
    /// the units to be more specific.
    ///
    /// @returns Unitset containing the units.
    ///
    /// @note This does not include units that are loaded
    /// into transports, @Bunkers, @Refineries, @Assimilators,
    /// or @Extractors.
    ///
    /// @~
    /// @Ex player.cpp getUnits
    virtual const Unitset &getUnits() const = 0;

    /// @~English
    /// Retrieves the race of the player. This allows
    /// you to change strategies against different
    /// races.
    ///
    /// @returns The Race that the player is using.
    ///
    /// @~
    /// @Ex player.cpp getRace
    virtual Race getRace() const = 0;

    /// @~English
    /// Retrieves the player's controller type.
    /// This allows you to distinguish betweeen 
    /// computer and human players.
    ///
    /// @returns The type that is controlling the player.
    ///
    /// @note Other players using BWAPI will be 
    /// treated as a human player and return
    /// PlayerTypes::Player.
    ///
    /// @~
    /// @Ex player.cpp getType
    virtual PlayerType getType() const = 0;

    /// @~English
    /// Retrieves the player's force. A force
    /// is the team that the player is playing
    /// on. This is only used in non-melee
    /// game types.
    ///
    /// @returns The force that the player is on.
    ///
    /// @note It is not called a team because
    /// players on the same force do not necessarily
    /// need to be allied at the beginning of a match.
    ///
    /// @~
    virtual Force* getForce() const = 0;

    /// @~English
    /// Checks if this player is allied to the specified
    /// player.
    ///
    /// @param player The player to check alliance with.
    ///
    /// @retval true if this player is allied with \p player .
    /// @retval false if this player is not allied with \p player.
    ///
    /// @note This function will also return false if this
    /// player is neutral or an observer, or if \p player
    /// is neutral or an observer.
    ///
    /// @~
    /// @see isEnemy
    virtual bool isAlly(Player* player) const = 0;

    /// @~English
    /// Checks if this player is unallied to the specified
    /// player.
    ///
    /// @param player The player to check alliance with.
    ///
    /// @retval true if this player is allied with \p player .
    /// @retval false if this player is not allied with \p player .
    ///
    /// @note This function will also return false if this
    /// player is neutral or an observer, or if \p player
    /// is neutral or an observer.
    ///
    /// @~
    /// @see isAlly
    virtual bool isEnemy(Player* player) const = 0;

    /// @~English
    /// Checks if this player is the neutral player.
    ///
    /// @retval true if this player is the neutral player.
    /// @retval false if this player is any other player.
    ///
    /// @~
    virtual bool isNeutral() const = 0;

    /// @~English
    /// Retrieve's the player's starting location.
    ///
    /// @returns A TilePosition containing the position
    /// of the start location.
    ///
    /// @retval TilePositions::None if the player does 
    /// not have a start location.
    /// @retval TilePositions::Unknown if an error occured
    /// while trying to retrieve the start location.
    ///
    /// @~
    /// @see Game::getStartLocations, Game::getLastError
    virtual TilePosition getStartLocation() const = 0;

    /** Returns true if the player has achieved victory. */
    virtual bool isVictorious() const = 0;

    /** Returns true if the player has been defeated. */
    virtual bool isDefeated() const = 0;

    /** Returns true if the player left the game. */
    virtual bool leftGame() const = 0;

    /** Returns the amount of minerals the player has. */
    virtual int minerals() const = 0;

    /** Returns the amount of vespene gas the player has. */
    virtual int gas() const = 0;

    /** Returns the cumulative amount of minerals the player has mined up to this point (including the 50
     * minerals at the start of the game). */
    virtual int gatheredMinerals() const = 0;

    /** Returns the cumulative amount of gas the player has harvested up to this point. */
    virtual int gatheredGas() const = 0;

    /** Returns the cumulative amount of minerals the player has spent on repairs up to this point. */
    virtual int repairedMinerals() const = 0;

    /** Returns the cumulative amount of gas the player has spent on repairs up to this point. */
    virtual int repairedGas() const = 0;

    /** Returns the cumulative amount of minerals the player has gained from refunded units up to this point. */
    virtual int refundedMinerals() const = 0;

    /** Returns the cumulative amount of gas the player has gained from refunded units up to this point. */
    virtual int refundedGas() const = 0;

    /** Returns the cumulative amount of minerals the player has spent up to this point (not including repairs). */
    virtual int spentMinerals() const = 0;

    /** Returns the cumulative amount of gas the player has spent up to this point (not including repairs). */
    virtual int spentGas() const = 0;

    /** Returns the total amount of supply the player has. If a race is provided, the total supply for the
     * given race will be returned, otherwise the player's initial race will be used. Supply counts returned
     * by BWAPI are double what you would expect to see from playing the game. This is because zerglings
     * take up 0.5 in-game supply. */
    virtual int supplyTotal(Race race = Races::None) const = 0;

    /** Returns how much of the supply is actually being used by units. If a race is provided, the used
     * supply for the given race will be returned, otherwise the player's initial race will be used. Supply
     * counts returned by BWAPI are double what you would expect to see from playing the game. This is
     * because zerglings take up 0.5 in-game supply. */
    virtual int supplyUsed(Race race = Races::None) const = 0;

    /** Returns the number of all accessible units of the given type. */
    virtual int allUnitCount(UnitType unit) const = 0;

    /** Returns the number of visible units of the given type. */
    virtual int visibleUnitCount(UnitType unit) const = 0;

    /** Returns the number of completed units of the given type. */
    virtual int completedUnitCount(UnitType unit) const = 0;

    /** Returns the number of incomplete units of the given type. */
    virtual int incompleteUnitCount(UnitType unit) const = 0;

    /** Returns the number of dead units of the given type. */
    virtual int deadUnitCount(UnitType unit) const = 0;

    /** Returns the number of killed units of the given type. */
    virtual int killedUnitCount(UnitType unit) const = 0;

    /** Returns the player's current upgrade level of the given upgrade. To order a unit to upgrade a given
     * upgrade type, see Unit::upgrade. */
    virtual int getUpgradeLevel(UpgradeType upgrade) const = 0;

    /** Returns true if the player has finished researching the given tech. To order a unit to research a
     * given tech type, see Unit::research. */
    virtual bool hasResearched(TechType tech) const = 0;

    /** Returns true if the player is researching the given tech. To order a unit to research a given tech
     * type, see Unit::reseach. */
    virtual bool isResearching(TechType tech) const = 0;

    /** Returns true if the player is upgrading the given upgrade. To order a unit to upgrade a given
     * upgrade type, see Unit::upgrade. */
    virtual bool isUpgrading(UpgradeType upgrade) const = 0;

    /** Returns the color of the player for drawing */
    virtual BWAPI::Color getColor() const = 0;

    /** Returns the color of the player for text messages */
    virtual int getTextColor() const = 0;

    /** Returns the max energy of the given unit type, taking into account upgrades */
    virtual int maxEnergy(UnitType unit) const = 0;

    /** Returns the top speed of the given unit type, includes upgrades */
    virtual double topSpeed(UnitType unit) const = 0;

    /** Returns the max range of the given weapon with upgrades */
    virtual int weaponMaxRange(WeaponType weapon) const = 0;

    /** Returns the sight range of the given unit type, includes upgrades */
    virtual int sightRange(UnitType unit) const = 0 ;

    /** Returns the ground weapon cooldown of the given unit type, includes upgrades */
    virtual int groundWeaponDamageCooldown(UnitType unit) const = 0;

    /** Returns the armor of the given unit type, includes upgrades */
    virtual int armor(UnitType unit) const = 0;

    /** Returns the Player's Total Unit Score */
    virtual int getUnitScore() const = 0;

    /** Returns the Player's Total Kill Score */
    virtual int getKillScore() const = 0;

    /** Returns the Player's Total Buildings Score */
    virtual int getBuildingScore() const = 0;

    /** Returns the Player's Total Razings Score */
    virtual int getRazingScore() const = 0;

    /** Returns the Player's Custom Score */
    virtual int getCustomScore() const = 0;

    /** Returns true if the Player is only observing the game, and not participating */
    virtual bool isObserver() const = 0;

    /** Returns the maximum upgrades available specific to the player (Use Map Settings). */
    virtual int  getMaxUpgradeLevel(UpgradeType upgrade) const = 0;

    /** Returns true if the research is available for the player to research (Use Map Settings). */
    virtual bool isResearchAvailable(TechType tech) const = 0;

    /** Returns true if the unit is available for the player to build (Use Map Settings). */
    virtual bool isUnitAvailable(UnitType unit) const = 0;
  };
};
