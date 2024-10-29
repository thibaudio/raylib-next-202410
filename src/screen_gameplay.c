/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

typedef struct EntityDot
{
    Vector2 WorldPosition;
    struct EntityDot* Prev;
    struct EntityDot* Next;
} EntityDot;

const int DOTS = 10;
const float DOT_RADIUS = 5.f;

static EntityDot* AllDots;
static int StartingDotIndex;
static int CurrentDotIndex;
static int SelectedDotIndex;

static int Points;

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    Points = 0;
    // Random draw dots in world
    AllDots = MemAlloc(sizeof(EntityDot) * DOTS);
    for(int i = 0; i < DOTS; ++i)
    {
       AllDots[i] = (EntityDot) {
          (Vector2) {GetRandomValue(10, GetScreenWidth() - 10 ), GetRandomValue(10, GetScreenHeight() - 10)},
           NULL,
           NULL
       };
    }
    // Random select starting dot
    StartingDotIndex = GetRandomValue(0, DOTS - 1);
    CurrentDotIndex = StartingDotIndex;
    
    // Camera over starting dot


    
    framesCounter = 0;
    finishScreen = 0;
}

bool IntersectsWithExisting(const Vector2* point)
{
    EntityDot* currentDot = &AllDots[StartingDotIndex];
    while(currentDot->Next != &AllDots[CurrentDotIndex] && currentDot->Next != NULL)
    {
        Vector2 collisionPoint;
        if(CheckCollisionLines(currentDot->WorldPosition, currentDot->Next->WorldPosition, AllDots[CurrentDotIndex].WorldPosition, *point, &collisionPoint))
        {
            return true;
        }
        currentDot = currentDot->Next;
    }
    return false;
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // TODO: Update GAMEPLAY screen variables here!
    // if no available dots on screen -> End screen
    // If hover dots -> select dot
    // If tap dots && no selected dot -> select dot
    // if click || (tap dot == selected dot) -> create connection
    // create connection -> move camera to end dot

    if(Vector2LengthSqr(GetMouseDelta()) > 0)
    {
        Vector2 mousePosition = GetMousePosition();
        SelectedDotIndex = -1;
        for(int i = 0; i < DOTS; i++)
        {
            if (Vector2Distance(mousePosition, AllDots[i].WorldPosition) < DOT_RADIUS)
            {
                SelectedDotIndex = i;
                break;
            }
        }
        if(SelectedDotIndex != -1)
        {
            if(IntersectsWithExisting(&AllDots[SelectedDotIndex].WorldPosition))
            {
                SelectedDotIndex = -1;
            }
        }
    }
    
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && SelectedDotIndex != -1 && AllDots[SelectedDotIndex].Next == NULL && AllDots[SelectedDotIndex].Prev == NULL)
    {
        Points += (int) floorf(Vector2DistanceSqr(AllDots[CurrentDotIndex].WorldPosition, AllDots[SelectedDotIndex].WorldPosition));
        
        AllDots[CurrentDotIndex].Next = &AllDots[SelectedDotIndex];
        AllDots[SelectedDotIndex].Prev = &AllDots[CurrentDotIndex];

        CurrentDotIndex = SelectedDotIndex;
        SelectedDotIndex = -1;
    }

    bool existingNextMove = false;
    for(int i = 0; i < DOTS; ++i)
    {
        if(i == CurrentDotIndex || i == StartingDotIndex) continue;
        if(AllDots[i].Next != NULL) continue;
        if(!IntersectsWithExisting(&AllDots[i].WorldPosition))
        {
            existingNextMove = true;
            break;
        }
    }
    if(existingNextMove == false)
    {
       finishScreen = 1; 
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    // Background
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    // Draw dots
    for(int i = 0; i < DOTS; ++i)
    {
        Color color;
        if(i == CurrentDotIndex)
        {
            color = BLUE;
        }
        else if (AllDots[i].Next != NULL || AllDots[i].Prev != NULL)
        {
            color = BLUE;
        }
        else if (i == SelectedDotIndex)
        {
           color = WHITE; 
        }
        else
        {
            color = MAROON;
        }
        DrawCircle(AllDots[i].WorldPosition.x, AllDots[i].WorldPosition.y, DOT_RADIUS, color);
    }

    //Draw connections
    EntityDot* currentDot = &AllDots[StartingDotIndex];
    while(currentDot->Next != NULL)
    {
        DrawLine(currentDot->WorldPosition.x, currentDot->WorldPosition.y, currentDot->Next->WorldPosition.x, currentDot->Next->WorldPosition.y, BLUE);
        currentDot = currentDot->Next;
    }
    Vector2 mousePos = GetMousePosition();
    Color lineColor = BLUE;
    if(IntersectsWithExisting(&mousePos))
    {
       lineColor = GRAY; 
    }
    DrawLine(currentDot->WorldPosition.x, currentDot->WorldPosition.y, mousePos.x, mousePos.y, lineColor);

    char buffer[32];
    snprintf(buffer, sizeof buffer, "Points: %d", Points);
    Vector2 pos = { 20, 10 };
    DrawTextEx(font, buffer, pos, font.baseSize*3.0f, 4, DARKBLUE);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
    MemFree(AllDots);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}