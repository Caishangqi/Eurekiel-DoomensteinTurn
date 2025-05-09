﻿#include "TileDefinition.hpp"

/// Definitions
std::vector<TileDefinition> TileDefinition::s_definitions = {};
/// 

void TileDefinition::LoadDefinitions(const char* path)
{
    printf("TileDefinition::LoadDefinitions    %s", "Start Loading TileDefinition\n");
    XmlDocument mapDefinitions;
    XmlResult   result = mapDefinitions.LoadFile(path);
    if (result == XmlResult::XML_SUCCESS)
    {
        XmlElement* rootElement = mapDefinitions.RootElement();
        if (rootElement)
        {
            printf("TileDefinition::LoadDefinitions    TileDefinitions from \"%s\" was loaded\n", path);
            const XmlElement* element = rootElement->FirstChildElement();
            while (element != nullptr)
            {
                auto tileDef = TileDefinition(*element);
                s_definitions.push_back(tileDef);
                element = element->NextSiblingElement();
            }
        }
        else
        {
            printf("TileDefinition::LoadDefinitions    TileDefinitions from \"%s\"was invalid (missing root element)\n", path);
        }
    }
    else
    {
        printf("TileDefinition::LoadDefinitions    Failed to load TileDefinitions from \"%s\"\n", path);
    }
}

void TileDefinition::ClearDefinitions()
{
}

TileDefinition* TileDefinition::GetByName(const std::string& name)
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

TileDefinition* TileDefinition::GetByTexelColor(const Rgba8& color)
{
    for (int i = 0; i < static_cast<int>(s_definitions.size()); i++)
    {
        Rgba8 defColor = s_definitions[i].m_mapImagePixelColor;
        if (defColor.r == color.r && defColor.g == color.g && defColor.b == color.b)
        {
            return &s_definitions[i];
        }
    }
    return nullptr;
}

TileDefinition::TileDefinition(const XmlElement& tileDefElement)
{
    m_name                = ParseXmlAttribute(tileDefElement, "name", m_name);
    m_isSolid             = ParseXmlAttribute(tileDefElement, "isSolid", m_isSolid);
    m_mapImagePixelColor  = ParseXmlAttribute(tileDefElement, "mapImagePixelColor", m_mapImagePixelColor);
    m_floorSpriteCoords   = ParseXmlAttribute(tileDefElement, "floorSpriteCoords", m_floorSpriteCoords);
    m_ceilingSpriteCoords = ParseXmlAttribute(tileDefElement, "ceilingSpriteCoords", m_ceilingSpriteCoords);
    m_wallSpriteCoords    = ParseXmlAttribute(tileDefElement, "wallSpriteCoords", m_wallSpriteCoords);
    printf("TileDefinition::MapDefinition    + Load Definition \"%s\" \n", m_name.c_str());
}
