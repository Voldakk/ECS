#pragma once

#include "CommandQueue.hpp"

namespace EVA::ECS
{
    CommandQueue::CommandQueue() : m_Data(DefaultCommandQueueSize) {}

    void CommandQueue::CreateEntity(const ComponentList& components)
    {
        m_Commands.emplace_back(Command(CommandType::CreateEntity, components, 0));
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
} // namespace EVA::ECS
