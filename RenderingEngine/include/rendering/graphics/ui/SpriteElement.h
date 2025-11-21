#ifndef SPRITE_ELEMENT_H
#define SPRITE_ELEMENT_H

#include "rendering/graphics/ui/Sprite2D.h"
#include <optional>
#include <functional>

class SpriteElement {
	Sprite2D sprite;

	std::optional<Sprite2D> hoverSprite = std::nullopt;
	std::optional<Sprite2D> clickSprite = std::nullopt;

	const float clickDelay = 0.5f;
	float remainingDelay = 0.0f;

public:
	SpriteElement(const Sprite2D& sprite) : sprite(sprite) {}
	SpriteElement(const Sprite2D& sprite, const Sprite2D& secondSprite, bool isHover) : sprite(sprite) {
		if (isHover)
			hoverSprite = secondSprite;
		else
			clickSprite = secondSprite;

	}
	SpriteElement(const Sprite2D& sprite, const Sprite2D& hoverSprite, const Sprite2D& clickSprite)
		: sprite(sprite),
		hoverSprite(hoverSprite),
		clickSprite(clickSprite)
	{
	}

	void SetHoverSprite(const Sprite2D& hs) {
		hoverSprite = hs;
	}

	void SetClickSprite(const Sprite2D& cs) {
		clickSprite = cs;
	}

	Sprite2D UpdateAndGetDisplayedSprite(const double deltaTime) {
		remainingDelay -= (deltaTime > 0.0f) ? deltaTime : 1.0f / 60.0f; // ignoble
		if (clickSprite.has_value() && remainingDelay > 0.0f)
			return clickSprite.value();
		else if (hoverSprite.has_value() && isHovered())
			return hoverSprite.value();
		else
			return sprite;
	}

	void onClick(std::function<void()> f) {
		if (isHovered())
		{
			remainingDelay = clickDelay;
			f();
		}
	}

private:
	bool isHovered() {
		POINT currentCursorCoordinates;
		if (!GetCursorPos(&currentCursorCoordinates))
			return false;

		const auto cursorX = currentCursorCoordinates.x;
		const auto cursorY = currentCursorCoordinates.y;

		return (cursorX >= sprite.position.x
			&& cursorX <= sprite.position.x + sprite.texture.width
			&& cursorY >= sprite.position.y
			&& cursorY <= sprite.position.y + sprite.texture.height);
	}
};

#endif
