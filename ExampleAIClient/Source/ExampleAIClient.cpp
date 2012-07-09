#include <stdio.h>

#include <BWAPI.h>
#include <BWAPI/Client.h>
//#include <BWTA.h>

#include <windows.h>

#include <string>

using namespace BWAPI;

void drawStats();
void drawBullets();
void drawVisibilityData();
//void drawTerrainData();
void showPlayers();
void showForces();
bool show_bullets;
bool show_visibility_data;
/*
bool analyzed;
bool analysis_just_finished;
BWTA::Region* home;
BWTA::Region* enemy_base;
DWORD WINAPI AnalyzeThread();
*/
void reconnect()
{
  while(!BWAPIClient.connect())
  {
    Sleep(1000);
  }
}
int main(int argc, const char* argv[])
{
  BWAPI::BWAPI_init();
  printf("Connecting...");
  reconnect();
  while(true)
  {
    printf("waiting to enter match\n");
    while (!Broodwar->isInGame())
    {
      BWAPI::BWAPIClient.update();
      if (!BWAPI::BWAPIClient.isConnected())
      {
        printf("Reconnecting...\n");
        reconnect();
      }
    }
    printf("starting match!\n");
    Broodwar->sendText("Hello world!");
    Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
    // Enable some cheat flags
    Broodwar->enableFlag(Flag::UserInput);
    // Uncomment to enable complete map information
    //Broodwar->enableFlag(Flag::CompleteMapInformation);
    /*
    //read map information into BWTA so terrain analysis can be done in another thread
    BWTA::readMap();
    analyzed=false;
    analysis_just_finished=false;
    */
    show_bullets=false;
    show_visibility_data=false;

    if (Broodwar->isReplay())
    {
      Broodwar->printf("The following players are in this replay:");
      Playerset players = Broodwar->getPlayers();
      for(Playerset::iterator p = players.begin(); p != players.end(); ++p )
      {
        if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
          Broodwar->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
      }
    }
    else
    {
      Broodwar->printf("The match up is %s v %s",
        Broodwar->self()->getRace().getName().c_str(),
        Broodwar->enemy()->getRace().getName().c_str());

      //send each worker to the mineral field that is closest to it
      Unitset units    = Broodwar->self()->getUnits();
      Unitset minerals  = Broodwar->getMinerals();
      for ( Unitset::iterator i = units.begin(); i != units.end(); ++i )
      {
        if ( i->getType().isWorker() )
        {
          Unit* closestMineral = NULL;

          for( Unitset::iterator m = minerals.begin(); m != minerals.end(); ++m )
          {
            if ( !closestMineral || i->getDistance(*m) < i->getDistance(closestMineral))
              closestMineral = *m;
          }
          if ( closestMineral )
            i->rightClick(closestMineral);
        }
        else if ( i->getType().isResourceDepot() )
        {
          //if this is a center, tell it to build the appropiate type of worker
          i->train(Broodwar->self()->getRace().getWorker());
        }
      }
    }
    while(Broodwar->isInGame())
    {
      for(std::list<Event>::const_iterator e = Broodwar->getEvents().begin(); e != Broodwar->getEvents().end(); ++e)
      {
        switch(e->getType())
        {
          case EventType::MatchEnd:
            if (e->isWinner())
              printf("I won the game\n");
            else
              printf("I didn't win the game\n");
            break;
          case EventType::SendText:
            if (e->getText()=="/show bullets")
            {
              show_bullets=!show_bullets;
            } else if (e->getText()=="/show players")
            {
              showPlayers();
            } else if (e->getText()=="/show forces")
            {
              showForces();
            } else if (e->getText()=="/show visibility")
            {
              show_visibility_data=!show_visibility_data;
            } /* else if (e->text=="/analyze")
            {
              if (analyzed == false)
              {
                Broodwar->printf("Analyzing map... this may take a minute");
                CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
              }
            } */ else
            {
              Broodwar->printf("You typed '%s'!",e->getText().c_str());
            }
            break;
          case EventType::ReceiveText:
            Broodwar->printf("%s said '%s'", e->getPlayer()->getName().c_str(), e->getText().c_str());
            break;
          case EventType::PlayerLeft:
            Broodwar->sendText("%s left the game.",e->getPlayer()->getName().c_str());
            break;
          case EventType::NukeDetect:
            if (e->getPosition()!=Positions::Unknown)
            {
              Broodwar->drawCircleMap(e->getPosition(), 40, Colors::Red, true);
              Broodwar->printf("Nuclear Launch Detected at (%d,%d)",e->getPosition().x,e->getPosition().y);
            }
            else
              Broodwar->printf("Nuclear Launch Detected");
            break;
          case EventType::UnitCreate:
            if (!Broodwar->isReplay())
              Broodwar->sendText("A %s [%x] has been created at (%d,%d)",e->getUnit()->getType().getName().c_str(),e->getUnit(),e->getUnit()->getPosition().x,e->getUnit()->getPosition().y);
            else
            {
              /*if we are in a replay, then we will print out the build order
              (just of the buildings, not the units).*/
              if (e->getUnit()->getType().isBuilding() && e->getUnit()->getPlayer()->isNeutral()==false)
              {
                int seconds=Broodwar->getFrameCount()/24;
                int minutes=seconds/60;
                seconds%=60;
                Broodwar->sendText("%.2d:%.2d: %s creates a %s",minutes,seconds,e->getUnit()->getPlayer()->getName().c_str(),e->getUnit()->getType().getName().c_str());
              }
            }
            break;
          case EventType::UnitDestroy:
            if (!Broodwar->isReplay())
              Broodwar->sendText("A %s [%x] has been destroyed at (%d,%d)",e->getUnit()->getType().getName().c_str(),e->getUnit(),e->getUnit()->getPosition().x,e->getUnit()->getPosition().y);
            break;
          case EventType::UnitMorph:
            if (!Broodwar->isReplay())
              Broodwar->sendText("A %s [%x] has been morphed at (%d,%d)",e->getUnit()->getType().getName().c_str(),e->getUnit(),e->getUnit()->getPosition().x,e->getUnit()->getPosition().y);
            else
            {
              /*if we are in a replay, then we will print out the build order
              (just of the buildings, not the units).*/
              if (e->getUnit()->getType().isBuilding() && e->getUnit()->getPlayer()->isNeutral()==false)
              {
                int seconds=Broodwar->getFrameCount()/24;
                int minutes=seconds/60;
                seconds%=60;
                Broodwar->sendText("%.2d:%.2d: %s morphs a %s",minutes,seconds,e->getUnit()->getPlayer()->getName().c_str(),e->getUnit()->getType().getName().c_str());
              }
            }
            break;
          case EventType::UnitShow:
            if (!Broodwar->isReplay())
              Broodwar->sendText("A %s [%x] has been spotted at (%d,%d)",e->getUnit()->getType().getName().c_str(),e->getUnit(),e->getUnit()->getPosition().x,e->getUnit()->getPosition().y);
            break;
          case EventType::UnitHide:
            if (!Broodwar->isReplay())
              Broodwar->sendText("A %s [%x] was last seen at (%d,%d)",e->getUnit()->getType().getName().c_str(),e->getUnit(),e->getUnit()->getPosition().x,e->getUnit()->getPosition().y);
            break;
          case EventType::UnitRenegade:
            if (!Broodwar->isReplay())
              Broodwar->sendText("A %s [%x] is now owned by %s",e->getUnit()->getType().getName().c_str(),e->getUnit(),e->getUnit()->getPlayer()->getName().c_str());
            break;
          case EventType::SaveGame:
            Broodwar->sendText("The game was saved to \"%s\".",e->getText().c_str());
            break;
        }
      }

      if (show_bullets)
        drawBullets();

      if (show_visibility_data)
        drawVisibilityData();

      drawStats();
      Broodwar->drawTextScreen(300,0,"FPS: %f",Broodwar->getAverageFPS());
      /*
      if (analyzed && Broodwar->getFrameCount()%30==0)
      {
        //order one of our workers to guard our chokepoint.
        for(Unitset::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();++i)
        {
          if ((*i)->getType().isWorker())
          {
            //get the chokepoints linked to our home region
            std::set<BWTA::Chokepoint*> chokepoints= home->getChokepoints();
            double min_length=10000;
            BWTA::Chokepoint* choke=NULL;

            //iterate through all chokepoints and look for the one with the smallest gap (least width)
            for(std::set<BWTA::Chokepoint*>::iterator c=chokepoints.begin();c!=chokepoints.end();++c)
            {
              double length=(*c)->getWidth();
              if (length<min_length || choke==NULL)
              {
                min_length=length;
                choke=*c;
              }
            }

            //order the worker to move to the center of the gap
            (*i)->rightClick(choke->getCenter());
            break;
          }
        }
      }
      if (analysis_just_finished)
      {
        Broodwar->printf("Finished analyzing map.");
        analysis_just_finished=false;
      }
      if (analyzed)
        drawTerrainData();
      */
      BWAPI::BWAPIClient.update();
      if (!BWAPI::BWAPIClient.isConnected())
      {
        printf("Reconnecting...\n");
        reconnect();
      }
    }
    printf("Game ended\n");
  }
  system("pause");
  return 0;
}
/*
DWORD WINAPI AnalyzeThread()
{
  BWTA::analyze();

  //self start location only available if the map has base locations
  if (BWTA::getStartLocation(BWAPI::Broodwar->self())!=NULL)
  {
    home       = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
  }
  //enemy start location only available if Complete Map Information is enabled.
  if (BWTA::getStartLocation(BWAPI::Broodwar->enemy())!=NULL)
  {
    enemy_base = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
  }
  analyzed   = true;
  analysis_just_finished = true;
  return 0;
}
*/
void drawStats()
{
  int line = 0;
  Broodwar->drawTextScreen(5, 0, "I have %d units:", Broodwar->self()->allUnitCount(UnitTypes::AllUnits) );
  for ( UnitType::set::iterator i = UnitTypes::allUnitTypes().begin(); i != UnitTypes::allUnitTypes().end(); ++i )
  {
    int count = Broodwar->self()->allUnitCount(*i);
    if ( count )
    {
      Broodwar->drawTextScreen(5, 16*line, "- %d %s%c", count, (*i).c_str(), count == 1 ? ' ' : 's');
      ++line;
    }
  }
}

void drawBullets()
{
  Bulletset bullets = Broodwar->getBullets();
  for(Bulletset::iterator i = bullets.begin(); i != bullets.end(); ++i)
  {
    Position p = i->getPosition();
    double velocityX = i->getVelocityX();
    double velocityY = i->getVelocityY();
    Broodwar->drawLineMap(p, p + Position((int)velocityX, (int)velocityY), i->getPlayer() == Broodwar->self() ? Colors::Green : Colors::Red);
    Broodwar->drawTextMap(p.x, p.y, "%c%s", i->getPlayer() == Broodwar->self() ? Text::Green : Text::Red, i->getType().c_str());
  }
}

void drawVisibilityData()
{
  for( TilePosition::iterator i(Broodwar->mapWidth(), Broodwar->mapHeight()); i; ++i)
  {
    if ( Broodwar->isExplored(i.x, i.y) )
      Broodwar->drawDotMap(i.x*32+16, i.y*32+16, Broodwar->isVisible(i.x, i.y) ? Colors::Green : Colors::Blue);
    else
      Broodwar->drawDotMap(i.x*32+16, i.y*32+16, Colors::Red);
  }
}
/*
void drawTerrainData()
{
  //we will iterate through all the base locations, and draw their outlines.
  for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();++i)
  {
    TilePosition p=(*i)->getTilePosition();
    Position c=(*i)->getPosition();

    //draw outline of center location
    Broodwar->drawBoxMap(p, p + TilePosition(4,3), Colors::Blue);

    //draw a circle at each mineral patch
    for(Unitset::iterator j=(*i)->getStaticMinerals().begin();j!=(*i)->getStaticMinerals().end();++j)
    {
      Position q=(*j)->getInitialPosition();
      Broodwar->drawCircleMap(q, 30, Colors::Cyan);
    }

    //draw the outlines of vespene geysers
    for(Unitset::iterator j=(*i)->getGeysers().begin();j!=(*i)->getGeysers().end();++j)
    {
      TilePosition q=(*j)->getInitialTilePosition();
      Broodwar->drawBoxMap(q, q + TilePosition(4,2), Colors::Orange);
    }

    //if this is an island expansion, draw a yellow circle around the base location
    if ((*i)->isIsland())
      Broodwar->drawCircleMap(c, 80, Colors::Yellow);
  }

  //we will iterate through all the regions and draw the polygon outline of it in green.
  for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();++r)
  {
    BWTA::Polygon p=(*r)->getPolygon();
    for(int j=0;j<(int)p.size();++j)
    {
      Position point1=p[j];
      Position point2=p[(j+1) % p.size()];
      Broodwar->drawLineMap(point1, point2, Colors::Green);
    }
  }

  //we will visualize the chokepoints with red lines
  for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();++r)
  {
    for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();++c)
    {
      Position point1=(*c)->getSides().first;
      Position point2=(*c)->getSides().second;
      Broodwar->drawLineMap(point1, point2, Colors::Red);
    }
  }
}
*/
void showPlayers()
{
  Playerset players = Broodwar->getPlayers();
  for(Playerset::iterator i = players.begin(); i != players.end(); ++i)
    Broodwar->printf("Player [%d]: %s is in force: %s", i->getID(), i->getName().c_str(), i->getForce()->getName().c_str());
}

void showForces()
{
  Forceset forces=Broodwar->getForces();
  for(Forceset::iterator i = forces.begin(); i != forces.end(); ++i)
  {
    Playerset players = i->getPlayers();
    Broodwar->printf("Force %s has the following players:", i->getName().c_str());
    for(Playerset::iterator j = players.begin(); j != players.end(); ++j)
      Broodwar->printf("  - Player [%d]: %s", j->getID(), j->getName().c_str());
  }
}
