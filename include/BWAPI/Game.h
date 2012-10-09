#pragma once
#include <list>
#include <string>

#include <BWAPI/UnitType.h>
#include <BWAPI/Error.h>

#include <BWAPI/Filters.h>
#include <BWAPI/UnaryFilter.h>
#include <BWAPI/Interface.h>

namespace BWAPI
{
  // Forward Declarations
  class Bulletset;
  class Color;
  class Event;
  class Force;
  class Forceset;
  class GameType;
  class Player;
  class Playerset;
  class Race;
  class Region;
  class Regionset;
  class TechType;
  class Unit;
  class UnitCommand;
  class Unitset;
  class UpgradeType;

  enum MouseButton;
  enum Key;

  /** The abstract Game class is implemented by BWAPI and offers many methods for retrieving information
   * about the current Broodwar game, including the set of players, units, map information, as well as
   * information about the user, such as mouse position, screen position, and the current selection of
   * units. */
  class Game : public Interface
  {
  protected:
    virtual ~Game() {};
  public :
    /** Returns the set of all forces in the match. */
    virtual const Forceset& getForces() const = 0;

    /** Returns the set of all players in the match. Note that this includes the Neutral player, which owns
     * all the neutral units such as minerals, critters, etc. */
    virtual const Playerset& getPlayers() const = 0;

    /** Returns all the visible units. If Flag::CompleteMapInformation is enabled, the set of all units
     * is returned, not just visible ones. Note that units inside refineries are not included in this set
     * yet. */
    virtual const Unitset& getAllUnits() const = 0;

    /** Returns the set of all accessible mineral patches. */
    virtual const Unitset& getMinerals() const = 0;

    /** Returns the set of all accessible vespene geysers. */
    virtual const Unitset& getGeysers() const = 0;

    /** Returns the set of all accessible neutral units. */
    virtual const Unitset& getNeutralUnits() const = 0;

    /** Returns the set of all mineral patches (including mined out and other inaccessible ones). */
    virtual const Unitset& getStaticMinerals() const = 0;

    /** Returns the set of all vespene geysers (including mined out and other inaccessible ones). */
    virtual const Unitset& getStaticGeysers() const = 0;

    /** Returns the set of all neutral units (including mined out and other inaccessible ones). */
    virtual const Unitset& getStaticNeutralUnits() const = 0;

    /** Returns all visible bullets. If Flag::CompleteMapInformation is enabled, the set of all bullets is
     * returned, not just visible ones. */
    virtual const Bulletset& getBullets() const = 0;

   /** Returns all visible nuke dots. If Flag::CompleteMapInformation is enabled, the set of all nuke dots
     * is returned, not just visible ones. */
    virtual const Position::set& getNukeDots() const = 0;

    /** Returns the list of events */
    virtual const std::list< Event >& getEvents() const = 0;

    /** Returns the force with the given ID, or nullptr if no force has the given ID */
    virtual Force* getForce(int forceID) const = 0;

    /** Returns the player with the given ID, or nullptr if no player has the given ID */
    virtual Player* getPlayer(int playerID) const = 0;

    /** Returns the unit with the given ID, or nullptr if no unit has the given ID */
    virtual Unit* getUnit(int unitID) const = 0;

    /** Returns a pointer to a Unit given an index. */
    virtual Unit* indexToUnit(int unitIndex) const = 0;

    /** Returns the Region with the given ID, or nullptr if no region has the given ID */
    virtual Region* getRegion(int regionID) const = 0;

    /** Returns the game type */
    virtual GameType getGameType() const = 0;

    /** Returns the amount of latency the current game has. Currently only returns Latency::SinglePlayer,
     * Latency::LanLow, Latency::LanMedium, or Latency::LanHigh. */
    virtual int getLatency() const = 0;

    /** Returns the number of logical frames since the match started. If the game is paused,
     * Game::getFrameCount will not increase however AIModule::onFrame will still be called while paused.
     * On Fastest, there are about 23.8 - 24 frames per second. */
    virtual int getFrameCount() const = 0;

    /** Retrieves the number of frames in the replay */
    virtual int getReplayFrameCount() const = 0;

    /** Returns the Frames Per Second (FPS) that the game is currently running at */
    virtual int getFPS() const = 0;
    virtual double getAverageFPS() const = 0;

    /** Returns the position of the mouse on the screen. Returns Positions::Unknown if Flag::UserInput is
     * disabled. */
    virtual Position getMousePosition() const = 0;

    /** Returns true if the specified mouse button is pressed. Returns false if Flag::UserInput is
     * disabled. */
    virtual bool getMouseState(MouseButton button) const = 0;
    virtual bool getMouseState(int button) const = 0;

    /** Returns true if the specified key is pressed. Returns false if Flag::UserInput is disabled.
     * Unfortunately this does not read the raw keyboard input yet - when you hold down a key, the
     * getKeyState function is true for a frame, then false for a few frames, and then alternates between
     * true and false (as if you were holding down the key in a text box). Hopefully this will be fixed in
     * a later version. */
    virtual bool getKeyState(Key key) const = 0;
    virtual bool getKeyState(int key) const = 0;

    /** Returns the position of the top left corner of the screen on the map. Returns Positions::Unknown if
     * Flag::UserInput is disabled. */
    virtual BWAPI::Position getScreenPosition() const = 0;

    /** Moves the screen to the given position on the map. The position specified where the top left corner
     * of the screen will be. */
    virtual void setScreenPosition(int x, int y) = 0;
    void setScreenPosition(BWAPI::Position p);

    /** Pings the given position on the minimap. */
    virtual void pingMinimap(int x, int y) = 0;
    void pingMinimap(BWAPI::Position p);

    /** Returns true if the given flag has been enabled. Note that flags can only be enabled at the
     * beginning of a match, during the AIModule::onStart callback. */
    virtual bool isFlagEnabled(int flag) const = 0;

    /** Enables the specified flag. Note that flags can only be enabled at the beginning of a match, during
     * the AIModule::onStart callback. */
    virtual void enableFlag(int flag) = 0;

    /** Returns the set of accessible units that are on the given build tile. */
    Unitset getUnitsOnTile(int tileX, int tileY, const UnitFilter &pred = nullptr) const;
    Unitset getUnitsOnTile(BWAPI::TilePosition tile, const UnitFilter &pred = nullptr) const;

    /** Returns the set of accessible units that are in or overlapping the given rectangle. */
    virtual Unitset getUnitsInRectangle(int left, int top, int right, int bottom, const UnitFilter &pred = nullptr) const = 0;

    /** Returns the set of accessible units within or overlapping a circle at the given point with the given radius. */
    Unitset getUnitsInRadius(BWAPI::Position center, int radius, const UnitFilter &pred = nullptr) const;

    /// @~English
    /// Retrieves the closest unit to center that
    /// matches the criteria of pred within a radius.
    ///
    /// @param center The position to start searching
    /// for the closest unit.
    /// @param pred The predicate to use to determine
    /// which unit is acceptable.
    /// @param radius The radius to search in.
    ///
    /// @returns The desired unit that is closest
    /// to center.
    /// @retval nullptr If a suitable unit was not found.
    /// @~
    /// @see getBestUnit
    Unit *getClosestUnit(Position center, const UnitFilter &pred = nullptr, int radius = 999999) const;
    virtual Unit *getClosestUnitInRectangle(Position center, const UnitFilter &pred = nullptr, int left = 0, int top = 0, int right = 999999, int bottom = 999999) const = 0;

    /// @~English
    /// Compares all units with pred to determine
    /// which of them is the best. All units are
    /// checked. If center and radius are specified,
    /// then it will check all units that are within
    /// the radius of the position.
    ///
    /// @param pred @TODO cannot be UnitFilter to determine "best"
    /// @param center The position to use in the search.
    /// @param radius The distance from \p center to search
    /// for units.
    /// 
    /// @returns The desired unit that best matches
    /// the given criteria.
    /// @retval nullptr if a suitable unit was not found.
    /// @~
    /// @see getClosestUnit
    virtual Unit *getBestUnit(const BestUnitFilter &best, const UnitFilter &pred, Position center = Positions::None, int radius = 999999) const = 0;

    /// @~English
    /// Returns the last error that was set using setLastError.
    /// If a function call in BWAPI has failed, you can use
    /// this function to retrieve the reason it failed.
    ///
    /// @returns Error type containing the reason for failure.
    /// @~
    /// @see setLastError, Errors
    virtual Error getLastError() const = 0;

    /// @~English
    /// Sets the last error so that future calls to 
    /// getLastError will return the value that was set.
    ///
    /// @param e The error code to set.
    ///
    /// @retval true If the type passed was Errors::None.
    /// @retval false If any other error type was passed.
    /// @~
    /// @see getLastError, Errors
    virtual bool setLastError(BWAPI::Error e = Errors::None) = 0;

    /** Returns the width of the current map, in build tile units. To get the width of the current map in
     * walk tile units, multiply by 4. To get the width of the current map in Position units, multiply by
     * TILE_SIZE (which is 32). */
    virtual int mapWidth() const = 0;

    /** Returns the height of the current map, in build tile units. To get the height of the current map in
     * walk tile units, multiply by 4. To get the height of the current map in Position units, multiply by
     * TILE_SIZE (which is 32). */
    virtual int mapHeight() const = 0;

    /** Returns the file name of the current map. */
    virtual std::string mapFileName() const = 0;

    /** Returns the full path name of the current map. */
    virtual std::string mapPathName() const = 0;

    /** Returns the name/title of the current map. */
    virtual std::string mapName() const = 0;

    /** Returns the SHA-1 hash of the map file. */
    virtual std::string mapHash() const = 0;

    /** Returns true if the specified walk tile is walkable. The values of x and y are in walk tile
     * coordinates (different from build tile coordinates). Note that this just uses the static map data.
     * You will also need to make sure no ground units are on the coresponding build tile to see if its
     * currently walkable. To do this, see unitsOnTile. */
    virtual bool isWalkable(int walkX, int walkY) const = 0;
    bool isWalkable(BWAPI::WalkPosition position) const;

    /** Returns the ground height of the given build tile. 0 = normal, 1 = high ground.  2 = very high ground. */
    virtual int  getGroundHeight(int tileX, int tileY) const = 0;
    int  getGroundHeight(TilePosition position) const;

    /** Returns true if the specified build tile is buildable. Note that this just uses the static map data.
     * You will also need to make sure no ground units on the tile to see if its currently buildable. To do
     * this, see unitsOnTile. */
    virtual bool isBuildable(int tileX, int tileY, bool includeBuildings = false) const = 0;
    bool isBuildable(TilePosition position, bool includeBuildings = false) const;

    /** Returns true if the specified build tile is visible. If the tile is concealed by fog of war, the
     * function will return false. */
    virtual bool isVisible(int tileX, int tileY) const = 0;
    bool isVisible(TilePosition position) const;

    /** Returns true if the specified build tile has been explored (i.e. was visible at some point in the
     * match). */
    virtual bool isExplored(int tileX, int tileY) const = 0;
    bool isExplored(TilePosition position) const;

    /** Returns true if the specified build tile has zerg creep on it. If the tile is concealed by fog of
     * war, the function will return false. */
    virtual bool hasCreep(int tileX, int tileY) const = 0;
    bool hasCreep(TilePosition position) const;

    /** Returns true if the given build location is powered by a nearby friendly pylon. */
    virtual bool hasPower(int tileX, int tileY, UnitType unitType = UnitTypes::None) const = 0;
    /** Returns true if the given build location is powered by a nearby friendly pylon. */
    virtual bool hasPower(TilePosition position, UnitType unitType = UnitTypes::None) const = 0;
    /** Returns true if the given build location is powered by a nearby friendly pylon. */
    virtual bool hasPower(int tileX, int tileY, int tileWidth, int tileHeight, UnitType unitType = UnitTypes::None) const = 0;
    /** Returns true if the given build location is powered by a nearby friendly pylon. */
    virtual bool hasPower(TilePosition position, int tileWidth, int tileHeight, UnitType unitType = UnitTypes::None) const = 0;
    /** Returns true if the given pixel location is powered by a nearby friendly pylon. */
    virtual bool hasPowerPrecise(int x, int y, UnitType unitType = UnitTypes::None ) const = 0;
    /** Returns true if the given pixel location is powered by a nearby friendly pylon. */
    virtual bool hasPowerPrecise(Position position, UnitType unitType = UnitTypes::None) const = 0;

    /** Returns true if the given unit type can be built at the given build tile position. Note the tile
     * position specifies the top left tile of the building. If builder is not null, the unit will be
     * discarded when determining whether or not any ground units are blocking the build location. */
    virtual bool canBuildHere(TilePosition position, UnitType type, const Unit *builder = nullptr, bool checkExplored = false) = 0;

    /** Returns true if the AI player has enough resources, supply, tech, and required units in order to
     * make the given unit type. If builder is not null, canMake will return true only if the builder unit
     * can build the given unit type. */
    virtual bool canMake(UnitType type, const Unit *builder = nullptr) = 0;

    /** Returns true if the AI player has enough resources required to research the given tech type. If unit
     * is not null, canResearch will return true only if the given unit can research the given tech type. */
    virtual bool canResearch(TechType type, const Unit *unit = nullptr) = 0;

    /** Returns true if the AI player has enough resources required to upgrade the given upgrade type. If
     * unit is not null, canUpgrade will return true only if the given unit can upgrade the given upgrade
     * type. */
    virtual bool canUpgrade(UpgradeType type, const Unit *unit = nullptr) = 0;

    /** Returns the set of starting locations for the given map. To determine the starting location for the
     * players in the current match, see Player::getStartLocation. */
    virtual const TilePosition::set& getStartLocations() const = 0;

    /** Prints text on the screen. Text is not sent to other players in multiplayer games. */
    virtual void printf(const char *format, ...) = 0;

    /** Sends text to other players - as if it were entered in chat. In single player games and replays,
     * this will just print the text on the screen. If the game is a single player match and not a replay,
     * then this function can be used to execute cheat codes, i.e. Broodwar->sendText("show me the money"). */
    virtual void sendText(const char *format, ...) = 0;
    virtual void sendTextEx(bool toAllies, const char *format, ...) = 0;

    /** Used to change the race while in a lobby. Note that there is no onLobbyEnter callback yet, so this
     * function cannot be used at this time. */
    virtual void changeRace(Race race) = 0;

    /** Returns true if Broodwar is in a game. Returns false for lobby and menu screens */
    virtual bool isInGame() const = 0;

    /** Returns true if Broodwar is in a multiplayer game. Returns false for single player games and
     * replays. */
    virtual bool isMultiplayer() const = 0;

    /** Returns true if Broodwar is in a BNet multiplayer game.
    */
    virtual bool isBattleNet() const = 0;

    /** Returns true if Broodwar is paused. If the game is paused, Game::getFrameCount will continue to
     * increase and AIModule::onFrame will still be called while paused. */
    virtual bool isPaused() const = 0;

    /** Returns true if Broodwar is in a replay. */
    virtual bool isReplay() const = 0;

    /** Used to start the game while in a lobby. Note that there is no onLobbyEnter callback yet, so this
     * function cannot be used at this time. */
    virtual void startGame() = 0;

    /** Pauses the game. If the game is paused, Game::getFrameCount will not increase however
     * AIModule::onFrame will still be called while paused. */
    virtual void pauseGame() = 0;

    /** Resumes the game. */
    virtual void resumeGame() = 0;

    /** Leaves the current match and goes to the after-game stats screen. */
    virtual void leaveGame() = 0;

    /** Restarts the match. Works the same way as if you restarted the match from the menu screen. Only
     * available in single player mode. */
    virtual void restartGame() = 0;

    /** Sets the speed of the game to the given number. Lower numbers are faster. 0 is the fastest speed
     * StarCraft can handle (which is about as fast as the fastest speed you can view a replay at). Any
     * negative value will reset the speed to the StarCraft default. */
    virtual void setLocalSpeed(int speed = -1) = 0;

    /** Issues a command to a group of units */
    virtual bool issueCommand(const Unitset& units, UnitCommand command) = 0;

    /** Returns the set of units currently selected by the user in the GUI. If Flag?::UserInput? was not
     * enabled during the AIModule::onStart callback, this function will always return an empty set. */
    virtual const Unitset& getSelectedUnits() = 0;

    /** Returns a pointer to the player that BWAPI controls. In replays this will return null. */
    virtual Player* self() = 0;

    /** Returns a pointer to the enemy player. If there is more than one enemy, this returns a pointer to
     * just one enemy (see enemies to get all enemies still in game). In replays this will
     * return nullptr. */
    virtual Player* enemy() = 0;

    /** Returns a pointer to the neutral player. */
    virtual Player* neutral() = 0;

    /** Returns a set of all the ally players that have not left or been defeated. Does not include self. */
    virtual Playerset& allies() = 0;

    /** Returns a set of all the enemy players that have not left or been defeated. */
    virtual Playerset& enemies() = 0;

    /** Returns a set of all the observer players that have not left. */
    virtual Playerset& observers() = 0;

    virtual void setTextSize(int size = 1) = 0;
    /** Draws text on the screen at the given position. Text can be drawn in different colors by using the
     * following control characters: TODO: add image from wiki.*/
    virtual void drawText(int ctype, int x, int y, const char *format, ...) = 0;
    virtual void drawTextMap(int x, int y, const char *format, ...) = 0;
    virtual void drawTextMouse(int x, int y, const char *format, ...) = 0;
    virtual void drawTextScreen(int x, int y, const char *format, ...) = 0;

    /** Draws a box on the screen, with the given color. If isSolid is true, the entire box will be
     * rendered, otherwise just the outline will be drawn. */
    virtual void drawBox(int ctype, int left, int top, int right, int bottom, Color color, bool isSolid = false) = 0;
    void drawBoxMap(int left, int top, int right, int bottom, Color color, bool isSolid = false);
    void drawBoxMouse(int left, int top, int right, int bottom, Color color, bool isSolid = false);
    void drawBoxScreen(int left, int top, int right, int bottom, Color color, bool isSolid = false);

    /** Draws a triangle on the screen. If isSolid is true, a solid triangle is drawn, otherwise just the
     * outline of the triangle will be drawn. */
    virtual void drawTriangle(int ctype, int ax, int ay, int bx, int by, int cx, int cy, Color color, bool isSolid = false) = 0;
    void drawTriangleMap(int ax, int ay, int bx, int by, int cx, int cy, Color color, bool isSolid = false);
    void drawTriangleMouse(int ax, int ay, int bx, int by, int cx, int cy, Color color, bool isSolid = false);
    void drawTriangleScreen(int ax, int ay, int bx, int by, int cx, int cy, Color color, bool isSolid = false);
    void drawTriangleMap(Position a, Position b, Position c, Color color, bool isSolid = false);
    void drawTriangleMouse(Position a, Position b, Position c, Color color, bool isSolid = false);
    void drawTriangleScreen(Position a, Position b, Position c, Color color, bool isSolid = false);

    /** Draws a circle on the screen, with the given color. If isSolid is true, a solid circle is drawn,
     * otherwise just the outline of a circle will be drawn. */
    virtual void drawCircle(int ctype, int x, int y, int radius, Color color, bool isSolid = false) = 0;
    void drawCircleMap(int x, int y, int radius, Color color, bool isSolid = false);
    void drawCircleMouse(int x, int y, int radius, Color color, bool isSolid = false);
    void drawCircleScreen(int x, int y, int radius, Color color, bool isSolid = false);
    void drawCircleMap(Position p, int radius, Color color, bool isSolid = false);
    void drawCircleMouse(Position p, int radius, Color color, bool isSolid = false);
    void drawCircleScreen(Position p, int radius, Color color, bool isSolid = false);

    /** Draws an ellipse on the screen, with the given color. If isSolid is true, a solid ellipse is drawn,
     * otherwise just the outline of an ellipse will be drawn. */
    virtual void drawEllipse(int ctype, int x, int y, int xrad, int yrad, Color color, bool isSolid = false) = 0;
    void drawEllipseMap(int x, int y, int xrad, int yrad, Color color, bool isSolid = false);
    void drawEllipseMouse(int x, int y, int xrad, int yrad, Color color, bool isSolid = false);
    void drawEllipseScreen(int x, int y, int xrad, int yrad, Color color, bool isSolid = false);
    void drawEllipseMap(Position p, int xrad, int yrad, Color color, bool isSolid = false);
    void drawEllipseMouse(Position p, int xrad, int yrad, Color color, bool isSolid = false);
    void drawEllipseScreen(Position p, int xrad, int yrad, Color color, bool isSolid = false);

    /** Draws a dot on the screen at the given position with the given color. */
    virtual void drawDot(int ctype, int x, int y, Color color) = 0;
    void drawDotMap(int x, int y, Color color);
    void drawDotMouse(int x, int y, Color color);
    void drawDotScreen(int x, int y, Color color);
    void drawDotMap(Position p, Color color);
    void drawDotMouse(Position p, Color color);
    void drawDotScreen(Position p, Color color);

    /** Draws a line on the screen from (x1,y1) to (x2,y2) with the given color. */
    virtual void drawLine(int ctype, int x1, int y1, int x2, int y2, Color color) = 0;
    void drawLineMap(int x1, int y1, int x2, int y2, Color color);
    void drawLineMouse(int x1, int y1, int x2, int y2, Color color);
    void drawLineScreen(int x1, int y1, int x2, int y2, Color color);
    void drawLineMap(Position a, Position b, Color color);
    void drawLineMouse(Position a, Position b, Color color);
    void drawLineScreen(Position a, Position b, Color color);

    /** Retrieves the screen buffer for the game (excluding the HUD) */
    virtual void *getScreenBuffer() = 0;

    /** Retrieves latency values for the game. Includes latency, speed, and mode */
    virtual int getLatencyFrames() = 0;
    virtual int getLatencyTime() = 0;
    virtual int getRemainingLatencyFrames() = 0;
    virtual int getRemainingLatencyTime() = 0;

    /** Retrieves the current revision of BWAPI. */
    virtual int getRevision() = 0;

    /** Retrieves the debug state of the BWAPI build. */
    virtual bool isDebug() = 0;

    /** Returns true if latency compensation is enabled */
    virtual bool isLatComEnabled() = 0;

    /** Use to enable or disable latency compensation. Default: Enabled */
    virtual void setLatCom(bool isEnabled) = 0;

    /** Sets the rendering state of the Starcraft GUI */
    virtual void setGUI(bool enabled = true) = 0;

    /** Retrieves the instance number recorded by BWAPI to identify which instance an AI module belongs to */
    virtual int  getInstanceNumber() = 0;

    /** Retrieves the bot's APM. Can include or exclude select commands. */
    virtual int getAPM(bool includeSelects = false) = 0;

    /** Changes the map to the one specified. Changes do not take effect unless the game is restarted. */
    virtual bool setMap(const char *mapFileName) = 0;

    /** Sets the frame skip value. 1 = normal */
    virtual void setFrameSkip(int frameSkip = 1) = 0;

    /** Returns true if Starcraft can find a path from the source to the destination. */
    virtual bool hasPath(Position source, Position destination) const = 0;

    /** Sets the BWAPI player's alliance with another player. */
    virtual bool setAlliance(BWAPI::Player *player, bool allied = true, bool alliedVictory = true) = 0;

    /** Sets the BWAPI player's vision with another player. */
    virtual bool setVision(BWAPI::Player *player, bool enabled = true) = 0;

    /** Returns the elapsed game time in seconds. */
    virtual int  elapsedTime() const = 0;

    /** Sets the level of command optimizations.
        0 = No optimization.
        1 = Some optimization    (Stop, Hold Position, Siege, Burrow, etc.).
        2 = More optimization    (Train, Set Rally, Lift, [multi-select buildings]).
        3 = Maximum optimization (Attack/Move to position, use ability at position, etc.).*/
    virtual void setCommandOptimizationLevel(int level = 0) = 0;

    /** Returns the remaining countdown time in seconds. */
    virtual int  countdownTimer() const = 0;

    /** Returns the set of all map regions. */
    virtual const Regionset &getAllRegions() const = 0;

    /** Returns the region at a position. */
    virtual BWAPI::Region *getRegionAt(int x, int y) const = 0;
    BWAPI::Region *getRegionAt(BWAPI::Position position) const;

    /** Returns the time taken to perform the previous event call. Used for tournament management. */
    virtual int getLastEventTime() const = 0;

    /** Hides or reveals a player in a replay. */
    virtual bool setReplayVision(BWAPI::Player *player, bool enabled = true) = 0;

    /** Enables or disables the Fog of War in a replay. */
    virtual bool setRevealAll(bool reveal = true) = 0;
  };
  extern Game* Broodwar;
}
