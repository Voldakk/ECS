#pragma once

#pragma warning(push, 0)
#include <gtest/gtest.h>
#pragma warning(pop)

#include "ecs/ecs.hpp"

struct Position
{
    EVA_ECS_REGISTER_COMPONENT(Position);

    int x, y;

    Position() : x(0), y(0) {}
    Position(int _x, int _y) : x(_x), y(_y) {}
};
inline bool operator==(const Position& lhs, const Position& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }

struct Velocity
{
    EVA_ECS_REGISTER_COMPONENT(Velocity);

    int x, y;

    Velocity() : x(0), y(0) {}
    Velocity(int _x, int _y) : x(_x), y(_y) {}
};
inline bool operator==(const Velocity& lhs, const Velocity& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }

struct StructComponentA
{
    EVA_ECS_REGISTER_COMPONENT(StructComponentA);

    int x, y, z;
    short something;
    bool flag1;
    bool flag2;

    StructComponentA() : x(0), y(0), z(0), something(123), flag1(false), flag2(true) {}
    StructComponentA(int _x, int _y, int _z, short _s, bool _f1, bool _f2) : x(_x), y(_y), z(_z), something(_s), flag1(_f1), flag2(_f2) {}
};

struct IntComp
{
    EVA_ECS_REGISTER_COMPONENT(IntComp);
    int value;
};
inline bool operator==(const IntComp& lhs, const IntComp& rhs) { return lhs.value == rhs.value; }


struct Comp0
{
    EVA_ECS_REGISTER_COMPONENT(Comp0);
};
struct Comp1
{
    EVA_ECS_REGISTER_COMPONENT(Comp1);
};
struct Comp2
{
    EVA_ECS_REGISTER_COMPONENT(Comp2);
};
struct Comp3
{
    EVA_ECS_REGISTER_COMPONENT(Comp3);
};
struct Comp4
{
    EVA_ECS_REGISTER_COMPONENT(Comp4);
};
struct Comp5
{
    EVA_ECS_REGISTER_COMPONENT(Comp5);
};
