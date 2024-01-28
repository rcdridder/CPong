#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"

#define W_WIDTH 1280
#define W_HEIGHT 720
#define FPS 60
#define PADDLE_HEIGHT 120
#define PADDLE_WIDTH 20
#define PADDLE_SEG (PADDLE_HEIGHT / 8)
#define PUCK_SPEED 6
#define PADDLE_SPEED 6
#define P1_COLOUR RED
#define P2_COLOUR BLUE

struct puck { 
    Vector2 center;
    float r;
    Vector2 left;
    Vector2 right;
    float angle;
    float sp_x;
    float sp_y;
};

struct paddle {
    Rectangle r;
    Vector2 top;
    Vector2 bottom;
};

enum gamescreen { ROUND_START, GAMEPLAY };

/* Returns the start angle for puck. */
float get_angle(void);

/* Updates score and returns true if it's a goal. */
bool puck_goal(struct puck *puck, Sound *score_sound, int *score1, int *score2);

/* Resets the puck and choose a serve direction */
void puck_reset(struct puck *puck, float angle);

/* Updates puck position. */
void puck_update(struct puck *puck, Sound *wall_sound);

/* Checks if the puck touches a paddle, if so changes the puck's direction. */
void check_collisions(struct puck *puck, struct paddle *pad1, struct paddle *pad2, Sound *paddle_sound);

/* Gets user input and calls paddle_move to move the paddle. */
void paddles_update(struct paddle *pad1, struct paddle *pad2);

/* Moves the paddle in the given direction. */
void paddle_move(struct paddle *pad, float speed);

/* Draws the playing field with scores. */
void draw_field(int *score1, int *score2);

int main(void)
{
    enum gamescreen current_screen = ROUND_START;
    char round_start_msg[] = "Press enter to start a new round";

    struct puck puck = {
        .center.x = W_WIDTH / 2,
        .center.y = W_HEIGHT / 2,
        .r = 10.0,
        .left.x = puck.center.x - (puck.r / 2),
        .left.y = puck.center.y,
        .right.x = puck.center.x + (puck.r / 2),
        .right.y = puck.center.y,
        .sp_x = PUCK_SPEED * cos((double) get_angle()),
        .sp_y = PUCK_SPEED * sin((double) get_angle())
    };

    struct paddle paddle1 = {
        .r.height = PADDLE_HEIGHT,
        .r.width = PADDLE_WIDTH,
        .r.x = PADDLE_WIDTH,
        .r.y = (W_HEIGHT / 2) - (PADDLE_HEIGHT / 2),
        .top.x = paddle1.r.x + PADDLE_WIDTH,
        .top.y = paddle1.r.y,
        .bottom.x = paddle1.r.x + PADDLE_WIDTH,
        .bottom.y = paddle1.r.y + PADDLE_HEIGHT
    };

    struct paddle paddle2 = {
        .r.height = PADDLE_HEIGHT,
        .r.width = PADDLE_WIDTH,
        .r.x = W_WIDTH - (PADDLE_WIDTH * 2),
        .r.y = (W_HEIGHT / 2) - (PADDLE_HEIGHT / 2),
        .top.x = paddle2.r.x,
        .top.y = paddle2.r.y,
        .bottom.x = paddle2.r.x,
        .bottom.y = paddle2.r.y + PADDLE_HEIGHT
    };

    int score1 = 0, score2 = 0;

    char start_loc[] = "./resources/start.wav";
    char paddle_loc[] = "./resources/hit.wav";
    char goal_loc[] = "./resources/goal.wav";
    char wall_loc[] = "./resources/wall.wav";

    srand((unsigned) time(NULL));

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
                    paddle1.r.y = (W_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
                    paddle2.r.y = (W_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
                    paddle1.top.y = paddle1.r.y;
                    paddle1.bottom.y = paddle1.r.y + PADDLE_HEIGHT;
                    paddle2.top.y = paddle2.r.y;
                    paddle2.bottom.y = paddle2.r.y + PADDLE_HEIGHT;
                    PlaySound(start_sound);
                }
            } break;
            case GAMEPLAY:
            {
                if (puck_goal(&puck, &score_sound, &score1, &score2)) {
                    puck_reset(&puck, get_angle());
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
                DrawCircleV(puck.center, puck.r, WHITE);
                DrawRectangle(paddle1.r.x, paddle1.r.y, paddle1.r.width, paddle1.r.height, P1_COLOUR);
                DrawRectangle(paddle2.r.x, paddle2.r.y, paddle2.r.width, paddle2.r.height, P2_COLOUR);
            } break;
            default: break;
        }

        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

float get_angle(void)
{
    return (float)rand()/(float)(RAND_MAX / 360);
}

bool puck_goal(struct puck *puck, Sound *score_sound, int *score1, int *score2)
{
     if (puck->center.x < 0) {
        PlaySound(*score_sound);
        *score2 += 1;
        return true;
    } 
    if (puck->center.x > W_WIDTH) {
        PlaySound(*score_sound);
        *score1 += 1;
        return true;
    }
    return false;
}

void puck_reset(struct puck *puck, float angle)
{
    puck->center.x = W_WIDTH / 2;
    puck->center.y = W_HEIGHT / 2;
    puck->left.x = puck->center.x - (puck->r / 2);
    puck->left.y = puck->center.y;
    puck->right.x = puck->center.x + (puck->r / 2);
    puck->right.y = puck->center.y;
    puck->sp_x = PUCK_SPEED * cos((double)angle);
    puck->sp_y = PUCK_SPEED * sin((double)angle); 
}

void puck_update(struct puck *puck, Sound *wall_sound)
{
    if (puck->center.y < puck->r || puck->center.y > W_HEIGHT - puck->r) {
        PlaySound(*wall_sound);
        puck->sp_y = -(puck->sp_y);
    }
    puck->center.x += puck->sp_x;
    puck->center.y += puck->sp_y;
    puck->left.x += puck->sp_x;
    puck->left.y += puck->sp_y;
    puck->right.x += puck->sp_x;
    puck->right.y += puck->sp_y;
}

void check_collisions(struct puck *puck, struct paddle *pad1, struct paddle *pad2, Sound *paddle_sound)
{
    float segments[8];

    //left paddle
    if (puck->left.x <= pad1->top.x && 
        puck->left.y > pad1->top.y &&
        puck->left.y < pad1->bottom.y) {
            for (int i = 0, p_seg = PADDLE_SEG; i < 7; i++, p_seg += PADDLE_SEG) {
                segments[i] = pad1->top.y + p_seg;
            }

            if (puck->left.y <= segments[0]) {
                puck->sp_x = PUCK_SPEED * cos((double)135);
                puck->sp_y = PUCK_SPEED * sin((double)135); 
            } else if (puck->left.y <= segments[1]) {
                puck->sp_x = PUCK_SPEED * cos((double)120);
                puck->sp_y = PUCK_SPEED * sin((double)120);
            } else if (puck->left.y <= segments[2]) {
                puck->sp_x = PUCK_SPEED * cos((double)105);
                puck->sp_y = PUCK_SPEED * sin((double)105);
            } else if (puck->left.y <= segments[4]) {
                puck->sp_x = PUCK_SPEED * cos((double)90);
                puck->sp_y = PUCK_SPEED * sin((double)90);
            } else if (puck->left.y <= segments[5]) {
                puck->sp_x = PUCK_SPEED * cos((double)75);
                puck->sp_y = PUCK_SPEED * sin((double)75);
            } else if (puck->left.y <= segments[6]) {
                puck->sp_x = PUCK_SPEED * cos((double)60);
                puck->sp_y = PUCK_SPEED * sin((double)60);
            } else if (puck->left.y <= segments[7]) {
                puck->sp_x = PUCK_SPEED * cos((double)45);
                puck->sp_y = PUCK_SPEED * sin((double)45);
            }
            puck->sp_x *= 1.05;
            puck->center.x = pad1->r.x + PADDLE_WIDTH + (puck->sp_x * 2);
            puck->left.x = pad1->r.x  + PADDLE_WIDTH + (puck->sp_x * 2);
            puck->right.x = pad1->r.x + PADDLE_WIDTH + (puck->sp_x * 2);
            puck->sp_y *= 1.1;
            PlaySound(*paddle_sound);
        }
    
    //right paddle
    if (puck->right.x >= pad2->top.x &&
        puck->right.y > pad2->top.y &&
        puck->right.y < pad2->bottom.y) {
            puck->sp_x = -(puck->sp_x * 1.1);
            puck->center.x = pad2->r.x + (puck->sp_x * 2);
            puck->left.x = pad2->r.x + (puck->sp_x * 2);
            puck->right.x = pad2->r.x + (puck->sp_x * 2);
            puck->sp_y *= 1.1;
        PlaySound(*paddle_sound);
        }
    // if (CheckCollisionCircleRec(puck->center, puck->r, pad1->r)) {
    //     puck->sp_x = -(puck->sp_x * 1.05);
    //     puck->center.x = pad1->r.x + PADDLE_WIDTH + (puck->sp_x * 2);
    //     puck->sp_y *= 1.1;
    //     PlaySound(*paddle_sound);
    // }
    // if (CheckCollisionCircleRec(puck->center, puck->r, pad2->r)) {
    //     puck->sp_x = -(puck->sp_x * 1.1);
    //     puck->center.x = pad2->r.x + (puck->sp_x * 2);
    //     puck->sp_y *= 1.1;
    //     PlaySound(*paddle_sound);
    // }
}

void paddles_update(struct paddle *pad1, struct paddle *pad2)
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

void paddle_move(struct paddle *pad, float speed)
{
    pad->r.y += speed;
    pad->top.y += speed;
    pad->bottom.y += speed;
    if (pad->r.y <= 0) {
        pad->r.y = 0;
        pad->top.y = pad->r.y;
        pad->bottom.y = pad->r.y + PADDLE_HEIGHT; 
    }
    if (pad->r.y >= W_HEIGHT - pad->r.height) {
        pad->r.y = W_HEIGHT - pad->r.height;
        pad->top.y = pad->r.y;
        pad->bottom.y = pad->r.y + PADDLE_HEIGHT;
    }
}

void draw_field(int *score1, int *score2)
{
    DrawText(TextFormat("%d", *score1), PADDLE_WIDTH * 3, PADDLE_WIDTH, 36, P1_COLOUR);
    DrawText(TextFormat("%d", *score2), W_WIDTH - PADDLE_WIDTH * 3, PADDLE_WIDTH, 36, P2_COLOUR);
    DrawLine(W_WIDTH / 2, 0, W_WIDTH / 2, W_HEIGHT, WHITE);
    DrawCircleLines(W_WIDTH / 2, W_HEIGHT / 2, 150, WHITE);
}
