#pragma once
#include <string>

class Character;
enum class BuffTag;

struct Buff
{
    BuffTag     m_tag;
    std::string m_desc;
    int         m_turnsLeft; // -1 代表永久
    int         m_value     = 0; // 数值幅度，Break 用不到
    bool        m_stackable = false;

    Character* m_owner = nullptr;

    Buff(BuffTag     tag,
         std::string desc,
         int         turns, // <=0 ⇒ 立即到期；-1 ⇒ 永久
         int         value = 0,
         bool        stack = false,
         Character*  owner = nullptr);

    void OnInit(); // 添加到角色时立即调用
    void OnRoundStart(); // 每回合开始
    void OnRoundEnd(); // 每回合结束（扣回合数）
    void OnEnd(); // 回合数归零后自动调用

    std::string ToString() const;

    static std::string TagToString(BuffTag tag);
};
