#include "assert.h"
#include "raylib.h"
#include "stdbool.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define NUM_CELLS 9
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef enum
{
    Scene_Menu = 0,
    Scene_Game,
} Scene;

typedef enum
{
    Cell_Empty = 0,
    Cell_O,
    Cell_X,
} Cell;

#define switch_player(c) (c == Cell_O ? Cell_X : Cell_O)

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
// Board functions
static bool grid_game_over(Cell grid[NUM_CELLS], Cell player)
{
    return (grid[0] == player && grid[4] == player && grid[8] == player) ||
           (grid[2] == player && grid[4] == player && grid[6] == player) ||
           (grid[0] == player && grid[1] == player && grid[2] == player) ||
           (grid[3] == player && grid[4] == player && grid[5] == player) ||
           (grid[6] == player && grid[7] == player && grid[8] == player) ||
           (grid[0] == player && grid[3] == player && grid[6] == player) ||
           (grid[1] == player && grid[4] == player && grid[7] == player) ||
           (grid[2] == player && grid[5] == player && grid[8] == player);
}

static void grid_game_over_lines(Cell grid[NUM_CELLS], Cell player,
                                 int screen_size, Vector2 *line_start,
                                 Vector2 *line_end)
{
    const int square_size = screen_size / 3;

    if (grid[0] == player && grid[4] == player && grid[8] == player)
    {
        line_start->x = 0;
        line_start->y = 0;
        line_end->x = screen_size;
        line_end->y = screen_size;
        return;
    }

    if (grid[2] == player && grid[4] == player && grid[6] == player)
    {
        line_start->x = screen_size;
        line_start->y = 0;
        line_end->x = 0;
        line_end->y = screen_size;
        return;
    }

    for (int row = 0; row < 3; row++)
    {
        int base = row * 3;
        if (grid[base] == player && grid[base + 1] == player &&
            grid[base + 2] == player)
        {
            line_start->x = 0;
            line_start->y = row * square_size + square_size / 2;
            line_end->x = screen_size;
            line_end->y = row * square_size + square_size / 2;
            return;
        }
    }

    for (int col = 0; col < 3; col++)
    {
        if (grid[col] == player && grid[col + 3] == player &&
            grid[col + 6] == player)
        {
            line_start->x = col * square_size + square_size / 2;
            line_start->y = 0;
            line_end->x = col * square_size + square_size / 2;
            line_end->y = screen_size;
            return;
        }
    }
}

static bool grid_is_full(Cell grid[NUM_CELLS])
{
    for (size_t i = 0; i < NUM_CELLS; ++i)
    {
        if (grid[i] == Cell_Empty)
        {
            return false;
        }
    }

    return true;
}

static int grid_empty_indexes(Cell grid[NUM_CELLS], int indexes[NUM_CELLS])
{
    int num_indexes = 0;
    for (int i = 0; i < NUM_CELLS; ++i)
    {
        if (grid[i] == Cell_Empty)
        {
            indexes[num_indexes] = i;
            ++num_indexes;
        }
    }

    return num_indexes;
}

static int grid_minimax(Cell grid[NUM_CELLS], Cell player)
{
    Cell opponent = switch_player(player);
    if (grid_game_over(grid, opponent))
    {
        return opponent == Cell_O ? 1 : -1;
    }

    int score;
    int indexes[NUM_CELLS];
    const int num_indexes = grid_empty_indexes(grid, indexes);

    if (num_indexes == 0)
    {
        return 0;
    }

    if (player == Cell_O)
    {
        score = -10000;
        for (int i = 0; i < num_indexes; ++i)
        {
            grid[indexes[i]] = player;
            score = MAX(score, grid_minimax(grid, switch_player(player)));
            grid[indexes[i]] = Cell_Empty;
        }
    }
    else
    {
        score = 100000;
        for (int i = 0; i < num_indexes; ++i)
        {
            grid[indexes[i]] = player;
            score = MIN(score, grid_minimax(grid, switch_player(player)));
            grid[indexes[i]] = Cell_Empty;
        }
    }

    return score;
}

static void grid_update_with_minimax(Cell grid[NUM_CELLS], Cell player)
{
    int indexes[NUM_CELLS];
    const int num_indexes = grid_empty_indexes(grid, indexes);

    int best_score = -100000;
    int best_index = 0;
    for (int i = 0; i < num_indexes; ++i)
    {
        grid[indexes[i]] = player;

        if (grid_game_over(grid, player))
        {
            return;
        }

        int score = grid_minimax(grid, switch_player(player));
        if (score > best_score)
        {
            best_score = score;
            best_index = i;
        }

        grid[indexes[i]] = Cell_Empty;
    }

    grid[indexes[best_index]] = player;
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
    int game_over_frame_count;
    Vector2 game_over_line_start;
    Vector2 game_over_line_end;
    Cell grid[NUM_CELLS] = {0};
    Cell player_turn;

    const int square_size = screen_width / 3;

    RenderTexture2D texture_o = LoadRenderTexture(square_size, square_size);
    BeginTextureMode(texture_o);
    DrawCircle(square_size / 2, square_size / 2, square_size / 2.4, WHITE);
    DrawCircle(square_size / 2, square_size / 2, square_size / 2.8, BLACK);
    EndTextureMode();

    RenderTexture2D texture_x = LoadRenderTexture(square_size, square_size);
    BeginTextureMode(texture_x);
    {
        Vector2 start = {.x = 0, .y = 0};
        Vector2 end = {.x = square_size, .y = square_size};
        DrawLineEx(start, end, 12, WHITE);

        start.x = square_size;
        end.x = 0;
        DrawLineEx(start, end, 12, WHITE);
    }
    EndTextureMode();

    ///////////////////////////////////////////////////////////////////////////
    //                               Game Loop                               //
    ///////////////////////////////////////////////////////////////////////////
    while (!WindowShouldClose())
    {
        ///////////////////////////////
        // Update
        ///////////////////////////////
        Vector2 mouse_pos = GetMousePosition();
        const int m_x = (int)floorf(mouse_pos.x / (float)square_size);
        const int m_y = (int)floorf(mouse_pos.y / (float)square_size);
        const int m_i = m_y * 3 + m_x;

        switch (scene)
        {
        case Scene_Menu:
        {
            if (CheckCollisionPointRec(mouse_pos, menu_start_button))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
                    IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) ||
                    IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
                {
                    game_over_frame_count = -1;
                    scene = Scene_Game;

                    for (size_t i = 0; i < NUM_CELLS; ++i)
                    {
                        grid[i] = Cell_Empty;
                    }

                    player_turn = Cell_X;
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
            if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_SPACE) ||
                IsKeyPressed(KEY_ENTER))
            {
                scene = Scene_Menu;
            }

            if (game_over_frame_count >= 0)
            {
                if (game_over_frame_count == 100)
                {
                    scene = Scene_Menu;
                }

                ++game_over_frame_count;
            }
            else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
                     IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) ||
                     IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                if (grid[m_i] == Cell_Empty)
                {
                    grid[m_y * 3 + m_x] = player_turn;

                    if (grid_game_over(grid, player_turn))
                    {
                        game_over_frame_count = 0;
                        grid_game_over_lines(grid, player_turn, screen_width,
                                             &game_over_line_start,
                                             &game_over_line_end);
                    }
                    else if (grid_is_full(grid))
                    {
                        game_over_frame_count = 0;
                        player_turn = Cell_Empty;
                    }
                    else
                    {
                        grid_update_with_minimax(grid, Cell_O);
                        if (grid_game_over(grid, Cell_O))
                        {
                            game_over_frame_count = 0;
                            grid_game_over_lines(grid, Cell_O, screen_width,
                                                 &game_over_line_start,
                                                 &game_over_line_end);
                        }
                        else if (grid_is_full(grid))
                        {
                            game_over_frame_count = 0;
                            player_turn = Cell_Empty;
                        }
                    }
                }
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
            DrawLine(screen_width / 3, 0, screen_width / 3, screen_height,
                     WHITE);
            DrawLine(2 * screen_width / 3, 0, 2 * screen_width / 3,
                     screen_height, WHITE);

            DrawLine(0, screen_height / 3, screen_width, screen_height / 3,
                     WHITE);
            DrawLine(0, 2 * screen_height / 3, screen_width,
                     2 * screen_height / 3, WHITE);

            if (game_over_frame_count < 0 && m_i < NUM_CELLS &&
                grid[m_i] == Cell_Empty)
            {
                DrawRectangle(m_x * square_size, m_y * square_size, square_size,
                              square_size, DARKBLUE);
            }

            int x = 0, y = 0;
            for (size_t i = 0; i < NUM_CELLS; ++i, ++x)
            {
                if (x >= 3)
                {
                    ++y;
                    x = 0;
                }

                switch (grid[i])
                {
                case Cell_O:
                    DrawTexture(texture_o.texture, x * square_size,
                                y * square_size, WHITE);
                    break;
                case Cell_X:
                    DrawTexture(texture_x.texture, x * square_size,
                                y * square_size, WHITE);
                    break;
                case Cell_Empty:
                default:
                    break;
                }
            }

            if (game_over_frame_count >= 0 && player_turn != Cell_Empty)
            {
                DrawLineEx(game_over_line_start, game_over_line_end, 10, RED);
            }
            break;
        }
        default:
            assert(false);
        }

        EndDrawing();
    }

    UnloadRenderTexture(texture_o);
    UnloadRenderTexture(texture_x);
    CloseWindow();

    return 0;
}
