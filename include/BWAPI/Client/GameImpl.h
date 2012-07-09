#pragma once
#include <BWAPI.h>
#include "GameData.h"
#include "Client.h"
#include "Shape.h"
#include "Command.h"
#include "UnitCommand.h"
#include "ForceImpl.h"
#include "PlayerImpl.h"
#include "RegionImpl.h"
#include "UnitImpl.h"
#include "BulletImpl.h"
#include <list>
#include <vector>

#include <BWAPI/Unitset.h>
#include <BWAPI/Bulletset.h>
#include <BWAPI/Playerset.h>
#include <BWAPI/Forceset.h>
#include <BWAPI/Regionset.h>

namespace BWAPI
{
  class Force;
  class Player;
  class Unit;

  class GameImpl : public Game
  {
    private :
      int addShape(const BWAPIC::Shape &s);
      int addString(const char* text);
      int addText(BWAPIC::Shape &s, const char* text);
      int addCommand(const BWAPIC::Command &c);
      void clearAll();

      GameData* data;
      std::vector<ForceImpl>  forceVector;
      std::vector<PlayerImpl> playerVector;
      std::vector<UnitImpl>   unitVector;
      std::vector<BulletImpl> bulletVector;
      RegionImpl *regionArray[5000];

      Forceset forces;
      Playerset playerSet;
      Unitset accessibleUnits;//all units that are accessible (and definitely alive)
      //notDestroyedUnits - accessibleUnits = all units that may or may not be alive (status unknown)
      Unitset minerals;
      Unitset geysers;
      Unitset neutralUnits;
      Unitset staticMinerals;
      Unitset staticGeysers;
      Unitset staticNeutralUnits;
      Bulletset bullets;
      Position::set nukeDots;
      Unitset selectedUnits;
      Unitset pylons;
      Regionset regionsList;

      TilePosition::set startLocations;
      std::list< Event > events;
      bool flagEnabled[2];
      Player* thePlayer;
      Player* theEnemy;
      Player* theNeutral;
      Playerset _allies;
      Playerset _enemies;
      Playerset _observers;
      Error lastError;
      int textSize;

    public :
      Event makeEvent(BWAPIC::Event e);
      int addUnitCommand(BWAPIC::UnitCommand& c);
      bool inGame;
      GameImpl(GameData* data);
      void onMatchStart();
      void onMatchEnd();
      void onMatchFrame();
      const GameData* getGameData() const;
      Unit *_unitFromIndex(int index);

      virtual const Forceset& getForces() const;
      virtual const Playerset& getPlayers() const;
      virtual const Unitset& getAllUnits() const;
      virtual const Unitset& getMinerals() const;
      virtual const Unitset& getGeysers() const;
      virtual const Unitset& getNeutralUnits() const;

      virtual const Unitset& getStaticMinerals() const;
      virtual const Unitset& getStaticGeysers() const;
      virtual const Unitset& getStaticNeutralUnits() const;

      virtual const Bulletset& getBullets() const;
      virtual const Position::set& getNukeDots() const;
      virtual const std::list< Event>& getEvents() const;

      virtual Force*  getForce(int forceID) const;
      virtual Player* getPlayer(int playerID) const;
      virtual Unit*   getUnit(int unitID) const;
      virtual Unit*   indexToUnit(int unitIndex) const;
      virtual Region* getRegion(int regionID) const;

      virtual GameType getGameType() const;
      virtual int getLatency() const;
      virtual int getFrameCount() const;
      virtual int getReplayFrameCount() const;
      virtual int getFPS() const;
      virtual double getAverageFPS() const;
      virtual BWAPI::Position getMousePosition() const;
      virtual bool getMouseState(MouseButton button) const;
      virtual bool getMouseState(int button) const;
      virtual bool getKeyState(Key key) const;
      virtual bool getKeyState(int key) const;
      virtual BWAPI::Position getScreenPosition() const;
      virtual void setScreenPosition(int x, int y);
      virtual void pingMinimap(int x, int y);

      virtual bool      isFlagEnabled(int flag) const;
      virtual void      enableFlag(int flag);
      virtual Unitset   getUnitsInRectangle(int left, int top, int right, int bottom, const UnitFilter &pred = nullptr) const;
      virtual Unit      *getClosestUnitInRectangle(Position center, const UnitFilter &pred = nullptr, int left = 0, int top = 0, int right = 999999, int bottom = 999999) const;
      virtual Unit      *getBestUnit(const BestUnitFilter &best, const UnitFilter &pred, Position center = Positions::None, int radius = 999999) const;
      virtual Error     getLastError() const;
      virtual bool      setLastError(BWAPI::Error e = Errors::None);

      virtual int         mapWidth() const;
      virtual int         mapHeight() const;
      virtual std::string mapFileName() const;
      virtual std::string mapPathName() const;
      virtual std::string mapName() const;
      virtual std::string mapHash() const;

      virtual bool isWalkable(int x, int y) const;
      virtual int  getGroundHeight(int x, int y) const;
      virtual bool isBuildable(int x, int y, bool includeBuildings = false) const;
      virtual bool isVisible(int x, int y) const;
      virtual bool isExplored(int x, int y) const;
      virtual bool hasCreep(int x, int y) const;
      virtual bool hasPower(int tileX, int tileY, UnitType unitType = UnitTypes::None) const;
      virtual bool hasPower(TilePosition position, UnitType unitType = UnitTypes::None) const;
      virtual bool hasPower(int tileX, int tileY, int tileWidth, int tileHeight, UnitType unitType = UnitTypes::None) const;
      virtual bool hasPower(TilePosition position, int tileWidth, int tileHeight, UnitType unitType = UnitTypes::None) const;
      virtual bool hasPowerPrecise(int x, int y, UnitType unitType = UnitTypes::None ) const;
      virtual bool hasPowerPrecise(Position position, UnitType unitType = UnitTypes::None) const;

      virtual bool canBuildHere(TilePosition position, UnitType type, const Unit* builder = nullptr, bool checkExplored = false);
      virtual bool canMake(UnitType type, const Unit* builder = nullptr);
      virtual bool canResearch(TechType type, const Unit* unit = nullptr);
      virtual bool canUpgrade(UpgradeType type, const Unit* unit = nullptr);
      virtual const TilePosition::set& getStartLocations() const;

      virtual void printf(const char* format, ...);
      virtual void sendText(const char* format, ...);
      virtual void sendTextEx(bool toAllies, const char *format, ...);

      virtual void changeRace(BWAPI::Race race);
      virtual bool isInGame() const;
      virtual bool isMultiplayer() const;
      virtual bool isBattleNet() const;
      virtual bool isPaused() const;
      virtual bool isReplay() const;

      virtual void startGame();
      virtual void pauseGame();
      virtual void resumeGame();
      virtual void leaveGame();
      virtual void restartGame();
      virtual void setLocalSpeed(int speed = -1);
      virtual bool issueCommand(const Unitset& units, UnitCommand command);
      virtual const Unitset& getSelectedUnits();
      virtual Player* self();
      virtual Player* enemy();
      virtual Player* neutral();
      virtual Playerset& allies();
      virtual Playerset& enemies();
      virtual Playerset& observers();

      virtual void setTextSize(int size = 1);
      virtual void drawText(int ctype, int x, int y, const char *format, ...);
      virtual void drawTextMap(int x, int y, const char *format, ...);
      virtual void drawTextMouse(int x, int y, const char *format, ...);
      virtual void drawTextScreen(int x, int y, const char *format, ...);

      virtual void drawBox(int ctype, int left, int top, int right, int bottom, Color color, bool isSolid = false);
      virtual void drawTriangle(int ctype, int ax, int ay, int bx, int by, int cx, int cy, Color color, bool isSolid = false);
      virtual void drawCircle(int ctype, int x, int y, int radius, Color color, bool isSolid = false);
      virtual void drawEllipse(int ctype, int x, int y, int xrad, int yrad, Color color, bool isSolid = false);
      virtual void drawDot(int ctype, int x, int y, Color color);
      virtual void drawLine(int ctype, int x1, int y1, int x2, int y2, Color color);

      virtual void *getScreenBuffer();
      virtual int  getLatencyFrames();
      virtual int  getLatencyTime();
      virtual int  getRemainingLatencyFrames();
      virtual int  getRemainingLatencyTime();
      virtual int  getRevision();
      virtual bool isDebug();
      virtual bool isLatComEnabled();
      virtual void setLatCom(bool isEnabled);
      virtual bool isGUIEnabled();
      virtual void setGUI(bool enabled = true);
      virtual int  getInstanceNumber();
      virtual int  getAPM(bool includeSelects = false);
      virtual bool setMap(const char *mapFileName);
      virtual void setFrameSkip(int frameSkip = 1);
      virtual bool hasPath(Position source, Position destination) const;
      virtual bool setAlliance(BWAPI::Player *player, bool allied = true, bool alliedVictory = true);
      virtual bool setVision(BWAPI::Player *player, bool enabled = true);
      virtual int  elapsedTime() const;
      virtual void setCommandOptimizationLevel(int level = 0);
      virtual int  countdownTimer() const;
      virtual const Regionset &getAllRegions() const;
      virtual BWAPI::Region *getRegionAt(int x, int y) const;
      virtual int getLastEventTime() const;
      virtual bool setReplayVision(BWAPI::Player *player, bool enabled = true);
      virtual bool setRevealAll(bool reveal = true);
  };
}
