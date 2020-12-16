#pragma once

#include <string>
#include <iostream>
#include <random>
#include <chrono>

#include "ECS/ECS.hpp"
#include "Components.hpp"
#include "Systems.hpp"

using namespace EVA::ECS;

class Benchmark
{
protected:

	std::unique_ptr<Engine> engine;

public:

	Benchmark()
	{
		engine = std::make_unique<Engine>();
	}

	virtual ~Benchmark() = default;

	virtual std::string Name() = 0;

	virtual void Init() = 0;

	virtual void Update()
	{
		engine->UpdateSystems();
	}

	void Run(float msMax, int numits)
	{
		std::cout << std::endl << "===== " << Name() << " =====" << std::endl;
        std::cout << "CONFIGURATION: " << BENCHMARK_CONFIGURATION << std::endl;
        std::cout << "PLATFORM: " << BENCHMARK_PLATFORM << std::endl;
        std::cout << "ARCHITECTURE: " << BENCHMARK_ARCHITECTURE << std::endl;

		auto start = std::chrono::high_resolution_clock::now();
		Init();
		auto end = std::chrono::high_resolution_clock::now();
		size_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "Init: " << ms << " ms\n";
		std::cout << engine->EntityCount() << " entities\n";

		for (int i = 0; i < numits; i++)
		{
			start = std::chrono::high_resolution_clock::now();
			int n = 0;
			do
			{
				Update();
				auto end = std::chrono::high_resolution_clock::now();
				ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				n++;
			}
			while (ms < msMax);

			std::cout << "Update: " << n << " iterations in " << ms << " ms\n";
		}
        std::cout << engine->EntityCount() << " entities" << std::endl;
	}
};

class MovementBenchmark : public Benchmark
{
public:
	virtual std::string Name() override
	{
		return "MovementBenchmark";
	}

	void Init() override
	{
		engine->AddSystem<MovementSystem>();

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(-1.0, 1.0);

		auto pos = ComponentList().Add<Position>();
		Entity e;
        Position p;
		for (size_t i = 0; i < 10000; i++)
		{
			e = engine->CreateEntityFromComponents(p, Velocity((float)dis(gen), (float)dis(gen)));
		}
	}
};

class BaselineBenchmark : public Benchmark
{
public:
	virtual std::string Name() override
	{
		return "BaselineBenchmark";
	}

	void Init() override
	{
		engine->AddSystem<EntityDeleterSystem<Position, PlainStructComponentA>>();
		engine->AddSystem<BaselinePositionSystem>();
		engine->AddSystem<BaselinePositionSystem2>();
		engine->AddSystem<BaselinePositionSystem3>();
	}
};

class PlainComponentBenchmark : public Benchmark
{
public:
	virtual std::string Name() override
	{
		return "PlainComponentBenchmark";
	}

	void Init() override
	{
		engine->AddSystem<PlainPositionSystem>();
		engine->AddSystem<PlainPositionSystem2>();
		engine->AddSystem<PlainPositionSystem3>();
		engine->AddSystem<EntityDeleterSystem<Position, PlainStructComponentA>>();
	}
};

class InsertRemoveBenchmark : public Benchmark
{
public:
	virtual std::string Name() override
	{
		return "InsertRemoveBenchmark ";
	}

	void Init() override
	{
		engine->AddSystem<EntityManglerSystem>();
		engine->AddSystem<CompSystemA>();
		engine->AddSystem<CompSystemB>();
		engine->AddSystem<CompSystemC>();
		engine->AddSystem<CompSystemD>();
	}
};
