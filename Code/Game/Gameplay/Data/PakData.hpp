﻿#pragma once

struct DungeonData
{
    DungeonData() = default;

    bool  _isDungeon            = false;
    float m_interactRadius      = 0.5f;
    bool  m_destroyWhenFinished = true;
};
