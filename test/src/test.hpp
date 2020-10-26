#pragma once

#pragma warning(push, 0)
#include <gtest/gtest.h>
#pragma warning(pop)

#include "ecs/ecs.hpp"

struct Position : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Position);

    int x, y;

    Position() : x(0), y(0) {}
    Position(int _x, int _y) : x(_x), y(_y) {}
};

struct Velocity : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Velocity);

    int x, y;

    Velocity() : x(0), y(0) {}
    Velocity(int _x, int _y) : x(_x), y(_y) {}
};

struct StructComponentA : public EVA::ECS::Component
{
    REGISTER_COMPONENT(StructComponentA);

    int x, y, z;
    short something;
    bool flag1;
    bool flag2;

    StructComponentA() : x(0), y(0), z(0), something(123), flag1(false), flag2(true) {}
    StructComponentA(int x, int y, int z, short s, bool f1, bool f2) : x(x), y(y), z(z), something(s), flag1(f1), flag2(f2) {}
};

struct Comp0 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp0);
};
struct Comp1 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp1);
};
struct Comp2 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp2);
};
struct Comp3 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp3);
};
struct Comp4 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp4);
};
struct Comp5 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp5);
};
struct Comp6 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp6);
};
struct Comp7 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp7);
};
struct Comp8 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp8);
};
struct Comp9 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp9);
};
struct Comp10 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp10);
};
struct Comp11 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp11);
};
struct Comp12 : public EVA::ECS::Component
{
    REGISTER_COMPONENT(Comp12);
};
