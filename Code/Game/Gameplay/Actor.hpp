﻿#pragma once
#include <vector>

#include "Map.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec3.hpp"
#include "../Definition/MapDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/ZCylinder.hpp"
#include "Game/Framework/ActorHandle.hpp"
#include "Game/Framework/Sound.hpp"


class AnimationGroup;
class Controller;
class AIController;
class Weapon;
class Map;
class PlayerController;
class ActorDefinition;
class AABB3;
class AABB2;
class Texture;


class Actor
{
public:
    Actor();
    Actor(const SpawnInfo& spawnInfo);
    Actor(const Vec3& position, const EulerAngles& orientation, const Rgba8& color, float physicalHeight = 2.0f, float physicalRadius = 1.0f, bool bIsStatic = false);
    virtual ~Actor();


    Vec3        m_position; // 3D position, as a Vec3, in world units.
    EulerAngles m_orientation; // 3D orientation, as EulerAngles, in degrees.
    Vec3        m_velocity; // 3D velocity, as a Vec3, in world units per second.
    Vec3        m_acceleration; // 3D acceleration, as a Vec3, in world units per second squared.

    bool  m_bIsStatic;
    float m_physicalHeight;
    float m_physicalRadius;

    Rgba8 m_color;

    ActorDefinition* m_definition = nullptr; // A reference to our actor definition.
    Map*             m_map        = nullptr; // Reference to the map that spawned us.
    ActorHandle      m_handle     = ActorHandle::INVALID; // This actor's handle, assigned by the map when this actor was spawned.
    float            m_health     = 1.f; // Current health.
    Actor*           m_owner      = nullptr; //Only applies to projectile actors. Actor that fired this projectile, for purposes of collision filtering.
    float            m_dead       = 0.f; // Any data needed to track if and how long we have been dead.
    bool             m_bIsDead    = false;
    bool             m_bIsGarbage = false; // If true, this actor is no longer needed and can be safely deleted.

    // A reference to our default AI controller, if any. Used to keep track of our AI controller
    // if the player possesses this actor, in which case he pushes the AI out of the way until
    // he releases possession.
    AIController* m_aiController = nullptr;
    // A reference to the controller currently possessing us, if any.
    Controller* m_controller = nullptr;

    std::vector<Weapon*> m_weapons;
    Weapon*              m_currentWeapon = nullptr;

protected:
    ZCylinder               m_collisionZCylinder;
    std::vector<Vertex_PCU> m_vertexes;
    std::vector<Vertex_PCU> m_vertexesCone;
    std::vector<Vertex_PCU> m_vertexesWireframe;
    std::vector<Vertex_PCU> m_vertexesConeWireframe;
    Texture*                m_texture = nullptr;

    AnimationGroup* m_currentPlayingAnimationGroup  = nullptr;
    Timer*          m_animationTimer                = nullptr;
    float           m_animationTimerSpeedMultiplier = 1.f;

    std::map<SoundID, SoundPlaybackID> m_soundPlaybackIDs;

public:
    /// After we inject the map pointer and other handle etc, we perform post initialize
    virtual void PostInitialize();

    virtual void Update(float deltaSeconds);
    /// Update Animation, if the animation is finished, we stop the animation timer and set current anim to nullptr.
    /// @param deltaSeconds 
    virtual void UpdateAnimation(float deltaSeconds);
    /// Perform physics processing if the actor is simulated. Set velocity Z-components to zero for non-flying actors.
    /// Add a drag force equal to our drag times our negative current velocity. Integrate acceleration, velocity,
    /// and position then clear out acceleration for next frame.
    /// @param deltaSeconds 
    virtual void UpdatePhysics(float deltaSeconds);
    /// Add a force to our acceleration to be used next frame. Must be called every frame to apply continual force over time.
    /// @param force 
    virtual void AddForce(Vec3 force);
    /// Add a force to our acceleration to be used next frame. Must be called every frame to apply continual force over time.
    /// @param impulse 
    virtual void AddImpulse(Vec3 impulse);
    /// Move in a specified direction at a specified speed. Movement will be done by physics simulation so
    /// add a force to make the actor move. Force should be equal in magnitude to the speed times our drag,
    /// to give us enough acceleration to overcome drag. Caller is trusted to set a speed within our limits.
    /// @param direction 
    /// @param speed 
    virtual void MoveInDirection(Vec3 direction, float speed);
    /// Turn in the shortest possible path in the supplied direction, up to a maximum that must also be provided.
    /// Set the actor orientation directly rather than using angular velocity or other physics simulation.
    /// The caller is trusted to determine the maximum amount to turn based on turn rate and delta seconds. 
    /// @param direction 
    virtual void TurnInDirection(Vec3 direction);
    ZCylinder&   GetColliderZCylinder();
    /// Handle Actor collied with other actors
    /// @param other 
    void OnColliedEnter(Actor* other);
    /// Handle Actor collied with Tile bound in XY plane
    /// @param tileXYBound 
    void OnColliedEnter(AABB2& tileXYBound);
    /// Handle Actor collied with Tile bound in Z axis
    /// @param tileXYZBound 
    void OnColliedEnter(AABB3& tileXYZBound);
    /// Take damage, handle dying due to health dropping below zero, and notify our controller of the source.
    /// @param damage
    /// @param instigator 
    void Damage(float damage, ActorHandle instigator);
    /// Set Actor state as dead and perform some visual effect
    /// @param bNewDead the new state of Dead of actor
    /// @return the current state after set Actor dead
    bool SetActorDead(bool bNewDead = true);
    void EquipWeapon(unsigned int index);
    void Attack(); // Fire our currently equipped weapon.

    Vec3          GetActorEyePosition();
    virtual void  Render() const;
    bool          PredicateRender() const; /// Predicate whether or not we render actor
    virtual Mat44 GetModelToWorldTransform() const;

    /// AI
    void OnPossessed(Controller* controller); // Callback for actors when possessed by a controller.
    void OnUnpossessed(); // Callback for actors when unpossessed by a controller. 


    /// Animation

    /// Play Animation by specific name, it will automatically reset the animation timer if
    /// successful play
    /// @param animationName The Name of the animation group
    /// @param force Whether or not force to play animation that do not wait current animation finished.
    /// @return the AnimationGroup that played.
    AnimationGroup* PlayAnimationByName(std::string animationName, bool force = false);

protected:
    virtual void InitLocalVertex();

};
