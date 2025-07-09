#pragma once

#include "Headers.h"

#ifndef NAVIGATION_H
#define NAVIGATION_H


/*
///////////////// VECTOR FIELDS ////////////////////////
bool HeroAI::CheckCollision(const GW::GamePos& position, const GW::GamePos& agentpos, float radius1, float radius2) {
    float distance = GW::GetDistance(position, agentpos);
    float radius = radius1 + radius2; // Radius of the bubble
    return distance <= radius; // Check if the distance from the center is within the bubble's radius
}

bool HeroAI::IsColliding(float x, float y) {
    const auto agents = GW::Agents::GetAgentArray();
    float AgentRadius;
    for (const GW::Agent* agent : *agents)
    {
        if (!agent) { continue; }
        if (!agent->GetIsLivingType()) { continue; }
        const auto tAllyLiving = agent->GetAsAgentLiving();
        if (!tAllyLiving) { continue; }
        const auto AllyID = tAllyLiving->agent_id;
        if (AllyID == GameVars.Target.Self.ID) { continue; }
        if (tAllyLiving->GetIsDead()) { continue; }

        float AgentRadius;
        switch (tAllyLiving->allegiance) {
        case GW::Constants::Allegiance::Ally_NonAttackable:
            AgentRadius = Fields.Agent.CollisionRadius;
            break;
        case GW::Constants::Allegiance::Enemy:
            AgentRadius = Fields.Enemy.CollisionRadius;
            break;
        case GW::Constants::Allegiance::Spirit_Pet:
            AgentRadius = Fields.Spirit.CollisionRadius;
            break;
        default:
            continue;
            break;
        }

        GW::GamePos pos;
        pos.x = x;
        pos.y = y;
        if (CheckCollision(pos, tAllyLiving->pos, Fields.Agent.CollisionRadius, AgentRadius)) {
            return true;
        }

    }
    return false;
}

GW::Constants::Allegiance HeroAI::CollisionType(float x, float y) {
    const auto agents = GW::Agents::GetAgentArray();
    float AgentRadius;
    for (const GW::Agent* agent : *agents)
    {
        if (!agent) { continue; }
        if (!agent->GetIsLivingType()) { continue; }
        const auto tAllyLiving = agent->GetAsAgentLiving();
        if (!tAllyLiving) { continue; }
        const auto AllyID = tAllyLiving->agent_id;
        if (AllyID == GameVars.Target.Self.ID) { continue; }
        if (tAllyLiving->GetIsDead()) { continue; }

        float AgentRadius;
        switch (tAllyLiving->allegiance) {
        case GW::Constants::Allegiance::Ally_NonAttackable:
            AgentRadius = Fields.Agent.CollisionRadius;
            break;
        case GW::Constants::Allegiance::Enemy:
            AgentRadius = Fields.Enemy.CollisionRadius;
            break;
        case GW::Constants::Allegiance::Spirit_Pet:
            AgentRadius = Fields.Spirit.CollisionRadius;
            break;
        default:
            continue;
            break;
        }

        GW::GamePos pos;
        pos.x = x;
        pos.y = y;
        if (CheckCollision(pos, tAllyLiving->pos, Fields.Agent.CollisionRadius, AgentRadius)) {
            return tAllyLiving->allegiance;
        }

    }
    return GW::Constants::Allegiance::Neutral;
}


float HeroAI::CalculatePathDistance(float targetX, float targetY, ImDrawList* drawList, bool draw) {
    using namespace GW;

    PathPoint pathArray[30];
    uint32_t pathCount = 30;
    PathPoint start{ GameVars.Target.Self.Living->pos, nullptr };


    GamePos pos;
    pos.x = targetX;
    pos.y = targetY;
    pos.zplane = GameVars.Target.Self.Living->pos.zplane;
    PathPoint goal{ pos,nullptr };

    if (GetSquareDistance(GameVars.Target.Self.Living->pos, goal.pos) > 4500 * 4500) { return -1; }

    if (!FindPath_Func) { return -2; }

    FindPath_Func(&start, &goal, 4500.0f, pathCount, &pathCount, pathArray);

    float altitude = 0;
    GamePos g1 = pathArray[0].pos;
    GW::Map::QueryAltitude(GameVars.Target.Self.Living->pos, 10, altitude);
    Vec3f p0{ GameVars.Target.Self.Living->pos.x, GameVars.Target.Self.Living->pos.y, altitude };

    Vec3f p1{ g1.x, g1.y, altitude };

    if (draw) { Overlay.DrawLine3D(drawList, p0, p1, IM_COL32(0, 255, 0, 127), 3); }


    float totalDistance = GetDistance(GameVars.Target.Self.Living->pos, pathArray[0].pos);

    for (uint32_t i = 1; i < pathCount; ++i) {
        p0 = p1;
        p1.x = pathArray[i].pos.x;
        p1.y = pathArray[i].pos.y;
        GW::Map::QueryAltitude(g1, 10, altitude);

        if (draw) { Overlay.DrawLine3D(drawList, p0, p1, IM_COL32(0, 255, 0, 127), 3); }
        totalDistance += GetDistance(p0, p1);
    }


    return totalDistance;

}

bool HeroAI::IsPointValid(float x, float y) {
    GW::Vec2f pos{ x,y };
    float distance = GW::GetDistance(GameVars.Target.Self.Living->pos, pos);

    float calcdistance = CalculatePathDistance(pos.x, pos.y, 0, false);

    if ((calcdistance - distance) > 50.0f) { return false; }
    if (calcdistance < distance) { return false; }
    return true;
}

// Function to check if a square's sides are valid
bool HeroAI::IsSquareValid(int x, int y) {
    GW::Vec2f center{ x,y };
    float halfSize = GridSize / 2;

    if (!IsPointValid(center.x - halfSize, center.y - halfSize)) { return false; }
    if (!IsPointValid(center.x + halfSize, center.y - halfSize)) { return false; }
    if (!IsPointValid(center.x + halfSize, center.y + halfSize)) { return false; }
    if (!IsPointValid(center.x - halfSize, center.y + halfSize)) { return false; }

    return true;
}

// Function to convert (x, y) to (q, r) based on GridSize
GW::Vec2f HeroAI::ConvertToGridCoords(float x, float y) {
    int q = static_cast<int>(std::floor(x / GridSize));
    int r = static_cast<int>(std::floor(y / GridSize));
    return GW::Vec2f{ static_cast<float>(q), static_cast<float>(r) };
}

// Function to convert (q, r) to (x, y) based on GridSize
GW::Vec2f HeroAI::ConvertToWorldCoords(int q, int r) {
    float x = q * GridSize;
    float y = r * GridSize;
    return GW::Vec2f{ x, y };
}


GW::Vec2f HeroAI::GetRandomValidCoordinate(float x, float y) {
    // Convert the initial (x, y) to grid coordinates (q, r)
    GW::Vec2f initialGridCoords = ConvertToGridCoords(x, y);
    int initialQ = static_cast<int>(initialGridCoords.x);
    int initialR = static_cast<int>(initialGridCoords.y);

    // Define the search radius and step size
    int searchRadius = 1;
    const int maxRadius = 10; // Define a maximum search radius to prevent infinite loops

    // Vector to store all surrounding cell points
    std::vector<CellInfo> cellPoints;

    while (searchRadius <= maxRadius) {
        for (int dq = -searchRadius; dq <= searchRadius; ++dq) {
            for (int dr = -searchRadius; dr <= searchRadius; ++dr) {
                int q = initialQ + dq;
                int r = initialR + dr;

                // Convert grid coordinates (q, r) back to world coordinates (x, y)
                GW::Vec2f worldCoords = ConvertToWorldCoords(q, r);
                float worldX = worldCoords.x;
                float worldY = worldCoords.y;

                // Check if the point is valid and not colliding
                bool pointValid = IsPointValid(worldX, worldY);
                bool colliding = IsColliding(worldX, worldY);


                // If the point is valid and not colliding, add it to the list of available points
                if (pointValid && !colliding) {
                    cellPoints.push_back({ GW::Vec2f{ worldX, worldY }, GW::Vec2f{ static_cast<float>(q), static_cast<float>(r) }, pointValid, colliding });
                }
            }
        }
        // Increase the search radius
        ++searchRadius;
    }

    // If there are available points, return a random one
    if (!cellPoints.empty()) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        int randomIndex = std::rand() % cellPoints.size();
        return cellPoints[randomIndex].coordsXY;
    }

    // If no valid point is found within the maximum radius, return the initial point
    return GW::Vec2f{ x, y };
}

GW::Vec2f HeroAI::GetClosestValidCoordinate(float x, float y) {
    // Convert the initial (x, y) to grid coordinates (q, r)
    GW::Vec2f initialGridCoords = ConvertToGridCoords(x, y);
    int initialQ = static_cast<int>(initialGridCoords.x);
    int initialR = static_cast<int>(initialGridCoords.y);

    // Define the search radius and step size
    int searchRadius = 1;
    const int maxRadius = 10; // Define a maximum search radius to prevent infinite loops

    while (searchRadius <= maxRadius) {
        for (int dq = -searchRadius; dq <= searchRadius; ++dq) {
            for (int dr = -searchRadius; dr <= searchRadius; ++dr) {
                int q = initialQ + dq;
                int r = initialR + dr;

                // Convert grid coordinates (q, r) back to world coordinates (x, y)
                GW::Vec2f worldCoords = ConvertToWorldCoords(q, r);
                float worldX = worldCoords.x;
                float worldY = worldCoords.y;

                // Check if the point is valid and not colliding
                if (IsPointValid(worldX, worldY) && !IsColliding(worldX, worldY)) {
                    return GW::Vec2f{ worldX, worldY };
                }
            }
        }
        // Increase the search radius
        ++searchRadius;
    }

    // If no valid point is found within the maximum radius, return the initial point
    return GW::Vec2f{ x, y };
}

GW::Vec2f HeroAI::PartyMassCenter() {
    GW::Vec2f massCenter{ 0.0f, 0.0f };
    int count = 0;

    const auto agents = GW::Agents::GetAgentArray();
    for (const GW::Agent* agent : *agents) {
        if (!agent) { continue; }
        if (!agent->GetIsLivingType()) { continue; }
        const auto tAllyLiving = agent->GetAsAgentLiving();
        if (!tAllyLiving) { continue; }
        if (tAllyLiving->allegiance != GW::Constants::Allegiance::Ally_NonAttackable) { continue; }

        massCenter.x += agent->pos.x;
        massCenter.y += agent->pos.y;
        count++;
    }

    if (count > 0) {
        massCenter.x /= count;
        massCenter.y /= count;
    }

    return massCenter;
}

GW::Vec2f HeroAI::CalculateAttractionVector(const GW::Vec2f& entityPos, const GW::Vec2f& massCenter, float strength) {
    GW::Vec2f direction = massCenter - entityPos;
    float distance = GW::GetDistance(entityPos, massCenter);

    // Normalize the direction vector
    if (distance != 0) {
        direction.x /= distance;
        direction.y /= distance;
    }

    direction.x *= strength;
    direction.y *= strength;

    // Check for collisions with other agents
    const auto agents = GW::Agents::GetAgentArray();
    for (const GW::Agent* agent : *agents) {
        if (!agent) { continue; }
        if (!agent->GetIsLivingType()) { continue; }
        const auto tAllyLiving = agent->GetAsAgentLiving();
        if (!tAllyLiving) { continue; }
        if (tAllyLiving->allegiance != GW::Constants::Allegiance::Ally_NonAttackable) { continue; }

        if (tAllyLiving->agent_id == GameVars.Target.Self.ID) { continue; } // Skip self

        if (CheckCollision(entityPos, agent->pos, Fields.Agent.CollisionRadius, Fields.Agent.CollisionRadius)) {
            // Adjust the attraction vector to be perpendicular to avoid collision
            float temp = direction.x;
            direction.x = -direction.y;
            direction.y = temp;
        }
    }

    return direction;
}


GW::Vec2f HeroAI::CalculateRepulsionVector(const GW::Vec2f& entityPos) {
    GW::Vec2f repulsionVector{ 0.0f, 0.0f };

    float repulsionRadius;
    float strength;
    float combinedStrength = 0.0f;

    const auto agents = GW::Agents::GetAgentArray();
    for (const GW::Agent* agent : *agents) {
        if (!agent) { continue; }
        if (!agent->GetIsLivingType()) { continue; }
        const auto tAllyLiving = agent->GetAsAgentLiving();
        if (!tAllyLiving) { continue; }
        if (tAllyLiving->GetIsDead()) { continue; }

        switch (tAllyLiving->allegiance) {
        case GW::Constants::Allegiance::Ally_NonAttackable:
            if (tAllyLiving->GetIsMoving()) { continue; }
            repulsionRadius = Fields.Agent.CollisionRadius * 2;
            strength = Fields.Agent.RepulsionStrength;
            break;
        case GW::Constants::Allegiance::Enemy:
            repulsionRadius = Fields.Enemy.CollisionRadius * 2;
            strength = Fields.Enemy.RepulsionStrength;
            break;
        case GW::Constants::Allegiance::Spirit_Pet:
            repulsionRadius = Fields.Spirit.CollisionRadius * 2;
            strength = Fields.Spirit.RepulsionStrength;
            break;
        default:
            continue;
            break;
        }


        if (tAllyLiving->agent_id == GameVars.Target.Self.ID) { continue; } // Skip self

        GW::Vec2f agentPos = agent->pos;
        GW::Vec2f direction;
        direction.x = entityPos.x - agentPos.x;
        direction.y = entityPos.y - agentPos.y;

        if (direction.x == 0 && direction.y == 0) {
            GW::Vec2f partymass = PartyMassCenter();
            direction.x = partymass.x - agentPos.x;
            direction.y = partymass.y - agentPos.y;
        }

        float distance = GW::GetDistance(entityPos, agentPos);

        if (distance < 10.0f) { distance = repulsionRadius / 2; } // Avoid division by zero by another agent on top of the entity


        if (distance < repulsionRadius) {
            // Normalize the direction vector
            direction.x /= distance;
            direction.y /= distance;

            // Apply repulsion strength inversely proportional to the distance
            float repulsionStrength = strength * (1.0f - (distance / repulsionRadius));
            direction.x *= repulsionStrength;
            direction.y *= repulsionStrength;

            repulsionVector.x += direction.x;
            repulsionVector.y += direction.y;

            combinedStrength = (combinedStrength + strength) / 2.0f;
        }
    }

    // Normalize the resulting repulsion vector and scale by the desired strength
    float magnitude = sqrt(repulsionVector.x * repulsionVector.x + repulsionVector.y * repulsionVector.y);
    if (magnitude > 0.0f) {
        repulsionVector.x /= magnitude;
        repulsionVector.y /= magnitude;

        repulsionVector.x *= strength;
        repulsionVector.y *= strength;
        //repulsionVector.x *= combinedStrength;
        //repulsionVector.y *= combinedStrength;
    }

    return repulsionVector;
}


////////////////////// END VECTOR FIELDS ////////////////////////

bool HeroAI::AvoidCollision()
{
    //*************** COLLISION ****************************

    bool oc = (GameVars.Party.SelfPartyNumber > 1) && (GameVars.Party.InAggro) ? true : false;
    bool ooc = (GameVars.Party.SelfPartyNumber > 1) && (!GameVars.Party.InAggro) ? true : false;
    if ((!GameVars.Target.Self.Living->GetIsCasting()) &&
        (oc || ooc) &&
        (!IsMelee(GameVars.Target.Self.ID)) &&
        (ScatterRest.getElapsedTime() > 200) &&
        IsColliding(GameVars.Target.Self.Living->pos.x, GameVars.Target.Self.Living->pos.y)
        ) {
        //VECTOR FIELDS

        GW::Vec2f playerPos = { GameVars.Target.Self.Living->pos.x, GameVars.Target.Self.Living->pos.y };
        GW::Vec2f repulsionVector = CalculateRepulsionVector(playerPos);

        GW::GamePos result_2d_repulsion_vector;
        result_2d_repulsion_vector.x = GameVars.Target.Self.Living->pos.x + repulsionVector.x;
        result_2d_repulsion_vector.y = GameVars.Target.Self.Living->pos.y + repulsionVector.y;

        int tryTimeout = 0;
        const int maxTryTimeout = 10;

        while (true) {
            if (IsPointValid(result_2d_repulsion_vector.x, result_2d_repulsion_vector.y)) {
                if (!IsColliding(result_2d_repulsion_vector.x, result_2d_repulsion_vector.y)) {
                    GW::Agents::Move(result_2d_repulsion_vector.x, result_2d_repulsion_vector.y);
                    break;
                }
                else {
                    repulsionVector = CalculateRepulsionVector(result_2d_repulsion_vector);
                    result_2d_repulsion_vector.x = GameVars.Target.Self.Living->pos.x + repulsionVector.x;
                    result_2d_repulsion_vector.y = GameVars.Target.Self.Living->pos.y + repulsionVector.y;
                    tryTimeout++;
                }
            }
            else {
                GW::Vec2f pos = GetClosestValidCoordinate(result_2d_repulsion_vector.x, result_2d_repulsion_vector.y);
                GW::Agents::Move(pos.x, pos.y);
                break;
            }

            if (tryTimeout >= maxTryTimeout) {
                GW::Vec2f pos = GetClosestValidCoordinate(result_2d_repulsion_vector.x, result_2d_repulsion_vector.y);
                GW::Agents::Move(pos.x, pos.y);
                break;
            }
        }

        ScatterRest.reset();
        return true;
    }

    return false;
    //***************** END COLLISION ************************
}

void HeroAI::DrawSquare(ImDrawList* drawList, int x, int y, ImU32 color, float thickness) {
    GW::Vec2f center{ x,y };
    float halfSize = GridSize / 2;
    float halfThickness = thickness / 2;

    // Vertices of the square in local coordinates
    std::vector<GW::Vec3f> vertices = {
        {center.x - halfSize + halfThickness, center.y - halfSize + halfThickness, 0.0f},
        {center.x + halfSize - halfThickness, center.y - halfSize + halfThickness, 0.0f},
        {center.x + halfSize - halfThickness, center.y + halfSize - halfThickness, 0.0f},
        {center.x - halfSize + halfThickness, center.y + halfSize - halfThickness, 0.0f}
    };

    // Query altitude for each vertex and adjust Z-coordinate
    for (auto& vertex : vertices) {
        float altitude = 0;
        GW::Map::QueryAltitude({ vertex.x, vertex.y, 0 }, 10, altitude);
        vertex.z = altitude;  // Set the altitude for each vertex
    }

    // Draw the edges of the square
    for (size_t i = 0; i < vertices.size(); ++i) {
        size_t next = (i + 1) % vertices.size();
        Overlay.DrawLine3D(drawList, vertices[i], vertices[next], color, thickness);
    }
}

void HeroAI::DrawGridAroundPlayer(float x, float y, float MaxDrawDistance, ImDrawList* drawList) {
    GW::Vec2f PlyerQR = ConvertToGridCoords(x, y);

    int LowerQ = static_cast<int>(PlyerQR.x - MaxDrawDistance);
    int LowerR = static_cast<int>(PlyerQR.y - MaxDrawDistance);
    int HigherQ = static_cast<int>(PlyerQR.x + MaxDrawDistance);
    int HigherR = static_cast<int>(PlyerQR.y + MaxDrawDistance);

    for (int Q = LowerQ; Q <= HigherQ; ++Q) {
        for (int R = LowerR; R <= HigherR; ++R) {
            GW::Vec2f pos = ConvertToWorldCoords(Q, R);

            float thickness = 3.0f;
            auto color = IM_COL32(64, 64, 64, 64);
            // auto color = Overlay.GetColorFromGrid(Q,R);

            //if (IsSquareValid(pos.x, pos.y)) { color = IM_COL32(0, 255, 0, 127); }
            if (IsPointValid(pos.x, pos.y)) { color = IM_COL32(255, 255, 255, 127); }
            else { continue; }

            if (IsColliding(pos.x, pos.y)) {
                switch (CollisionType(pos.x, pos.y)) {
                case GW::Constants::Allegiance::Ally_NonAttackable: { color = IM_COL32(0, 255, 0, 127); break; }
                case GW::Constants::Allegiance::Enemy: { color = IM_COL32(255, 0, 0, 127); break; }
                case GW::Constants::Allegiance::Spirit_Pet: { color = IM_COL32(0, 255, 255, 127); break; }
                default: { color = IM_COL32(255, 255, 0, 127); break; }
                }
            }

            DrawSquare(drawList, pos.x, pos.y, color, thickness);
        }
    }
}

void HeroAI::DrawNavigationGrid(ImDrawList* drawList)
{
    float thickness = 4.0f;
    auto color = IM_COL32(0, 0, 255, 127);

    GW::Vec3f pos;
    pos.x = GameVars.Target.Self.Living->pos.x;
    pos.y = GameVars.Target.Self.Living->pos.y;
    //Overlay.DrawPoly3D(drawList, pos, Fields.Agent.CollisionRadius, color, 360);
    DrawGridAroundPlayer(GameVars.Target.Self.Living->pos.x, GameVars.Target.Self.Living->pos.y, 5, drawList);

}
*/

#endif // NAVIGATION_H
