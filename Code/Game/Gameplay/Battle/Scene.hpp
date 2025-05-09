#pragma once
#include <vector>

#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/Definition/SceneDefinition.hpp"
#include "Game/Gameplay/Actor.hpp"
class Game;
class SceneDefinition;
class Character;
class PlayerController;
class IndexBuffer;
class VertexBuffer;
class Shader;
class Texture;

class Scene
{
    friend class Character;
    friend class TurnBaseSubsystem;

public:
    Scene(Game* game, const SceneDefinition* definition);
    ~Scene();

    void Update(float deltaTime);
    void EndFrame();
    void Render() const;

    void CreateTiles();
    void CreateGeometry();
    void AddGeometryForWall(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices, const AABB3& bounds, const AABB2& UVs) const;
    void AddGeometryForFloor(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices, const AABB3& bounds, const AABB2& UVs) const;
    void AddGeometryForCeiling(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices, const AABB3& bounds, const AABB2& UVs) const;
    void CreateBuffers();

    Actor*                  SpawnActor(const SpawnInfo& spawnInfo);
    Character*              SpawnCharacter(const SpawnInfo& spawnInfo);
    std::vector<Character*> GetCharacters() const;
    std::vector<Character*> GetCharactersByFraction(std::string fraction);
    Actor*                  GetActorByHandle(ActorHandle handle) const;
    Character*              GetCharacterByHandle(ActorHandle handle) const;
    Actor*                  GetActorByName(const std::string& name) const;

    Tile*                  GetTile(int x, int y);
    const SceneDefinition* GetSceneDefinition() const;

    /// Scene Slot Management
    const CharacterSlot* GetCharacterSlot(int index) const;
    const CharacterSlot* GetStageSlot() const;

private:
    Game*                  m_game       = nullptr;
    const SceneDefinition* m_definition = nullptr;

    // Map
    std::vector<Tile> m_tiles;
    IntVec2           m_dimensions;

    /// Actors
    std::vector<Actor*>           m_actors;
    std::vector<Character*>       m_characters;
    static constexpr unsigned int MAX_ACTOR_UID  = 0x0000fffeu;
    unsigned int                  m_nextActorUID = 3568;
    /// 

    /// Lighting
    Vec3  m_sunDirection     = Vec3(2, -1, -1);
    float m_sunIntensity     = 0.85f;
    float m_ambientIntensity = 0.35f;

    // Rendering
    std::vector<Vertex_PCUTBN> m_vertexes;
    std::vector<unsigned int>  m_indices;
    Texture*                   m_texture       = nullptr;
    Shader*                    m_shader        = nullptr;
    VertexBuffer*              m_vertexBuffer  = nullptr;
    IndexBuffer*               m_indexBuffer   = nullptr;
    PlayerController*          m_renderContext = nullptr; // Set The Current map render context, means render to which player (multiplayer compatibility support)
};
