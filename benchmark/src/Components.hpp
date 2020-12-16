#pragma once

#include "ECS/ECS.hpp"

struct Position
{
    EVA_ECS_REGISTER_COMPONENT(Position);

	float x, y;

	Position() : x(0), y(0) {}
	Position(float _x, float _y) : x(_x), y(_y) {}
};

struct Velocity
{
    EVA_ECS_REGISTER_COMPONENT(Velocity);

	float x, y;

	Velocity() : x(0), y(0) {}
	Velocity(float _x, float _y) : x(_x), y(_y) {}
};

struct Comp0 { EVA_ECS_REGISTER_COMPONENT(Comp0); };
struct Comp1 { EVA_ECS_REGISTER_COMPONENT(Comp1); };
struct Comp2 { EVA_ECS_REGISTER_COMPONENT(Comp2); };
struct Comp3 { EVA_ECS_REGISTER_COMPONENT(Comp3); };
struct Comp4 { EVA_ECS_REGISTER_COMPONENT(Comp4); };
struct Comp5 { EVA_ECS_REGISTER_COMPONENT(Comp5); };
struct Comp6 { EVA_ECS_REGISTER_COMPONENT(Comp6); };
struct Comp7 { EVA_ECS_REGISTER_COMPONENT(Comp7); };
struct Comp8 { EVA_ECS_REGISTER_COMPONENT(Comp8); };
struct Comp9 { EVA_ECS_REGISTER_COMPONENT(Comp9); };
struct Comp10 { EVA_ECS_REGISTER_COMPONENT(Comp10); };
struct Comp11 { EVA_ECS_REGISTER_COMPONENT(Comp11); };
struct Comp12 { EVA_ECS_REGISTER_COMPONENT(Comp12); };

struct PlainStructComponentA
{
    EVA_ECS_REGISTER_COMPONENT(PlainStructComponentA);

	float x, y, z;
	short something;
	bool flag;

	PlainStructComponentA() : x(0), y(0), z(0), something(0), flag(true) {}
	PlainStructComponentA(float x, float y, float z) : x(x), y(y), z(z)
	{
		something = 0;
		flag = true;
	}
};
