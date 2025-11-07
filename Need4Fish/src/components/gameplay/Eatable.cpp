#include "pch.h"
#include "components/gameplay/Eatable.h"

bool Eatable::CanBeEatenBy(Eatable other) const {
	return other.mass > this->mass;
}
