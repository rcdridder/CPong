#ifndef PONG_C
#define PONG_C

#include "raylib.h"

enum gamescreen { ROUND_START, GAMEPLAY };

struct puck {
    float radius;
    Vector2 pos;
    Vector2 vel;
};

struct paddle {
    Vector2 top;
    Vector2 bottom;
};

const float my_pi = 3.14159265359;

/* Draws the playing field with scores. */
void draw_field(int *score1, int *score2);

/* Moves the paddle in the given direction. */
void paddle_move(struct paddle *pad, float speed);

/* Resets the paddle to the starting position. */
void paddle_reset(struct paddle *pad);

/* Gets user input and calls paddle_move to move the paddle. */
void paddles_update(struct paddle *pad1, struct paddle *pad2);

/* Returns true if puck is in a goal and updates corresponding score. */
bool puck_goal(struct puck *puck, Sound *goal, int *p1_score, int *p2_score);

/* Returns a radians if the puck collides with a pad. Returns 0 when there's no collision. */
float puck_pad_coll(struct puck *puck, struct paddle *pad1, struct paddle *pad2);

/* Resets the puck to the starting position and generates new serve angle */
void puck_reset(struct puck *puck);

/* Returns a random serve angle between 0 and 45 degrees. */
float puck_serve_angle(void);

/* Updates the puck position and velocity. */
/* Increases puck speed each time a pad is hit until a round finishes. */
/* Plays sounds in case of collision. */
void puck_update(struct puck *puck, bool wall_coll, float pad_coll, float *puck_speed, Sound *wall, Sound *pad);

/* Returns true if puck touches a wall. */
bool puck_wall_coll(struct puck *puck);


#endif