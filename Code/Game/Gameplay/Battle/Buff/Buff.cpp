#include "Buff.hpp"
#include "BuffTag.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Gameplay/Character.hpp"

Buff::Buff(BuffTag tag, std::string  desc, int turns, int value,
           bool    stack, Character* owner)
    : m_tag(tag), m_desc(std::move(desc)),
      m_turnsLeft(turns), m_value(value),
      m_stackable(stack), m_owner(owner)
{
}

void Buff::OnInit()
{
    if (m_tag == BuffTag::Break)
    {
        m_owner->m_canAct = false; // 本回合仍跳过
    }
}

void Buff::OnRoundStart()
{
    if (m_tag == BuffTag::Break)
    {
        m_owner->m_canAct = false; // 本回合仍跳过
    }
}

void Buff::OnRoundEnd()
{
    if (m_turnsLeft > 0) --m_turnsLeft;
    if (m_turnsLeft == 0) OnEnd();
}

void Buff::OnEnd()
{
    // 撤销效果
    if (m_tag == BuffTag::Break)
    {
        // 恢复护盾
    }
}

std::string Buff::ToString() const
{
    std::string templateString;
    std::string tagName = TagToString(m_tag);
    templateString      = Stringf("Buff {tag: %s, turn: %d, value: %d, stack: %d, owner: %s}", tagName.c_str(), m_turnsLeft, m_value, m_stackable, m_owner->m_definition->m_name.c_str());
    return templateString;
}

std::string Buff::TagToString(BuffTag tag)
{
    switch (tag)
    {
    case BuffTag::Break:
        return "Break";
        break;
    case BuffTag::Speed:
        return "Speed";
        break;
    }
    return "None";
}
