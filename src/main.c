#include "assert.h"
#include "raylib.h"
#include <stdio.h>

typedef enum
{
    Scene_Menu,
    Scene_Game,
} Scene;

int main(void)
{
    const int screen_width = 800;
    const int screen_height = 800;

    Scene scene = Scene_Menu;

    InitWindow(screen_width, screen_height, "Raylib Template");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        switch (scene)
        {
        case Scene_Menu:
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                scene = Scene_Game;
            }
            break;
        }
        case Scene_Game:
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                scene = Scene_Menu;
            }
            break;
        }
        default:
            assert(false);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        switch (scene)
        {
        case Scene_Menu:
        {
            DrawText("Menu Scene", 100, 100, 40, WHITE);
            break;
        }
        case Scene_Game:
        {
            DrawText("Game Scene", 100, 100, 40, WHITE);
            break;
        }
        default:
            assert(false);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
