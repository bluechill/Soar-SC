#pragma once
#include <functional>

namespace BWAPI
{
  template < typename T >
  class InterfaceEvent
  {
  public:
    // default ctor
    InterfaceEvent()
      : condProc(nullptr)
      , execProc(nullptr)
      , runCount(0)
      , runFreq(0)
      , lastFrame(0)
    {};

    // expected ctor
    InterfaceEvent(const std::function<void(T*)> &action, const std::function<bool(T*)> &condition = nullptr, int timesToRun = -1, int framesToCheck = 0)
      : condProc( condition )
      , execProc( action )
      , runCount( timesToRun )
      , runFreq( framesToCheck )
      , lastFrame( -100000 )
    {};
    // copy ctor
    InterfaceEvent(const InterfaceEvent<T> &other)
      : condProc( other.condProc )
      , execProc( other.execProc )
      , runFreq( other.runFreq )
      , runCount( other.runCount )
      , lastFrame( other.lastFrame )
    {};
    // move ctor
    InterfaceEvent(InterfaceEvent<T> &&other)
      : condProc( std::move(other.condProc) )
      , execProc( std::move(other.execProc) )
      , runFreq( other.runFreq )
      , runCount( other.runCount )
      , lastFrame( other.lastFrame )
    {};
    // copy assignment
    InterfaceEvent &operator =(const InterfaceEvent<T> &other)
    {
      condProc  = other.condProc;
      execProc  = other.execProc;
      runFreq   = other.runFreq;
      runCount  = other.runCount;
      lastFrame = other.lastFrame;
      return *this;
    };
    // move assignment
    InterfaceEvent &operator =(InterfaceEvent<T> &&other)
    {
      condProc  = std::move(other.condProc);
      execProc  = std::move(other.execProc);
      runFreq   = other.runFreq;
      runCount  = other.runCount;
      lastFrame = other.lastFrame;
      return *this;
    };

    // dtor
    virtual ~InterfaceEvent()
    {
    };

    /// Checks if the event has finished its execution and is marked for removal.
    ///
    /// @retval true If the event has completed all runs and/or is marked for removal.
    /// @retval false If the event should continue execution.
    bool isFinished()
    {
      return this->runCount == 0;
    };

    /// Marks the event for removal.
    void removeEvent()
    {
      this->runCount = 0;
    };

  protected:
    template < typename U >
    friend class Interface;

    // Function that runs the event, checkings its conditions and running its action, then
    // decrementing the run count.
    bool execute(T *instance, int currentFrame)
    {
      // condition check
      if ( !this->isConditionMet(instance, currentFrame) )
        return false;

      // There must be an exec proc!
      if ( !this->execProc )
        return false;

      // execute
      this->execProc(instance);

      // Decrement run count (-1 being infinite)
      if ( this->runCount > 0 )
        --this->runCount;
      return true;
    };

    // Function to check if the condition associated with the event is true. Includes frame and
    // run count checking.
    bool isConditionMet(T *instance, int currentFrame)
    {
      // Validity check
      if ( this->isFinished() )
        return false;

      // Frame check
      if ( this->lastFrame + this->runFreq >= currentFrame )
        return false;
      this->lastFrame = currentFrame;

      // Conditional check
      if ( this->condProc )
        return this->condProc(instance);
      return true; // always run if there is no conditional function
    };

  private:
    // Data members
    std::function<bool(T*)> condProc;
    std::function<void(T*)> execProc;
    int runFreq;  // Frequency of runs, in frames (0 means every frame, 1 means every other frame)
    int runCount; // Number of times that the action can occur (-1 being infinite)
    int lastFrame;
  };

}
