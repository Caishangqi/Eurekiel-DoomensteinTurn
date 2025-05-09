#include "WidgetTurnSequence.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Definition/CharacterDefinition.hpp"
#include "Game/Framework/TurnBaseSubsystem.hpp"
#include "Game/Gameplay/Character.hpp"
#include "Game/Gameplay/Battle/State/StateCharacterTurn.hpp"
#include "Game/Gameplay/Battle/State/StateRound.hpp"

WidgetTurnSequence::WidgetTurnSequence()
{
    m_name                      = "WidgetTurnSequence";
    m_boundingBox               = AABB2(Vec2(20.f, 680.f), Vec2(600.f, 780.f));
    mainSequenceBounding        = m_boundingBox;
    mainSequenceBounding.m_mins = m_boundingBox.m_mins;
    mainSequenceBounding.m_maxs = m_boundingBox.m_mins + Vec2(m_boundingBox.GetDimensions().y, m_boundingBox.GetDimensions().y);
    m_sequenceLabel             = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/sequence-label-main.png");
    m_sequenceLabelSub          = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/gui/sequence-label-sub.png");
}

WidgetTurnSequence::~WidgetTurnSequence()
{
}

void WidgetTurnSequence::Draw() const
{
    Widget::Draw();
    std::vector<Vertex_PCU> vertices;
    vertices.reserve(1024);

    AddVertsForAABB2D(vertices, m_boundingBox, Rgba8::DEBUG_WHITE_TRANSLUCENT);
    SetRenderState();
    g_theRenderer->BindTexture(nullptr);
    g_theRenderer->DrawVertexArray(vertices);

    std::vector<Vertex_PCU> verticesMainSeq;
    verticesMainSeq.reserve(1024);
    AddVertsForAABB2D(verticesMainSeq, mainSequenceBounding, Rgba8::WHITE);
    g_theRenderer->BindTexture(m_sequenceLabel);
    g_theRenderer->DrawVertexArray(verticesMainSeq);

    if (m_StateRound)
    {
        std::vector<Vertex_PCU> verticesMainAvtar;
        verticesMainAvtar.reserve(64);
        AddVertsForAABB2D(verticesMainAvtar, mainSequenceBounding, Rgba8::WHITE);
        g_theRenderer->BindTexture(m_StateRound->GetInitiativeEntries()[m_StateRound->GetCharacterRoundIndex()].character->GetCharacterDefinition()->m_avatar);
        g_theRenderer->DrawVertexArray(verticesMainAvtar);
    }


    std::vector<Vertex_PCU> verticesSubSeq;
    verticesSubSeq.reserve(1024);

    for (const AABB2& box : m_subSequenceBox)
    {
        AddVertsForAABB2D(verticesSubSeq, box, Rgba8::WHITE);

        g_theRenderer->BindTexture(m_sequenceLabelSub);
        g_theRenderer->DrawVertexArray(verticesSubSeq);
    }

    for (int i = 0; i < (int)m_charactersDrawingSub.size(); i++)
    {
        std::vector<Vertex_PCU> verticesSubSeqImg;
        verticesSubSeqImg.reserve(64);
        AABB2 box = m_subSequenceBox[i];
        AddVertsForAABB2D(verticesSubSeqImg, box, Rgba8::WHITE);
        g_theRenderer->BindTexture(m_charactersDrawingSub[i]->GetCharacterDefinition()->m_avatar);
        g_theRenderer->DrawVertexArray(verticesSubSeqImg);
    }
}

void WidgetTurnSequence::Update()
{
    Widget::Update();
    if (!m_StateRound) return;
    m_subSequenceBox.clear();

    m_charactersDrawingSub.clear();
    m_charactersDrawingSub.reserve(m_StateRound->GetInitiativeEntries().size());

    if (!m_StateCharacterTurn) return;
    std::vector<InitiativeEntry>& entries = m_StateRound->GetInitiativeEntries();
    size_t                        index   = m_StateRound->GetCharacterRoundIndex() + 1;
    if (index >= entries.size()) return;
    std::vector<InitiativeEntry> subEntries(entries.begin() + (int)index, entries.end()); // Exclude The Main display, the subsequence characters

    for (InitiativeEntry& entry : subEntries)
    {
        if (entry.character && entry.character->GetHandle().IsValid() && entry.character != m_StateCharacterTurn->GetCharacter())
        {
            if (entry.character->CanAct())
                m_charactersDrawingSub.push_back(entry.character);
        }
    }

    m_subSequenceBox.reserve(m_charactersDrawingSub.size());
    for (int i = 0; i < (int)m_charactersDrawingSub.size(); ++i)
    {
        AABB2 boundingBox = mainSequenceBounding;
        Vec2  mainCenter  = mainSequenceBounding.GetCenter();
        boundingBox.SetDimensions(Vec2(48, 48));
        boundingBox.SetCenter(Vec2(mainSequenceBounding.m_maxs.x, mainCenter.y) - Vec2(24, 0) + (i + 1) * Vec2(48, 0));
        /*boundingBox.m_mins = Vec2(mainSequenceBounding.m_maxs.x * ((float)i + 1), mainSequenceBounding.m_mins.y);
        boundingBox.m_maxs = boundingBox.m_mins + Vec2(mainSequenceBounding.GetDimensions().y, mainSequenceBounding.GetDimensions().y);*/
        m_subSequenceBox.push_back(boundingBox);
    }
}

void WidgetTurnSequence::InjectRoundState(StateRound* InStateRound)
{
    m_StateRound = InStateRound;
}

void WidgetTurnSequence::InjectCharacterTurnState(StateCharacterTurn* InStateCharacterTurn)
{
    m_StateCharacterTurn = InStateCharacterTurn;
}

void WidgetTurnSequence::SetRenderState() const
{
    g_theRenderer->BindShader(nullptr);
    g_theRenderer->SetBlendMode(BlendMode::ALPHA);
}
