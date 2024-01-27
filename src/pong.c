#include "raylib.h"

#define W_WIDTH 1280
#define W_HEIGHT 720
#define FPS 60
#define PADDLE_HEIGHT 100
#define PADDLE_WIDTH 20
#define PUCK_SPEED 4.5
#define PADDLE_SPEED 6
#define FONT_SIZE 36

struct puck { 
    Vector2 p;
    float r;
    float sp_x;
    float sp_y;
};

/* Draws the playing field with scores. */
void draw_field(int *score1, int *score2);

/* Updates puck position and adds score if puck is in a goal. */
void puck_update(struct puck *puck, int *score1, int *score2);

/* Gets user input and calls paddle_move to move the paddle. */
void paddles_update(struct Rectangle *pad1, struct Rectangle *pad2);

/* Moves the paddle in the given direction. */
void paddle_move(struct Rectangle *pad, float speed);

/* Checks if the puck touches a paddle, if so changes the puck's direction. */
void check_collisions(struct puck *puck, struct Rectangle *pad1, struct Rectangle *pad2);

int main(void)
{
    struct puck puck = {
        .p.x = W_WIDTH / 2,
        .p.y = W_HEIGHT / 2,
        .r = 10.0,
        .sp_x = PUCK_SPEED,
        .sp_y = PUCK_SPEED
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

    InitWindow(W_WIDTH, W_HEIGHT, "Pong");
    SetTargetFPS(FPS);

    while(!WindowShouldClose()) {
        BeginDrawing();
            draw_field(&score1, &score2);
            check_collisions(&puck, &paddle1, &paddle2);
            puck_update(&puck, &score1, &score2);
            paddles_update(&paddle1, &paddle2);
        EndDrawing();
    }
    return 0;
}

void draw_field(int *score1, int *score2)
{
    ClearBackground(BLACK);
    DrawText(TextFormat("%d", *score1), PADDLE_WIDTH * 3, PADDLE_WIDTH, FONT_SIZE, RED);
    DrawText(TextFormat("%d", *score2), W_WIDTH - PADDLE_WIDTH * 3, PADDLE_WIDTH, FONT_SIZE, BLUE);
    DrawLine(W_WIDTH / 2, 0, W_WIDTH / 2, W_HEIGHT, WHITE);
    DrawCircleLines(W_WIDTH / 2, W_HEIGHT / 2, 150, WHITE);
}

void puck_update(struct puck *puck, int *score1, int *score2)
{
    if (puck->p.y < puck->r || puck->p.y > W_HEIGHT - puck->r)
        puck->sp_y = -(puck->sp_y);

    if (puck->p.x < 0) {
        *score2 += 1;
        puck->p.x = W_WIDTH / 2;
        puck->p.y = W_HEIGHT / 2;
    } 
    if (puck->p.x > W_WIDTH) {
        *score1 += 1;
        puck->p.x = W_WIDTH / 2;
        puck->p.y = W_HEIGHT / 2;
    }
    puck->p.x += puck->sp_x;
    puck->p.y += puck->sp_y;
    DrawCircleV(puck->p, puck->r, WHITE);
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
    
    DrawRectangle(pad1->x, pad1->y, pad1->width, pad1->height, RED);
    DrawRectangle(pad2->x, pad2->y, pad2->width, pad2->height, BLUE);
}

void paddle_move(struct Rectangle *pad, float speed)
{
    pad->y += speed;
    if (pad->y <= 0)
        pad->y = 0;
    if (pad->y >= W_HEIGHT - pad->height)
        pad->y = W_HEIGHT - pad->height;     
}

void check_collisions(struct puck *puck, struct Rectangle *pad1, struct Rectangle *pad2)
{
    if (CheckCollisionCircleRec(puck->p, puck->r, *pad1)) {
        puck->sp_x = -(puck->sp_x);
        puck->p.x = pad1->x + PADDLE_WIDTH + (puck->sp_x * 2);
    }
    if (CheckCollisionCircleRec(puck->p, puck->r, *pad2)) {
        puck->sp_x = -(puck->sp_x);
        puck->p.x = pad2->x + (puck->sp_x * 2);
    }
}
