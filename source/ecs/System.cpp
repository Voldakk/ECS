#include "System.hpp"
#include "Engine.hpp"

namespace EVA::ECS
{
    Engine& System::GetEngine() { return *m_Engine; }
    std::vector<Archetype*> System::GetArchetypes(const ComponentFilter& filter) { return m_Engine->GetArchetypes(filter, false); }
} // namespace EVA::ECS
