#pragma once
#include <BWAPI/Vectorset.h>

namespace BWAPI
{
  // Forward Declarations
  class Force;
  class Playerset;

  /// @~English
  /// A container that holds a group of Forces.
  ///
  /// @~
  /// @see BWAPI::Force, BWAPI::Vectorset
  class Forceset : public Vectorset<BWAPI::Force*>
  {
  public:
    Forceset(size_t initialSize = 5);
    Forceset(const Forceset &other);
    Forceset(Forceset &&other);

    /// @copydoc Force::getPlayers
    Playerset getPlayers() const;
  };
}

