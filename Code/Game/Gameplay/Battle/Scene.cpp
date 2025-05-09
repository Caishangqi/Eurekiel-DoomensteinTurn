#include "Scene.hpp"

#include "Engine/Core/Image.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Definition/ActorDefinition.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Definition/SceneDefinition.hpp"
#include "Game/Definition/TileDefinition.hpp"
#include "Game/Gameplay/Character.hpp"

Scene::Scene(Game* game, const SceneDefinition* definition): m_game(game), m_definition(definition)
{
    printf("Scene::Scene    + Creating Scene from the definition \"%s\"\n", definition->m_name.c_str());
    m_texture    = g_theRenderer->CreateTextureFromFile("Data/Images/Terrain_8x8.png");
    m_dimensions = definition->m_sceneImage->GetDimensions();
    m_shader     = definition->m_shader;
    CreateTiles();
    CreateGeometry();
    CreateBuffers();
}

Scene::~Scene()
{
}

void Scene::Update(float deltaTime)
{
    for (Character* character : m_characters)
    {
        if (character)
            character->Update(deltaTime);
    }

    for (Actor* actor : m_actors)
    {
        if (actor)
            actor->Update(deltaTime);
    }
}

void Scene::EndFrame()
{
    for (Actor* actor : m_actors)
    {
        if (actor && actor->m_handle.IsValid() && actor->m_bIsGarbage)
        {
            unsigned int index = actor->m_handle.GetIndex();
            delete actor;
            m_actors[index] = nullptr;
        }
    }

    for (Character* character : m_characters)
    {
        if (character && character->m_handle.IsValid() && character->m_bIsGarbage)
        {
            unsigned int index = character->m_handle.GetIndex();
            delete character;
            m_characters[index] = nullptr;
        }
    }
}

void Scene::Render() const
{
    g_theRenderer->SetModelConstants(Mat44(), Rgba8::WHITE);
    g_theRenderer->BindShader(m_shader);
    g_theRenderer->BindTexture(m_texture);
    g_theRenderer->SetLightConstants(m_sunDirection, m_sunIntensity, m_ambientIntensity);
    g_theRenderer->DrawIndexedVertexBuffer(m_vertexBuffer, m_indexBuffer, static_cast<int>(m_indices.size()));
    g_theRenderer->BindShader(nullptr);
    for (Actor* actor : m_actors)
    {
        if (actor && actor->m_handle.IsValid() && actor->m_definition->m_visible)
        {
            actor->Render();
        }
    }

    for (Character* character : m_characters)
    {
        if (character && character->m_handle.IsValid())
        {
            character->Render();
        }
    }
}

void Scene::CreateTiles()
{
    printf("Scene::CreateTiles       ‖ Creating Scene tiles \n");
    m_tiles.resize(m_definition->m_sceneImage->GetDimensions().x * m_definition->m_sceneImage->GetDimensions().y);
    for (int y = 0; y < m_definition->m_sceneImage->GetDimensions().y; y++)
    {
        for (int x = 0; x < m_definition->m_sceneImage->GetDimensions().x; x++)
        {
            Tile* tile = GetTile(x, y);
            tile->SetTileCoords(IntVec2(x, y));
            tile->SetBounds(AABB3(Vec3(static_cast<float>(x), static_cast<float>(y), 0.f), Vec3(static_cast<float>(x + 1), static_cast<float>(y + 1), 1.f)));
            Rgba8           color      = m_definition->m_sceneImage->GetTexelColor(IntVec2(x, y));
            TileDefinition* definition = TileDefinition::GetByTexelColor(color);
            if (definition == nullptr)
                printf("Scene::CreateTiles       ‖ Tile definition not found for texel color (%d, %d)", x, y);
            tile->SetTileDefinition(definition);
            //printf("Map::Create       ‖ Add tile %s at (%d, %d)\n", definition->m_name.c_str(), x, y);
        }
    }
    printf("Scene::CreateTiles       ‖ Creating total tiles: %d\n", static_cast<int>(m_tiles.size()));
}

void Scene::CreateGeometry()
{
    printf("Scene::CreateGeometry       ‖ Creating Scene Geometry \n");
    for (Tile& tile : m_tiles)
    {
        TileDefinition* definition = tile.GetTileDefinition();
        if (!definition)
            continue;
        if (definition->m_floorSpriteCoords != IntVec2::INVALID)
        {
            AddGeometryForFloor(m_vertexes, m_indices, tile.GetBounds(), m_definition->m_spriteSheet->GetSpriteUVs(definition->m_floorSpriteCoords));
        }
        if (definition->m_wallSpriteCoords != IntVec2::INVALID)
        {
            AddGeometryForWall(m_vertexes, m_indices, tile.GetBounds(), m_definition->m_spriteSheet->GetSpriteUVs(definition->m_wallSpriteCoords));
        }
        if (definition->m_ceilingSpriteCoords != IntVec2::INVALID)
        {
            AddGeometryForCeiling(m_vertexes, m_indices, tile.GetBounds(), m_definition->m_spriteSheet->GetSpriteUVs(definition->m_ceilingSpriteCoords));
        }
    }
}

void Scene::AddGeometryForWall(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices, const AABB3& bounds, const AABB2& UVs) const
{
    // -x
    AddVertsForQuad3D(vertexes, indices, Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z),
                      bounds.m_mins,
                      Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z),
                      Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z)
                      , Rgba8::WHITE, UVs);
    // +x
    AddVertsForQuad3D(vertexes, indices,
                      Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z),
                      Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z),
                      bounds.m_maxs,
                      Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z)
                      , Rgba8::WHITE, UVs);
    // -y
    AddVertsForQuad3D(vertexes, indices,
                      Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z),
                      Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z),
                      Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z),
                      bounds.m_maxs
                      , Rgba8::WHITE, UVs);
    // +y
    AddVertsForQuad3D(vertexes, indices, bounds.m_mins,
                      Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z),
                      Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z),
                      Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z),
                      Rgba8::WHITE, UVs);
}

void Scene::AddGeometryForFloor(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices, const AABB3& bounds, const AABB2& UVs) const
{
    Vec3 bottomLeft  = bounds.m_mins;
    auto bottomRight = Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
    auto topRight    = Vec3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);
    auto topLeft     = Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
    AddVertsForQuad3D(vertexes, indices, bottomLeft, bottomRight, topRight, topLeft, Rgba8::WHITE, UVs);
}

void Scene::AddGeometryForCeiling(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices, const AABB3& bounds, const AABB2& UVs) const
{
    UNUSED(vertexes)
    UNUSED(indices)
    UNUSED(bounds)
    UNUSED(UVs)
}

void Scene::CreateBuffers()
{
    printf("Scene::CreateBuffers       ‖ Creating Buffers for optimization\n");
    printf("Scene::CreateBuffers       ‖ Creating Vertex Buffers...\n");
    /// TODO: Consider refactory those steps
    m_vertexBuffer = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
    g_theRenderer->CopyCPUToGPU(m_vertexes.data(), static_cast<int>(m_vertexes.size()) * sizeof(Vertex_PCUTBN), m_vertexBuffer);
    printf("Scene::CreateBuffers       ‖ Creating Index Buffers...\n");
    m_indexBuffer = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int));
    m_indexBuffer->Resize(static_cast<int>(m_indices.size()) * sizeof(unsigned int));
    g_theRenderer->CopyCPUToGPU(m_indices.data(), static_cast<int>(m_indices.size()) * sizeof(unsigned int), m_indexBuffer);
}


Actor* Scene::SpawnActor(const SpawnInfo& spawnInfo)
{
    unsigned int newIndex = 0;
    newIndex              = static_cast<unsigned int>(m_actors.size());
    m_actors.push_back(nullptr);
    ++m_nextActorUID;
    ActorHandle handle(m_nextActorUID, newIndex);
    auto        actor  = new Actor(spawnInfo);
    actor->m_map       = g_theGame->m_map; // TODO: Because we are not design component system, we fuck it up.
    actor->m_handle    = handle;
    m_actors[newIndex] = actor;
    actor->PostInitialize();
    return actor;
}

Character* Scene::SpawnCharacter(const SpawnInfo& spawnInfo)
{
    unsigned int newIndex = 0;
    newIndex              = static_cast<unsigned int>(m_characters.size());
    m_characters.push_back(nullptr);
    ++m_nextActorUID;
    ActorHandle handle(m_nextActorUID, newIndex);
    auto        character  = new Character(spawnInfo);
    character->m_scene     = this;
    character->m_handle    = handle;
    m_characters[newIndex] = character;
    character->PostInitialize();
    return character;
}

std::vector<Character*> Scene::GetCharacters() const
{
    std::vector<Character*> characters;
    characters.reserve(m_characters.size());
    for (auto& character : m_characters)
    {
        if (character == nullptr) continue;
        Character* handle = GetCharacterByHandle(character->GetHandle());
        if (handle && !handle->GetIsGarbage())
            characters.push_back(handle);
    }
    return m_characters;
}

std::vector<Character*> Scene::GetCharactersByFraction(std::string fraction)
{
    std::vector<Character*> characters;
    for (Character* character : m_characters)
    {
        if (character)
        {
            Character* handle = GetCharacterByHandle(character->GetHandle());
            if (handle && character->m_definition->m_faction == fraction)
            {
                characters.push_back(character);
            }
        }
    }
    return characters;
}

Actor* Scene::GetActorByHandle(ActorHandle handle) const
{
    if (!handle.IsValid())
    {
        return nullptr;
    }
    unsigned int index = handle.GetIndex();
    if (index >= m_actors.size())
    {
        return nullptr;
    }
    Actor* actor = m_actors[index];
    if (actor == nullptr)
    {
        return nullptr;
    }
    if (actor->m_handle != handle)
    {
        return nullptr;
    }
    return actor;
}

Character* Scene::GetCharacterByHandle(ActorHandle handle) const
{
    if (!handle.IsValid())
    {
        return nullptr;
    }
    unsigned int index = handle.GetIndex();
    if (index >= m_characters.size())
    {
        return nullptr;
    }
    Character* character = m_characters[index];
    if (character == nullptr)
    {
        return nullptr;
    }
    if (character->m_handle != handle)
    {
        return nullptr;
    }
    return character;
}

Actor* Scene::GetActorByName(const std::string& name) const
{
    for (Actor* actor : m_actors)
    {
        if (actor && actor->m_handle.IsValid())
        {
            if (actor->m_definition->m_name == name)
            {
                return actor;
            }
        }
    }
    return nullptr;
}

Tile* Scene::GetTile(int x, int y)
{
    return &m_tiles[x + y * m_dimensions.x];
}

const SceneDefinition* Scene::GetSceneDefinition() const
{
    return m_definition;
}

const CharacterSlot* Scene::GetCharacterSlot(int index) const
{
    for (const CharacterSlot& slot : m_definition->m_characterSlots)
    {
        if (slot.m_slotIndex == index)
        {
            return &slot;
        }
    }
    return nullptr;
}

const CharacterSlot* Scene::GetStageSlot() const
{
    return &m_definition->m_stateSlot;
}
