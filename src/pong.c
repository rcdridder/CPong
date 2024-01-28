#include "raylib.h"

#define W_WIDTH 1280
#define W_HEIGHT 720
#define FPS 60
#define PADDLE_HEIGHT 120
#define PADDLE_WIDTH 20
#define PUCK_SPEED 6
#define PADDLE_SPEED 6
#define P1_COLOUR RED
#define P2_COLOUR BLUE

struct puck { 
    Vector2 p;
    float r;
    float sp_x;
    float sp_y;
};

enum gamescreen { ROUND_START, GAMEPLAY };

/* Updates score and returns true if it's a goal. */
bool puck_goal(struct puck *puck, Sound *score_sound, int *score1, int *score2);

/* Resets the puck and choose a serve direction */
void puck_reset(struct puck *puck);

/* Updates puck position. */
void puck_update(struct puck *puck, Sound *wall_sound);

/* Checks if the puck touches a paddle, if so changes the puck's direction. */
void check_collisions(struct puck *puck, struct Rectangle *pad1, struct Rectangle *pad2, Sound *paddle_sound);

/* Gets user input and calls paddle_move to move the paddle. */
void paddles_update(struct Rectangle *pad1, struct Rectangle *pad2);

/* Moves the paddle in the given direction. */
void paddle_move(struct Rectangle *pad, float speed);

/* Draws the playing field with scores. */
void draw_field(int *score1, int *score2);

int main(void)
{
    enum gamescreen current_screen = ROUND_START;
    char round_start_msg[] = "Press enter to start a new round";
    
    float start_angle = 0;
    while(!start_angle)
        start_angle = GetRandomValue(-2, 2);

    struct puck puck = {
        .p.x = W_WIDTH / 2,
        .p.y = W_HEIGHT / 2,
        .r = 10.0,
        .sp_x = PUCK_SPEED,
        .sp_y = (float)GetRandomValue(-2, 2)
    };

    struct Rectangle paddle1 = {
        .height = PADDLE_HEIGHT,
        .width = PADDLE_WIDTH,
        .x = PADDLE_WIDTH,
        .y = (W_HEIGHT / 2) - (PADDLE_HEIGHT / 2),
    };

    struct Rectangle paddle2 = {
        .height = PADDLE_HEIGHT,
        .width = PADDLE_WIDTH,
        .x = W_WIDTH - (PADDLE_WIDTH * 2),
        .y = (W_HEIGHT / 2) - (PADDLE_HEIGHT / 2),
    };

    int score1 = 0, score2 = 0;

    char start_loc[] = "./resources/start.wav";
    char paddle_loc[] = "./resources/hit.wav";
    char goal_loc[] = "./resources/goal.wav";
    char wall_loc[] = "./resources/wall.wav";

    SetTargetFPS(FPS);
    InitWindow(W_WIDTH, W_HEIGHT, "Pong");
    
    InitAudioDevice();
    Sound start_sound = LoadSound(start_loc);
    Sound paddle_sound = LoadSound(paddle_loc); 
    Sound score_sound = LoadSound(goal_loc);
    Sound wall_sound = LoadSound(wall_loc);

    while(!WindowShouldClose()) {
        //Update
        switch (current_screen) {
            case ROUND_START:
            {
                if (IsKeyPressed(KEY_ENTER)) {
                    current_screen = GAMEPLAY;
                    paddle1.y = (W_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
                    paddle2.y = (W_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
                    PlaySound(start_sound);
                }
            } break;
            case GAMEPLAY:
            {
                if (puck_goal(&puck, &score_sound, &score1, &score2)) {
                    puck_reset(&puck);
                    current_screen = ROUND_START;
                }
                check_collisions(&puck, &paddle1, &paddle2, &paddle_sound);
                puck_update(&puck, &wall_sound);
                paddles_update(&paddle1, &paddle2);
            } break;
            default: break;
        }
        //Draw
        BeginDrawing();
        
        ClearBackground(BLACK);
        switch (current_screen) {
            case ROUND_START:
            {
                draw_field(&score1, &score2);
                DrawRectangle(W_WIDTH * 0.25, W_HEIGHT * 0.25, W_WIDTH * 0.5, W_HEIGHT * 0.5, WHITE);
                DrawText(round_start_msg, W_WIDTH * 0.275, W_HEIGHT * 0.5, 33, BLACK);
            } break;
            case GAMEPLAY:
            {
                draw_field(&score1, &score2);
                DrawCircleV(puck.p, puck.r, WHITE);
                DrawRectangle(paddle1.x, paddle1.y, paddle1.width, paddle1.height, P1_COLOUR);
                DrawRectangle(paddle2.x, paddle2.y, paddle2.width, paddle2.height, P2_COLOUR);
            } break;
            default: break;
        }

        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

bool puck_goal(struct puck *puck, Sound *score_sound, int *score1, int *score2)
{
     if (puck->p.x < 0) {
        PlaySound(*score_sound);
        *score2 += 1;
        return true;
    } 
    if (puck->p.x > W_WIDTH) {
        PlaySound(*score_sound);
        *score1 += 1;
        return true;
    }
    return false;
}

void puck_reset(struct puck *puck)
{
    float angle = 0;
    int dir = GetRandomValue(0, 1);

    while(!angle)
        angle = GetRandomValue(-2, 2);

    puck->p.x = W_WIDTH / 2;
    puck->p.y = W_HEIGHT / 2;
    if (dir == 1)
        puck->sp_x = PUCK_SPEED;
    else
        puck->sp_x = -PUCK_SPEED;
    puck->sp_y = angle;
}

void puck_update(struct puck *puck, Sound *wall_sound)
{
    if (puck->p.y < puck->r || puck->p.y > W_HEIGHT - puck->r) {
        PlaySound(*wall_sound);
        puck->sp_y = -(puck->sp_y);
    }
    puck->p.x += puck->sp_x;
    puck->p.y += puck->sp_y;
}

void check_collisions(struct puck *puck, struct Rectangle *pad1, struct Rectangle *pad2, Sound *paddle_sound)
{
    if (CheckCollisionCircleRec(puck->p, puck->r, *pad1)) {
        puck->sp_x = -(puck->sp_x * 1.05);
        puck->p.x = pad1->x + PADDLE_WIDTH + (puck->sp_x * 2);
        puck->sp_y *= 1.1;
        PlaySound(*paddle_sound);
    }
    if (CheckCollisionCircleRec(puck->p, puck->r, *pad2)) {
        puck->sp_x = -(puck->sp_x * 1.1);
        puck->p.x = pad2->x + (puck->sp_x * 2);
        puck->sp_y *= 1.1;
        PlaySound(*paddle_sound);
    }
}

void paddles_update(struct Rectangle *pad1, struct Rectangle *pad2)
{
    if (IsKeyDown('W'))
        paddle_move(pad1, -PADDLE_SPEED);
    if (IsKeyDown('A'))
        paddle_move(pad1, PADDLE_SPEED);
    if (IsKeyDown('O'))
        paddle_move(pad2, -PADDLE_SPEED);
    if (IsKeyDown('L'))
        paddle_move(pad2, PADDLE_SPEED);
}

void paddle_move(struct Rectangle *pad, float speed)
{
    pad->y += speed;
    if (pad->y <= 0)
        pad->y = 0;
    if (pad->y >= W_HEIGHT - pad->height)
        pad->y = W_HEIGHT - pad->height;     
}

void draw_field(int *score1, int *score2)
{
    DrawText(TextFormat("%d", *score1), PADDLE_WIDTH * 3, PADDLE_WIDTH, 36, P1_COLOUR);
    DrawText(TextFormat("%d", *score2), W_WIDTH - PADDLE_WIDTH * 3, PADDLE_WIDTH, 36, P2_COLOUR);
    DrawLine(W_WIDTH / 2, 0, W_WIDTH / 2, W_HEIGHT, WHITE);
    DrawCircleLines(W_WIDTH / 2, W_HEIGHT / 2, 150, WHITE);
}
