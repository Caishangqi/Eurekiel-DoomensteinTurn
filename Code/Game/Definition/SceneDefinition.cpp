#include "SceneDefinition.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Framework/LoggerSubsystem.hpp"
std::vector<SceneDefinition> SceneDefinition::s_definitions = {};

void SceneDefinition::LoadDefinitions(const char* path)
{
    LOG(LogResource, Info, "SceneDefinition::LoadDefinitions    %s", "Start Loading SceneDefinitions\n");
    XmlDocument sceneDefinitions;
    XmlResult   result = sceneDefinitions.LoadFile(path);
    if (result == XmlResult::XML_SUCCESS)
    {
        XmlElement* rootElement = sceneDefinitions.RootElement();
        if (rootElement)
        {
            printf("SceneDefinition::LoadDefinitions    SceneDefinition from \"%s\" was loaded\n", path);
            const XmlElement* element = rootElement->FirstChildElement();
            while (element != nullptr)
            {
                auto sceneDef = SceneDefinition(*element);
                s_definitions.push_back(sceneDef);
                element = element->NextSiblingElement();
            }
        }
        else
        {
            printf("SceneDefinition::LoadDefinitions    SceneDefinitions from \"%s\"was invalid (missing root element)\n", path);
        }
    }
    else
    {
        printf("SceneDefinition::LoadDefinitions    Failed to load SceneDefinitions from \"%s\"\n", path);
    }
}

void SceneDefinition::ClearDefinitions()
{
    for (SceneDefinition& definition : s_definitions)
    {
        delete definition.m_shader;
        definition.m_shader = nullptr;
    }
    s_definitions.clear();
}

const SceneDefinition* SceneDefinition::GetByName(const std::string& name)
{
    for (int i = 0; i < static_cast<int>(s_definitions.size()); i++)
    {
        if (s_definitions[i].m_name == name)
        {
            return &s_definitions[i];
        }
    }
    return nullptr;
}

SceneDefinition::SceneDefinition(const XmlElement& sceneDefElement)
{
    m_name                              = ParseXmlAttribute(sceneDefElement, "name", m_name);
    m_sceneImage                        = g_theRenderer->CreateImageFromFile(ParseXmlAttribute(sceneDefElement, "image", m_name).c_str());
    m_spriteSheetCellCount              = ParseXmlAttribute(sceneDefElement, "spriteSheetCellCount", m_spriteSheetCellCount);
    m_spriteSheet                       = new SpriteSheet(*g_theRenderer->CreateOrGetTextureFromFile(ParseXmlAttribute(sceneDefElement, "spriteSheetTexture", m_name).c_str()), m_spriteSheetCellCount);
    m_shader                            = g_theRenderer->CreateShaderFromFile(ParseXmlAttribute(sceneDefElement, "shader", m_name).c_str(), VertexType::Vertex_PCUTBN);
    m_sceneCenter                       = ParseXmlAttribute(sceneDefElement, "sceneCenter", m_sceneCenter);
    m_sceneCenterOrientation            = ParseXmlAttribute(sceneDefElement, "sceneCenterOrientation", m_sceneCenterOrientation);
    const XmlElement* spawnInfosElement = FindChildElementByName(sceneDefElement, "SpawnInfos");
    if (spawnInfosElement)
    {
        printf("                                ‖ Loading SpawnInfos Information\n");
        const XmlElement* spawnInfoElement = spawnInfosElement->FirstChildElement();
        while (spawnInfoElement != nullptr)
        {
            SpawnInfo spawnInfo;
            spawnInfo.m_actorName   = ParseXmlAttribute(*spawnInfoElement, "actor", spawnInfo.m_actorName);
            spawnInfo.m_faction     = ParseXmlAttribute(*spawnInfoElement, "faction", spawnInfo.m_faction);
            spawnInfo.m_position    = ParseXmlAttribute(*spawnInfoElement, "position", spawnInfo.m_position);
            spawnInfo.m_orientation = ParseXmlAttribute(*spawnInfoElement, "orientation", spawnInfo.m_orientation);
            m_spawnInfos.push_back(spawnInfo);
            spawnInfoElement = spawnInfoElement->NextSiblingElement();
        }
    }

    const XmlElement* stageSlotsElement = FindChildElementByName(sceneDefElement, "StageSlot");
    if (stageSlotsElement)
    {
        CharacterSlot stageSlot     = {};
        stageSlot.m_slotCenter      = ParseXmlAttribute(*stageSlotsElement, "position", stageSlot.m_slotCenter);
        stageSlot.m_slotOrientation = EulerAngles(ParseXmlAttribute(*stageSlotsElement, "orientation", Vec3::ZERO));
        m_stateSlot                 = stageSlot;
    }
    else
    {
        ERROR_AND_DIE("SceneDefinition::SceneDefinition     You must set the Scene Stage before loading the game!\n")
    }

    const XmlElement* characterSlotsElement = FindChildElementByName(sceneDefElement, "CharacterSlots");
    if (characterSlotsElement)
    {
        const XmlElement* characterSlotElement = characterSlotsElement->FirstChildElement();
        while (characterSlotElement != nullptr)
        {
            CharacterSlot characterSlot;
            characterSlot.m_slotCenter      = ParseXmlAttribute(*characterSlotElement, "position", characterSlot.m_slotCenter);
            characterSlot.m_slotIndex       = ParseXmlAttribute(*characterSlotElement, "index", characterSlot.m_slotIndex);
            characterSlot.m_slotOrientation = EulerAngles(ParseXmlAttribute(*stageSlotsElement, "orientation", Vec3::ZERO));
            characterSlot.m_fraction        = "Friend";
            m_characterSlots.push_back(characterSlot);
            characterSlotElement = characterSlotElement->NextSiblingElement();
        }
    }
    const XmlElement* enemyCharacterSlotsElement = FindChildElementByName(sceneDefElement, "EnemyCharacterSlots");
    if (enemyCharacterSlotsElement)
    {
        const XmlElement* enemyCharacterSlotElement = enemyCharacterSlotsElement->FirstChildElement();
        while (enemyCharacterSlotElement != nullptr)
        {
            CharacterSlot enemyCharacterSlot;
            enemyCharacterSlot.m_slotCenter      = ParseXmlAttribute(*enemyCharacterSlotElement, "position", enemyCharacterSlot.m_slotCenter);
            enemyCharacterSlot.m_slotIndex       = ParseXmlAttribute(*enemyCharacterSlotElement, "index", enemyCharacterSlot.m_slotIndex);
            enemyCharacterSlot.m_slotOrientation = EulerAngles(ParseXmlAttribute(*stageSlotsElement, "orientation", Vec3::ZERO));
            enemyCharacterSlot.m_fraction        = "Enemy";
            m_characterSlots.push_back(enemyCharacterSlot);
            enemyCharacterSlotElement = enemyCharacterSlotElement->NextSiblingElement();
        }
    }

    printf("SceneDefinition::SceneDefinition    + Load Definition \"%s\" \n", m_name.c_str());
    printf("                                ‖ Scene SpriteSheet Dimension: %d x %d \n", m_spriteSheetCellCount.x, m_spriteSheetCellCount.y);
    printf("                                ‖ Scene Dimension: %d x %d \n", m_sceneImage->GetDimensions().x, m_sceneImage->GetDimensions().y);
}
