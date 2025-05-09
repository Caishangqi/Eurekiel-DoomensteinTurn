#pragma once
#include "MapDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/IntVec2.hpp"

class Shader;
class SpriteSheet;
class Image;

struct CharacterSlot
{
    int         m_slotIndex       = 0;
    std::string m_fraction        = "None";
    Vec3        m_slotCenter      = Vec3::ZERO;
    EulerAngles m_slotOrientation = EulerAngles();

    CharacterSlot()
    {
    }

    friend bool operator==(const CharacterSlot& lhs, const CharacterSlot& rhs)
    {
        return lhs.m_slotIndex == rhs.m_slotIndex
            && lhs.m_fraction == rhs.m_fraction;
    }

    friend bool operator!=(const CharacterSlot& lhs, const CharacterSlot& rhs)
    {
        return !(lhs == rhs);
    }
};

class SceneDefinition
{
public:
    static std::vector<SceneDefinition> s_definitions;
    static void                         LoadDefinitions(const char* path);
    static void                         ClearDefinitions();
    static const SceneDefinition*       GetByName(const std::string& name);

    SceneDefinition(const XmlElement& sceneDefElement);

    std::string                m_name                   = "Default";
    Image*                     m_sceneImage             = nullptr;
    SpriteSheet*               m_spriteSheet            = nullptr;
    Shader*                    m_shader                 = nullptr;
    Vec3                       m_sceneCenter            = Vec3::ZERO;
    Vec3                       m_sceneCenterOrientation = Vec3::ZERO;
    IntVec2                    m_spriteSheetCellCount   = IntVec2::ZERO;
    std::vector<SpawnInfo>     m_spawnInfos;
    std::vector<CharacterSlot> m_characterSlots;

    CharacterSlot m_stateSlot = {};
};
