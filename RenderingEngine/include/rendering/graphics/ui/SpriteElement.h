#ifndef SPRITE_ELEMENT_H
#define SPRITE_ELEMENT_H

#include "rendering/graphics/ui/Sprite2D.h"
#include <optional>
#include <functional>

struct SpriteElement {
	Sprite2D sprite;

	std::optional<Sprite2D> hoverSprite = std::nullopt;
	std::optional<Sprite2D> clickSprite = std::nullopt;

	float clickDelay = 1.0f;

	float remainingDelay = 0.0f;

	std::function<void()> onClick = []{};

	bool isCheckBox = false;

	void SetHoverSprite(const Sprite2D& hs) {
		hoverSprite = hs;
	}

	void SetClickSprite(const Sprite2D& cs) {
		clickSprite = cs;
	}

	void SetClickFunction(std::function<void()> clickFunction_) {
		onClick = clickFunction_;
	}

	void InvertBaseAndClickSprites() {
		if (clickSprite.has_value()) {
			auto tmp = sprite;
			sprite = clickSprite.value();
			clickSprite = tmp;
		}
	}

	Sprite2D UpdateAndGetDisplayedSprite() {
		//remainingDelay -= 1.0f / 60.0f; // ignoble
		remainingDelay = (remainingDelay >= 1.0f / 60.0f) ? remainingDelay - 1.0f / 60.0f : 0.0f;
		if (clickSprite.has_value() && remainingDelay > 0.0f && !isCheckBox)
		{
			return clickSprite.value();
		}
		else if (hoverSprite.has_value() && isHovered())
			return hoverSprite.value();
		else
			return sprite;
	}

	bool isHovered() const {
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
