#include "Building.hpp";

Building::Building(BuildingType _type) : type(_type) { }

Building::~Building() { }

BuildingType Building::GetType() const {
	return type;
}