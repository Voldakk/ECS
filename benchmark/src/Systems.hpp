#pragma once

#include "ECS/ECS.hpp"
#include "Components.hpp"

using namespace EVA::ECS;

class MovementSystem : public System
{
public:
	void Update() override
	{
		for (auto [pos, vel] : EntityIterator<Position, Velocity>(GetEngine().GetArchetypes<Position, Velocity>()))
		{
			pos.x += vel.x;
			pos.y += vel.y;
		}
	}
};

class BaselinePositionSystem : public System
{
public:
	void Update() override
	{
        for (auto [ent, pos] : GetEntityIterator<Position>())
        {
        }
	}
};

class BaselinePositionSystem2 : public System
{
public:
	void Update() override
	{
        for (auto [ent, pos] : GetEntityIterator<Position>())
        {
        }
	}
};

class BaselinePositionSystem3 : public System
{
  public:
    void Update() override
    {
        for (auto [ent, pos] : GetEntityIterator<Position>())
        {
        }
    }
};

class CompSystemA : public System
{
  public:
    void Update() override
    {
        for (auto [ent, c1, c2, c3] : GetEntityIterator<Comp1, Comp4, Comp5>())
        {
        }
    }
};

class CompSystemB : public System
{
  public:
    void Update() override
    {
        for (auto [ent, c1, c2, c3] : GetEntityIterator<Comp2, Comp8, Comp9>())
        {
        }
    }
};

class CompSystemC : public System
{
  public:
    void Update() override
    {
        for (auto [ent, c1, c2, c3] : GetEntityIterator<Comp1, Comp7, Comp9>())
        {
        }
    }
};

class CompSystemD : public System
{
  public:
    void Update() override
    {
        for (auto [ent, c1, c2, c3] : GetEntityIterator<Comp6, Comp7, Comp8>())
        {
        }
    }
};

class CompSystemE : public System
{
  public:
    void Update() override
    {
        for (auto [ent, c1, c2, c3] : GetEntityIterator<Comp10, Comp11, Comp12>())
        {
        }
    }
};

class PlainPositionSystem : public System
{
  public:
    void Update() override
    {
        for (auto [ent, pos] : GetEntityIterator<Position>())
        {
            pos.x += 1;
            pos.y -= 1;
        }
    }
};

class PlainPositionSystem2 : public System
{
  public:
    void Update() override
    {
        for (auto [ent, pos] : GetEntityIterator<Position>())
        {
            pos.x -= 1;
            pos.y += 1;
        }
    }
};

class PlainPositionSystem3 : public System
{
  public:
    void Update() override
    {
        for (auto [ent, pos] : GetEntityIterator<Position>())
        {
            pos.x += 0.5f;
            pos.y -= 0.5f;
        }
    }
};

template<typename T, typename U>
class EntityDeleterSystem : public System
{
	size_t entityCount = 100000;
	size_t counter = 0;

	size_t creationIndex = 0;
	Entity* ids;

	ComponentList components = ComponentList::Create<T, U>();

public:

	EntityDeleterSystem()
	{
		ids = new Entity[entityCount];
	}

	virtual ~EntityDeleterSystem()
	{
		delete[] ids;
	}

	void Init() override
	{
		for (size_t i = 0; entityCount > i; i++)
			CreateEntity();
	}

	void CreateEntity()
	{
		auto e = GetEngine().CreateEntity(components);

		ids[creationIndex++] = e;
		creationIndex = creationIndex % entityCount;
	}

	void Update() override
	{
		counter++;
		auto it = GetEntityIterator<Position>();

		if (counter == 100)
		{
			auto [e, pos] = *it.begin();
			GetEngine().DeleteEntity(e);
			CreateEntity();
			counter = 0;
		}
	}
};

class EntityManglerSystem : public System
{
	std::vector<size_t> ids;
	std::vector<size_t> cmp;

	std::vector<ComponentType> types;

	size_t entityCount = 4096;
	size_t renew = 0;

	size_t counter = 0;
	size_t index = 0;

    CommandQueue cq;



	std::vector<ComponentList> permutations;

	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<> dis;

	double NextFloat()
	{
		return dis(gen);
	}

public:

	EntityManglerSystem()
	{
		gen = std::mt19937(rd());
		dis = std::uniform_real_distribution<>(0, 1.0);


		size_t entityPermutations = static_cast<size_t>(sqrt(static_cast<double>(entityCount) * 16.0));
		renew = entityCount / 4;

		std::vector<size_t> idsList;
		idsList.resize(entityCount);
		for (size_t i = 0; entityCount > i; i++)
			idsList[i] = i;

		auto rng = std::default_random_engine{};
		std::shuffle(std::begin(idsList), std::end(idsList), rng);

		ids.resize(entityCount);
		for (size_t i = 0; ids.size() > i; i++)
			ids[i] = idsList[i];

		types.push_back(Comp1::GetType());
		types.push_back(Comp2::GetType());
		types.push_back(Comp3::GetType());
		types.push_back(Comp4::GetType());
		types.push_back(Comp5::GetType());
		types.push_back(Comp6::GetType());
		types.push_back(Comp7::GetType());
		types.push_back(Comp8::GetType());
		types.push_back(Comp9::GetType());
		types.push_back(Comp10::GetType());
		types.push_back(Comp11::GetType());
		types.push_back(Comp12::GetType());

		permutations.resize(entityPermutations);
		for (size_t i = 0; permutations.size() > i; i++)
		{
			for (size_t classIndex = 0, s = static_cast<size_t>(NextFloat() * 7) + 3; s > classIndex; classIndex++)
			{
                ComponentType c;
                do
                {
                    c = types[static_cast<size_t>(NextFloat() * static_cast<float>(types.size()))];
                } while (permutations[i].Contains(c));

				permutations[i].Add(c);
			}
		}

		cmp.resize(entityCount * 4);
		for (size_t i = 0; cmp.size() > i; i++)
			cmp[i] = static_cast<size_t>(NextFloat() * static_cast<float>(permutations.size()));
	}

	void Init() override
	{
		for (size_t i = 0; entityCount > i; i++)
			CreateEntity();
	}

	size_t cmpIndex = 0;
	void CreateEntity()
	{
		auto components = permutations[cmp[cmpIndex++]];
		if (cmpIndex == cmp.size()) cmpIndex = 0;
		GetEngine().CreateEntity(components);
	}

	void Update() override
	{
		counter++;
        auto it = GetEntityIterator<>();
		auto itt = it.begin();

		if (counter % 2 == 1)
		{
			for (size_t i = 0; renew > i; i++)
			{
                auto [e] = *itt;
                cq.DestroyEntity(e);
				++itt;
				index = index % entityCount;
			}
            cq.Execute(GetEngine());
		}
		else
		{
			for (size_t i = 0; renew > i; i++)
			{
				CreateEntity();
			}
		}
	}
};
