#include "picosystem.hpp"
using namespace picosystem;

struct vec_t
{
  int32_t x, y;
};

enum state_t
{
  PLAYING,
  GAME_OVER
};
state_t state = PLAYING;

// X is -1..1, Y is -5..5
vec_t lazy_random_direction()
{
  vec_t return_vect;
  return_vect.x = 0;
  while (return_vect.x == 0)
  {
    return_vect.x = (rand() % 3) - 1;
  }
  return_vect.y = (rand() % 5);
  if (rand() %2 == 0) return_vect.y = return_vect.y * -1;
  return_vect.y = 0;
  return return_vect;
}

// Paddle definitions
struct paddle
{
  int32_t y;
  int32_t x;
  int32_t speed;
  bool moving;
};

paddle paddle_one, paddle_two;
constexpr vec_t bounds{.x = 240, .y = 222};
constexpr int32_t paddle_width = 8;
constexpr int32_t paddle_height = 40;
constexpr int32_t max_speed = 100;
constexpr int32_t ball_radius = 2;

int32_t flip_direction(int32_t direction)
{
  return direction * -1;
}

// Ball Definitions
struct
{
  vec_t position;
  vec_t movement;
  int32_t speed;
} ball;

void init()
{
  state = PLAYING;
  srand((unsigned int)time());
  paddle_one = {.y = 0, .x = 0, .speed = 1, .moving = false};
  //Set up ball
  ball = {.position = {.x = bounds.x / 2, .y = bounds.y / 2}, .movement = lazy_random_direction(), .speed = 1};
}

void update_paddle_speed()
{
  if (button(UP) || button(DOWN))
  {
    paddle_one.moving = true;
  }
  else
  {
    paddle_one.moving = false;
  }
  if (paddle_one.moving)
  {
    paddle_one.speed = paddle_one.speed + 1;
    if (paddle_one.speed > max_speed)
      paddle_one.speed = max_speed;
  }
  else
  {
    paddle_one.speed = 1;
  }
}

void move_paddle_one()
{
  if (button(UP))
  {
    paddle_one.y -= paddle_one.speed;
  }
  else if (button(DOWN))
  {
    paddle_one.y += paddle_one.speed;
  }
  if (paddle_one.y < 0)
    paddle_one.y = 0;
  if (paddle_one.y > bounds.y - paddle_height)
    paddle_one.y = bounds.y - paddle_height;
}

void move_ball()
{
  // movement from velocity
  ball.position.x += (ball.movement.x * ball.speed);
  ball.position.y += (ball.movement.y * ball.speed);
  // velocity change from right boundary
  if (ball.position.x > bounds.x - ball_radius)
  {
    ball.position.x = bounds.x - ball_radius;
    ball.movement.x = flip_direction(ball.movement.x);
    ball.movement.y = flip_direction(ball.movement.y);
  }
  // velocity change from top boundary
  if (ball.position.y < ball_radius)
  {
    ball.movement.y = flip_direction(ball.movement.y);
    ball.position.y = ball_radius;
  }
  //velocity change from bottom boundary
  if (ball.position.y > bounds.y - ball_radius)
  {
    ball.movement.y = flip_direction(ball.movement.y);
    ball.position.y = bounds.y - ball_radius;
  }
  // velocity change from paddle one
  if (ball.position.x >= ball_radius && ball.position.x < paddle_width + ball_radius)
  {
    if ((paddle_one.y <= ball.position.y - ball_radius) && (ball.position.y < paddle_one.y + paddle_height + ball_radius))
    {
        int32_t difference = (ball.position.y - paddle_one.y);
      if (difference <= paddle_height / 2){
        difference = (paddle_height / 2) - difference;
      }
      else{
        difference = difference - (paddle_height / 2);
        difference = difference * -1;
      }
      difference = difference / 4;
      ball.movement.y += difference;
      ball.position.x = paddle_width + ball_radius;
      ball.movement.x = flip_direction(ball.movement.x);
      ball.movement.y = flip_direction(ball.movement.y);
      if (rand() % 2 == 0) ball.speed = ball.speed + 1;
    }
    else
    {
      state = GAME_OVER;
    }
  }
}

void update(uint32_t tick)
{
  if (state == PLAYING)
  {
    // Every 2 ticks, check if we're pushing a movement button, and change speed accordingly
    if (tick % 2 == 0)
    {
      update_paddle_speed();
    }

    move_paddle_one();
    move_ball();
  }
  else
  {
    if (pressed(A))
    {
      init();
    }
  }
}

void draw_paddle(int32_t x, int32_t y)
{
  frect(x, y, paddle_width, paddle_height);
}

void draw_ball()
{
  fcircle(ball.position.x, ball.position.y, ball_radius);
}

void draw_scoreboard()
{
  char position_text[50];

  sprintf(position_text, "ball position is %i,%i and momentum %i,%i", ball.position.x, ball.position.y, ball.movement.x, ball.movement.y);
  text(position_text);
}

void game_over()
{
  pen(0, 0, 0);
  clear();
  pen(15, 15, 15);
  text("GAME OVER", bounds.x / 2, (bounds.y / 2) - 4);
}

void draw(uint32_t tick)
{
  if (state == PLAYING)
  {
    pen(0, 0, 0);
    clear();
    pen(15, 15, 15);
    draw_paddle(paddle_one.x, paddle_one.y);
    draw_ball();
    draw_scoreboard();
  }
  else
  {
    game_over();
  }
}