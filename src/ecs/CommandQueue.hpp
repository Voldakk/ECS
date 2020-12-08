#pragma once

#include "Core.hpp"
#include "Engine.hpp"

namespace EVA::ECS
{
    class CommandQueue
    {
        enum class CommandType
        {
            CreateEntity,
            CreateEntityFromComponents,
            DestroyEntity
        };

        struct Command
        {
            CommandType type;
            ComponentList components;
            Index dataIndex;
            Command(CommandType _type, ComponentList _components, Index _dataIndex)
            : type(_type), components(std::move(_components)), dataIndex(_dataIndex)
            {
            }
        };

      public:
        CommandQueue();
        void CreateEntity(const ComponentList& components);
        template <typename... T> void CreateEntityFromComponents(const T&... components);
        void DestroyEntity(const Entity& entity);

        void Execute(Engine& engine);
        void Clear();

      private:
        template <typename... T> void AddData(const T&... items);

        std::vector<Command> m_Commands;
        std::vector<Byte> m_Data;
        Index m_DataIndex = 0;
    };

    CommandQueue::CommandQueue() : m_Data(DefaultCommandQueueSize) {}

    void CommandQueue::CreateEntity(const ComponentList& components)
    {
        m_Commands.emplace_back(Command(CommandType::CreateEntity, components, 0));
    }

    template <typename... T> void CommandQueue::CreateEntityFromComponents(const T&... components)
    {
        m_Commands.emplace_back(Command(CommandType::CreateEntityFromComponents, ComponentList::Create<T...>(), m_DataIndex));
        AddData(components...);
    }

    void CommandQueue::DestroyEntity(const Entity& entity)
    {
        m_Commands.emplace_back(Command(CommandType::DestroyEntity, ComponentList(), m_DataIndex));
        AddData(entity);
    }

    void CommandQueue::Execute(Engine& engine)
    {
        for (const auto& command : m_Commands)
        {
            switch (command.type)
            {
                case CommandType::CreateEntity:
                {
                    engine.CreateEntity(command.components);
                    break;
                }
                case CommandType::CreateEntityFromComponents:
                {
                    engine.CreateEntity(command.components, &m_Data[command.dataIndex]);
                    break;
                }
                case CommandType::DestroyEntity:
                {
                    engine.DeleteEntity(*FromBytes<Entity>(&m_Data[command.dataIndex]));
                    break;
                }
            }
        }

        Clear();
    }

    void CommandQueue::Clear()
    {
        m_Commands.clear();
        m_DataIndex = 0;
    }

    template <typename... T> void CommandQueue::AddData(const T&... items)
    {
        if (m_DataIndex + SizeOf<T...> > m_Data.size())
        {
            m_Data.resize(m_Data.size() * 2);
        }

        CopyInto(m_Data, m_DataIndex, items...);
    }
} // namespace EVA::ECS
