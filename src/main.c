#include "assert.h"
#include "raylib.h"
#include <stdio.h>

typedef enum
{
    Scene_Menu,
    Scene_Game,
} Scene;

///////////////////////////////////////////////////////////////////////////////
// Centered Text
typedef struct
{
    char *text;
    int font_size;
    int _x;
    int y;
    Color color;
} Centered_Text;

static void centered_text_init(Centered_Text *ct, char *text, int font_size,
                               int y, Color color, int screen_width)
{
    ct->text = text;
    ct->font_size = font_size;
    ct->y = y;
    ct->color = color;

    const int w = MeasureText(text, font_size);
    ct->_x = (screen_width - w) / 2;
}

static void centered_text_render(Centered_Text *ct)
{
    DrawText(ct->text, ct->_x, ct->y, ct->font_size, ct->color);
}

///////////////////////////////////////////////////////////////////////////////
int main(void)
{
    const int screen_width = 800;
    const int screen_height = 800;
    Scene scene = Scene_Menu;

    Rectangle menu_start_button;
    Color menu_start_button_color;
    Centered_Text menu_start_button_text;
    Centered_Text menu_title;

    InitWindow(screen_width, screen_height, "Raylib Template");
    SetTargetFPS(60);

    ///////////////////////////////
    // Initialize Menu
    ///////////////////////////////
    menu_start_button.width = 200;
    menu_start_button.x = (screen_width - menu_start_button.width) / 2;
    menu_start_button.y = screen_height / 2;
    menu_start_button.height = 100;

    menu_start_button_color = WHITE;
    centered_text_init(&menu_title, (char *)"Tic-Tac-Toe", 60,
                       screen_height / 4, WHITE, screen_width);
    centered_text_init(&menu_start_button_text, "Play", 50,
                       menu_start_button.y + menu_start_button.height / 4,
                       BLACK, menu_start_button.width);

    menu_start_button_text._x += (int)menu_start_button.x;

    ///////////////////////////////
    // Initialize Game
    ///////////////////////////////

    while (!WindowShouldClose())
    {
        ///////////////////////////////
        // Update
        ///////////////////////////////
        switch (scene)
        {
        case Scene_Menu:
        {
            Vector2 mouse_pos = GetMousePosition();
            if (CheckCollisionPointRec(mouse_pos, menu_start_button))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
                    IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) ||
                    IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
                {
                    scene = Scene_Game;
                }
                else
                {
                    menu_start_button_color = YELLOW;
                }
            }
            else
            {
                menu_start_button_color = WHITE;
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

        ///////////////////////////////
        // Drawing
        ///////////////////////////////
        BeginDrawing();
        ClearBackground(BLACK);

        switch (scene)
        {
        case Scene_Menu:
        {
            DrawRectangleRec(menu_start_button, menu_start_button_color);
            centered_text_render(&menu_start_button_text);
            centered_text_render(&menu_title);
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
