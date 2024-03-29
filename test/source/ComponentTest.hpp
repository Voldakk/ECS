#pragma once
#include <unordered_map>

#include "test.hpp"

namespace EVA::ECS
{
    TEST(Component, ComponentData)
    {
        Comp0 c;
        Position p;
        StructComponentA a;

        EXPECT_EQ(c.GetType(), Comp0::GetType());
        EXPECT_EQ(p.GetType(), Position::GetType());
        EXPECT_EQ(a.GetType(), StructComponentA::GetType());
    }

    TEST(ComponentMap, DefaultData)
    {
        Position p;
        StructComponentA a;

        EXPECT_EQ(memcmp(&p, ComponentMap::DefaultData(p.GetType()), sizeof(Position)), 0);
        EXPECT_EQ(memcmp(&a, ComponentMap::DefaultData(a.GetType()), sizeof(StructComponentA)), 0);
    }

    TEST(ComponentMap, CreateComponent)
    {
        Position p;
        StructComponentA a;

        void* pData = malloc(sizeof(Position));
        void* aData = malloc(sizeof(StructComponentA));

        ComponentMap::CreateComponent(Position::GetType(), pData);
        ComponentMap::CreateComponent(StructComponentA::GetType(), aData);

        EXPECT_EQ(memcmp(&p, pData, sizeof(Position)), 0);
        EXPECT_EQ(memcmp(&a, aData, sizeof(StructComponentA)), 0);

        free(pData);
        free(aData);
    }

    TEST(ComponentList, Create)
    {
        ComponentList lA = ComponentList::Create<Comp1, Comp3, Comp2>();

        ComponentList lB;
        lB.Add<Comp1>().Add<Comp3>().Add<Comp2>();

        EXPECT_EQ(lA, lB);
    }

    TEST(ComponentList, AddRemoveContains)
    {
        ComponentList cl;
        EXPECT_EQ(cl.Count(), 0);
        EXPECT_FALSE(cl.Contains(Comp0::GetType()));

        cl.Add<Comp0>();
        EXPECT_EQ(cl.Count(), 1);
        EXPECT_TRUE(cl.Contains(Comp0::GetType()));

        cl.Remove<Comp0>();
        EXPECT_EQ(cl.Count(), 0);
        EXPECT_FALSE(cl.Contains(Comp0::GetType()));
    }

    TEST(ComponentList, EQ)
    {
        ComponentList lA;
        lA.Add<Comp0>().Add<Comp1>().Add<Comp2>().Remove<Comp1>().Add<Comp3>();
        ComponentList lB;
        lB.Add<Comp3>().Add<Comp2>().Add<Comp1>().Remove<Comp1>().Add<Comp0>();

        EXPECT_EQ(lA, lB);
    }

    TEST(ComponentList, Contains)
    {
        ComponentList l;

        ComponentList l_0123;
        l_0123.Add<Comp0>().Add<Comp1>().Add<Comp2>().Add<Comp3>();

        ComponentList l_01234;
        l_01234.Add<Comp0>().Add<Comp1>().Add<Comp2>().Add<Comp3>().Add<Comp4>();

        ComponentList l_23;
        l_23.Add<Comp2>().Add<Comp3>();

        ComponentList l_24;
        l_24.Add<Comp2>().Add<Comp4>();

        ComponentList l_25;
        l_25.Add<Comp2>().Add<Comp5>();

        EXPECT_TRUE(l_0123.Contains(l));
        EXPECT_TRUE(l_01234.Contains(l));

        EXPECT_TRUE(l_0123.Contains(l_23));
        EXPECT_TRUE(l_01234.Contains(l_23));

        EXPECT_FALSE(l_0123.Contains(l_24));
        EXPECT_TRUE(l_01234.Contains(l_24));

        EXPECT_FALSE(l_0123.Contains(l_25));
        EXPECT_FALSE(l_01234.Contains(l_25));
    }

    TEST(ComponentList, ComponentListHash)
    {
        ComponentList l_23;
        l_23.Add<Comp2>().Add<Comp3>();
        ComponentList l_24;
        l_24.Add<Comp2>().Add<Comp4>();
        ComponentList l_25;
        l_25.Add<Comp2>().Add<Comp5>();

        ComponentList::Hash h;
        auto h_23 = h(l_23);
        auto h_24 = h(l_24);
        auto h_25 = h(l_25);

        EXPECT_NE(h_23, h_24);
        EXPECT_NE(h_23, h_25);
        EXPECT_NE(h_24, h_25);

        std::unordered_map<ComponentList, int, ComponentList::Hash> map;

        map[l_23] = 23;
        map[l_24] = 24;
        map[l_25] = 25;

        EXPECT_EQ(map[l_23], 23);
        EXPECT_EQ(map[l_24], 24);
        EXPECT_EQ(map[l_25], 25);
    }
} // namespace EVA::ECS
