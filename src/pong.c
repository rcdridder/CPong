#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "pong.h"

#define W_WIDTH 1280
#define W_HEIGHT 720
#define FPS 60

#define PUCK_RADIUS 10
#define PUCK_SPEED 6

#define PADDLE_HEIGHT 120
#define PADDLE_WIDTH 20
#define PADDLE_SPEED 6

#define P1_COLOUR RED
#define P2_COLOUR BLUE

int main(void)
{
    /* Global variables */
    enum gamescreen current_screen = ROUND_START;

    /* ROUND_START variables */
    char round_start_msg[] = "Press enter to start a new round";

    /* GAMEPLAY variables */
    srand(time(NULL));

    struct puck puck = {
        .radius = PUCK_RADIUS,
        .pos.x = W_WIDTH / 2,
        .pos.y = W_HEIGHT / 2,
        .vel.x = PUCK_SPEED * cos(puck_serve_angle()),
        .vel.y = PUCK_SPEED * sin(puck_serve_angle()),
    };

    struct paddle pad1 = {
        .top.x = PADDLE_WIDTH * 2,
        .top.y = (W_HEIGHT / 2) - (PADDLE_HEIGHT / 2),
        .bottom.x = PADDLE_WIDTH * 2,
        .bottom.y = (W_HEIGHT / 2) + (PADDLE_HEIGHT / 2)
    };

    struct paddle pad2 = {
        .top.x = W_WIDTH - (PADDLE_WIDTH * 2),
        .top.y = (W_HEIGHT / 2) - (PADDLE_HEIGHT / 2),
        .bottom.x = W_WIDTH - (PADDLE_WIDTH * 2),
        .bottom.y = (W_HEIGHT / 2) + (PADDLE_HEIGHT / 2)
    };

    int p1_score = 0, p2_score = 0;

    bool wall_coll;
    float pad_coll, puck_speed;

    /* Initialization */
    SetTargetFPS(FPS);
    InitWindow(W_WIDTH, W_HEIGHT, "Pong");
    InitAudioDevice();

    Sound sounds[] = {
        LoadSound("./resources/start.wav"),
        LoadSound("./resources/hit.wav"),
        LoadSound("./resources/goal.wav"),
        LoadSound("./resources/wall.wav")
    };

    /* Gameloop */
    while(!WindowShouldClose()) {
        //Update
        switch (current_screen) {
            case ROUND_START:
            {
                if (IsKeyPressed(KEY_ENTER)) {
                    current_screen = GAMEPLAY;
                    puck_speed = PUCK_SPEED;
                    puck_reset(&puck);
                    paddle_reset(&pad1);
                    paddle_reset(&pad2);
                    PlaySound(sounds[0]);
                }
            } break;
            case GAMEPLAY:
            {
                if(puck_goal(&puck, &sounds[2], &p1_score, &p2_score))
                    current_screen = ROUND_START;
                wall_coll = puck_wall_coll(&puck);
                pad_coll = puck_pad_coll(&puck, &pad1, &pad2);
                puck_update(&puck, wall_coll, pad_coll, &puck_speed, &sounds[3], &sounds[1]);
                paddles_update(&pad1, &pad2);
            } break;
            default: break;
        }
        //Draw
        BeginDrawing();
            ClearBackground(BLACK);
            switch (current_screen) {
                case ROUND_START:
                {
                    draw_field(&p1_score, &p2_score);
                    DrawRectangle(W_WIDTH * 0.25, W_HEIGHT * 0.25, W_WIDTH * 0.5, W_HEIGHT * 0.5, WHITE);
                    DrawText(round_start_msg, W_WIDTH * 0.275, W_HEIGHT * 0.5, 33, BLACK);
                } break;
                case GAMEPLAY:
                {
                    draw_field(&p1_score, &p2_score);
                    DrawCircleV(puck.pos, puck.radius, WHITE);
                    DrawRectangle(pad1.top.x - PADDLE_WIDTH, pad1.top.y, PADDLE_WIDTH, PADDLE_HEIGHT, P1_COLOUR);
                    DrawRectangle(pad2.top.x, pad2.top.y, PADDLE_WIDTH, PADDLE_HEIGHT, P2_COLOUR);
                } break;
                default: break;
            }
        EndDrawing();
    }

    /* Breakdown*/
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

void paddle_move(struct paddle *pad, float speed)
{
    pad->top.y += speed;
    pad->bottom.y += speed;
    if (pad->top.y <= 0) {
        pad->top.y = 0;
        pad->bottom.y = pad->top.y + PADDLE_HEIGHT;
    }
    if (pad->bottom.y >= W_HEIGHT) {
        pad->bottom.y = W_HEIGHT;
        pad->top.y = pad->bottom.y - PADDLE_HEIGHT;
    }
}

void paddle_reset(struct paddle *pad)
{
    pad->top.y = (W_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
    pad->bottom.y = (W_HEIGHT / 2) + (PADDLE_HEIGHT / 2);
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

bool puck_goal(struct puck *puck, Sound *goal, int *p1_score, int *p2_score)
{
    if (puck->pos.x < 0) {
        PlaySound(*goal);
        *p2_score += 1;
        return true;
    } else if (puck-> pos.x > W_WIDTH) {
        PlaySound(*goal);
        *p1_score += 1;
        return true;
    } else {
        return false;
    }
}

float puck_pad_coll(struct puck *puck, struct paddle *pad1, struct paddle *pad2)
{
    float puck_y = puck->pos.y;
    float p1_segement = pad1->top.y;
    float p2_segment = pad2->top.y;

    //Check pad1
    if (puck->pos.x - puck->radius <= pad1->top.x && puck_y >= pad1->top.y && puck_y <= pad1->bottom.y)  {
        if (puck_y < (p1_segement += (PADDLE_HEIGHT / 8)))
            return -my_pi / 4;
        else if (puck_y < (p1_segement += (PADDLE_HEIGHT / 8)))
            return -my_pi / 6;
        else if (puck_y < (p1_segement += (PADDLE_HEIGHT / 8)))
            return -my_pi / 12;
        else if (puck_y < (p1_segement += ((PADDLE_HEIGHT / 8) * 3)))
            return 2 * my_pi;
        else if (puck_y < (p1_segement += (PADDLE_HEIGHT / 8)))
            return  my_pi / 12;
        else if (puck_y < (p1_segement += (PADDLE_HEIGHT / 8)))
            return my_pi / 6;
        else {
            return my_pi / 4;
        }
    } 
    //Check pad2
    if (puck->pos.x + puck->radius >= pad2->top.x && puck_y >= pad2->top.y && puck_y <= pad2->bottom.y) {
        if (puck_y < (p2_segment += (PADDLE_HEIGHT / 8)))
            return -((3 * my_pi) / 4);
        else if (puck_y < (p2_segment += (PADDLE_HEIGHT / 8)))
            return -((5 * my_pi) / 6);
        else if (puck_y < (p2_segment += (PADDLE_HEIGHT / 8)))
            return -((11 * my_pi) / 12);
        else if (puck_y < (p2_segment += ((PADDLE_HEIGHT / 8) * 3)))
            return my_pi;
        else if (puck_y < (p2_segment += (PADDLE_HEIGHT / 8)))
            return (11 * my_pi) / 12;
        else if (puck_y < (p2_segment += (PADDLE_HEIGHT / 8)))
            return (5 * my_pi) / 6;
        else
            return (3 * my_pi) / 4;
    }

    return 0;
}

void puck_reset(struct puck *puck)
{
        puck->pos.x = W_WIDTH / 2;
        puck->pos.y = W_HEIGHT / 2;
        puck->vel.x = PUCK_SPEED * cos(puck_serve_angle());
        puck->vel.y = PUCK_SPEED * sin(puck_serve_angle());
        if (GetRandomValue(0, 1) == 1) {
            puck->vel.x *= -1;
            puck->vel.y *= -1;
        }
}

float puck_serve_angle(void)
{
    return ((float)rand() / ((float) RAND_MAX)) * (((my_pi / 4) - (-my_pi / 4)) + (-my_pi / 4));  
}

void puck_update(struct puck *puck, bool wall_coll, float pad_coll, float *puck_speed, Sound *wall, Sound *pad)
{
    if (wall_coll) {
        puck->vel.y *= -1;
        PlaySound(*wall);
    }
    if (pad_coll != 0) {
        *puck_speed *= 1.1;
        puck->vel.x = (*puck_speed * cos(pad_coll));
        puck->vel.y = (*puck_speed * sin(pad_coll));
        PlaySound(*pad);
    }

    puck->pos.x += puck->vel.x;
    puck->pos.y += puck->vel.y;
}

bool puck_wall_coll(struct puck *puck)
{
    float puck_top = puck->pos.y - puck->radius;
    float puck_bottom = puck->pos.y + puck->radius;

    if (puck_top < 0 || puck_bottom > W_HEIGHT)
        return true;
    else
        return false;
}

void draw_field(int *score1, int *score2)
{
    DrawText(TextFormat("%d", *score1), PADDLE_WIDTH * 3, PADDLE_WIDTH, 36, P1_COLOUR);
    DrawText(TextFormat("%d", *score2), W_WIDTH - PADDLE_WIDTH * 3, PADDLE_WIDTH, 36, P2_COLOUR);
    DrawLine(W_WIDTH / 2, 0, W_WIDTH / 2, W_HEIGHT, WHITE);
    DrawCircleLines(W_WIDTH / 2, W_HEIGHT / 2, 150, WHITE);
}
