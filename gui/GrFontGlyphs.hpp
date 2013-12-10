#ifndef ___INANITY_GUI_GR_FONT_GLYPHS_HPP___
#define ___INANITY_GUI_GR_FONT_GLYPHS_HPP___

#include "FontGlyphs.hpp"
#include "../graphics/graphics.hpp"
#include <vector>

BEGIN_INANITY_GRAPHICS

class Texture;

END_INANITY_GRAPHICS

BEGIN_INANITY_GUI

class GrFontGlyphs : public FontGlyphs
{
public:
	/// Glyph structure.
	/** UV coords: (0, 0) is left-top.
	Offset coords: (0, 0) is left-bottom. */
	struct Glyph
	{
		/// Left-bottom UV coordinates.
		Graphics::vec2 uvLeftBottom;
		/// Right-top UV coordinates.
		Graphics::vec2 uvRightTop;
		/// Offset from pen point to left-bottom corner, in pixels.
		Graphics::vec2 offsetLeftBottom;
		/// Offset from pen point to right-top corner, in pixels.
		Graphics::vec2 offsetRightTop;
	};

	typedef std::vector<Glyph> Glyphs;

private:
	/// Texture with glyphs.
	ptr<Graphics::Texture> texture;
	/// Glyphs info.
	Glyphs glyphs;

public:
	GrFontGlyphs(ptr<Graphics::Texture> texture, const Glyphs& glyphs);

	ptr<Graphics::Texture> GetTexture() const;
	const Glyphs& GetGlyphs() const;
};

END_INANITY_GUI

#endif