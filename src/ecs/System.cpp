#include "System.hpp"
#include "Engine.hpp"

namespace EVA::ECS
{
    Engine* System::GetEngine() { return m_Engine; }
    ArchetypeContainer System::GetArchetypes(const ComponentList& components) { return m_Engine->GetArchetypes(components, false); }
} // namespace EVA::ECS
