#pragma once
#include <BWAPI/Type.h>

namespace BWAPI
{
  /// @~English
  /// The Color object is used in drawing routines to 
  /// specify the color to use.
  ///
  /// @note Starcraft uses a 256 color palette for rendering.
  /// Thus, the colors available are limited to this palette.
  /// @~
  class Color : public Type<Color, 255>
  {
    public:
      /// @~English
      /// A constructor that uses the color at the specified
      /// palette index.
      ///
      /// @param id The index of the color in the palette.
      /// @~
      Color(int id = 0);

      /// @~English
      /// A constructor that uses the color index in the
      /// palette that is closest to the given rgb values.
      /// On its first call, the colors in the palette
      /// will be sorted for fast indexing.
      ///
      /// @note This function computes the distance of
      /// the RGB values and may not be accurate.
      ///
      /// @param red The amount of red.
      /// @param green The amount of green.
      /// @param blue The amount of blue.
      /// @~
      Color(int red, int green, int blue);

      /// @~English
      /// Retrieves the red component of the color.
      ///
      /// @returns integer containing the value of
      /// the red component.
      /// @~
      int red() const;

      /// @~English
      /// Retrieves the green component of the color.
      ///
      /// @returns integer containing the value of
      /// the green component.
      /// @~
      int green() const;

      /// @~English
      /// Retrieves the blue component of the color.
      ///
      /// @returns integer containing the value of
      /// the blue component.
      /// @~
      int blue() const;
  };

  /// @~English
  /// This namespace contains known colors.
  /// @~
  namespace Colors
  {
    /// @~English
    /// The default color for Player 1.
    /// @~
    extern const Color Red;

    /// @~English
    /// The default color for Player 2.
    /// @~
    extern const Color Blue;

    /// @~English
    /// The default color for Player 3.
    /// @~
    extern const Color Teal;

    /// @~English
    /// The default color for Player 4.
    /// @~
    extern const Color Purple;

    /// @~English
    /// The default color for Player 5.
    /// @~
    extern const Color Orange;

    /// @~English
    /// The default color for Player 6.
    /// @~
    extern const Color Brown;

    /// @~English
    /// The default color for Player 7.
    /// @~
    extern const Color White;

    /// @~English
    /// The default color for Player 8.
    /// @~
    extern const Color Yellow;

    /// @~English
    /// The alternate color for Player 7 on Ice tilesets.
    /// @~
    extern const Color Green;

    /// @~English
    /// The default color for Neutral (Player 12).
    /// @~
    extern const Color Cyan;
    
    extern const Color Black;
    extern const Color Grey;
  }
  /// @~English
  /// This namespace contains text formatting codes.
  /// @~
  namespace Text
  {
    /// @~English
    /// This enumeration contains text formatting codes.
    /// Such codes are used in calls to Game::drawText
    /// and Game::printf.
    /// @~
    enum Enum
    {
      /// @~English
      /// Uses the previous color that was specified
      /// before the current one.
      /// @~
      Previous    = 1,
      /// @~English
      /// Uses the default blueish color. This color
      /// is used in standard game messages.
      /// @~
      Default      = 2,
      /// @~English
      /// A solid yellow. This yellow is used
      /// in notifications and is also the
      /// default color when using Game::printf.
      /// @~
      Yellow      = 3,
      /// @~English
      /// A bright white. This is used for timers.
      /// @~
      White      = 4,
      /// @~English
      /// A dark grey. This color code will
      /// override all color formatting
      /// that follows.
      /// @~
      Grey      = 5,
      /// @~English
      /// A deep red. This color code is
      /// used for error messages.
      /// @~
      Red        = 6,
      /// @~English
      /// A solid green. This color is used
      /// for sent messages and resource
      /// counters.
      /// @~
      Green      = 7,
      /// @~English
      /// A type of red. This color is used
      /// to color the name of the red player.
      /// @~
      BrightRed    = 8,
      /// @~English
      /// This code hides all text and formatting
      /// that follows.
      /// @~
      Invisible    = 11,
      /// @~English
      /// A deep blue. This color is used
      /// to color the name of the blue player.
      /// @~
      Blue      = 14,
      /// @~English
      /// A teal color. This color is used
      /// to color the name of the teal player.
      /// @~
      Teal      = 15,
      /// @~English
      /// A deep purple. This color is used
      /// to color the name of the purple player.
      /// @~
      Purple      = 16,
      /// @~English
      /// A solid orange. This color is used
      /// to color the name of the orange player.
      /// @~
      Orange      = 17,
      /// @~English
      /// An alignment directive that aligns
      /// the text to the right side of the
      /// screen.
      /// @~
      Align_Right    = 18,
      /// @~English
      /// An alignment directive that aligns
      /// the text to the center of the screen.
      /// @~
      Align_Center  = 19,
      /// @copydoc Invisible
      Invisible2    = 20,
      /// @~English
      /// A dark brown. This color is used
      /// to color the name of the brown player.
      /// @~
      Brown      = 21,
      /// @~English
      /// A dirty white. This color is used
      /// to color the name of the white player.
      /// @~
      PlayerWhite    = 22,
      /// @~English
      /// A deep yellow. This color is used
      /// to color the name of the yellow player.
      /// @~
      PlayerYellow  = 23,
      /// @~English
      /// A dark green. This color is used
      /// to color the name of the green player.
      /// @~
      DarkGreen    = 24,
      /// @~English
      /// A bright yellow.
      /// @~
      LightYellow    = 25,
      /// @~English
      /// A cyan color. Similar to Default.
      /// @~
      Cyan      = 26,
      /// @~English
      /// A tan color.
      /// @~
      Tan        = 27,
      /// @~English
      /// A dark blueish color.
      /// @~
      GreyBlue    = 28,
      /// @~English
      /// A type of Green.
      /// @~
      GreyGreen    = 29,
      /// @~English
      /// A different type of Cyan.
      /// @~
      GreyCyan    = 30,
      /// @~English
      /// A bright blue color.
      /// @~
      Turquoise    = 31
    };
  }
}
