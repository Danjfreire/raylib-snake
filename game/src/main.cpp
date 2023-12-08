#include "raylib.h"
#include "string"
#include "iostream"

// data types
struct Snake
{
	Vector2 position;
	Vector2 speed;
	Color color;
};

struct Food
{
	Vector2 position;
	Color color;
};

// screen config
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TARGET_FPS = 60;

// game config 
const Color SNAKE_COLOR = GetColor(0x080002ff);
const Color BACKGROUND_COLOR = GetColor(0x9ACC99ff);
const int SQUARE_SIZE = 16;
const int WALL_OFFSET = 16;
const int DEFAULT_FONT_SIZE = 24;
const int MAX_SNAKE_LENGTH = 200;
const char* GAME_OVER_TEXT = "Game Over";
const char* RESTART_GAME_TEXT = "Press R to restart";
const Vector2 SNAKE_STARTING_POSITION = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2 };

// game state
bool isGameOver;
int score;
std::string scoreText;
bool shouldSpawnFood;

Food fruit = { 0 };
Snake snake[MAX_SNAKE_LENGTH] = { 0 };
Vector2 snakePreviousPosition[MAX_SNAKE_LENGTH] = { 0 };
int snakeLength;
bool canSnakeMove;

int frameCounter = 0;

void InitGame();
void Tick();
void DrawGame();
void UpdateGame();
void UpdateSnake();
void HandleInput();
void HandleCollision();
void SpawnFood();

int main(void)
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");
	SetTargetFPS(TARGET_FPS);

	InitGame();

	while (!WindowShouldClose())
	{
		Tick();
	}

	return 0;
}

void InitGame()
{
	isGameOver = false;
	score = 0;
	snakeLength = 3;
	canSnakeMove = true;

	// init snake arrays with default values
	for (int i = 0; i < MAX_SNAKE_LENGTH; i++)
	{
		snake[i].position = { 0 , 0 };
		snake[i].speed = { SQUARE_SIZE, 0 };
		snake[i].color = SNAKE_COLOR;
		snakePreviousPosition[i] = { 0.0f, 0.0f };
	}

	// init initial snake position
	for (int i = 0; i < snakeLength; i++)
	{
		snake[i].position = { SNAKE_STARTING_POSITION.x - i, SNAKE_STARTING_POSITION.y - i };
	}

	fruit.color = BLACK;

	shouldSpawnFood = true;
}

void Tick()
{
	UpdateGame();
	DrawGame();
	frameCounter++;
}

void DrawGame()
{
	BeginDrawing();

	ClearBackground(BACKGROUND_COLOR);

	if (isGameOver)
	{
		DrawText(
			GAME_OVER_TEXT,
			SCREEN_WIDTH / 2 - MeasureText(GAME_OVER_TEXT, DEFAULT_FONT_SIZE) / 2,
			SCREEN_HEIGHT / 2, DEFAULT_FONT_SIZE,
			SNAKE_COLOR
		);

		scoreText = "Score: " + std::to_string(score);

		DrawText(
			scoreText.c_str(),
			SCREEN_WIDTH / 2 - MeasureText(scoreText.c_str(), DEFAULT_FONT_SIZE) / 2,
			SCREEN_HEIGHT / 2 + (2 * SQUARE_SIZE), DEFAULT_FONT_SIZE,
			SNAKE_COLOR
		);

		DrawText(
			RESTART_GAME_TEXT,
			SCREEN_WIDTH / 2 - MeasureText(RESTART_GAME_TEXT, DEFAULT_FONT_SIZE) / 2,
			SCREEN_HEIGHT / 2 + (4 * SQUARE_SIZE), DEFAULT_FONT_SIZE,
			SNAKE_COLOR
		);
		
	}
	else
	{
		// draw walls
		DrawRectangleLines(
			WALL_OFFSET,
			WALL_OFFSET,
			SCREEN_WIDTH - (2 * WALL_OFFSET),
			SCREEN_HEIGHT - (2 * WALL_OFFSET),
			SNAKE_COLOR
		);

		// draw snake
		for (int i = 0; i < snakeLength; i++)
		{
			DrawRectangle(
				snake[i].position.x,
				snake[i].position.y,
				SQUARE_SIZE,
				SQUARE_SIZE,
				snake[i].color
			);
			DrawRectangleLines(
				snake[i].position.x,
				snake[i].position.y,
				SQUARE_SIZE,
				SQUARE_SIZE,
				BACKGROUND_COLOR
			);
		}

		// draw food
		DrawCircle(
			fruit.position.x + SQUARE_SIZE / 2,
			fruit.position.y + SQUARE_SIZE / 2,
			SQUARE_SIZE / 2,
			fruit.color
		);
	}

	EndDrawing();
}

void UpdateGame()
{
	HandleInput();
	UpdateSnake();
	HandleCollision();
	SpawnFood();
}

void UpdateSnake()
{
	// update previous position with the current position of the snake
	for (int i = 0; i < snakeLength; i++)
	{
		snakePreviousPosition[i] = snake[i].position;
	}

	// move after 5 frames
	if ((frameCounter % 5) == 0)
	{
		for (int i = 0; i < snakeLength; i++)
		{
			if (i == 0) // move snake head
			{
				snake[0].position.x += snake[0].speed.x;
				snake[0].position.y += snake[0].speed.y;
				canSnakeMove = true;
			}
			else // move snake body
			{
				snake[i].position = snakePreviousPosition[i - 1];
			}
		}
	}
}

void HandleInput()
{
	if (isGameOver)
	{
		if (IsKeyPressed(KEY_R))
		{
			InitGame();
		}
	}
	else
	{
		if (canSnakeMove)
		{
			if (IsKeyPressed(KEY_RIGHT) && snake[0].speed.x == 0)
			{
				snake[0].speed = { SQUARE_SIZE, 0 };
				canSnakeMove = false;
			}
			else if (IsKeyPressed(KEY_LEFT) && snake[0].speed.x == 0)
			{
				snake[0].speed = { -SQUARE_SIZE, 0 };
				canSnakeMove = false;
			}
			else if (IsKeyPressed(KEY_DOWN) && snake[0].speed.y == 0)
			{
				snake[0].speed = { 0, SQUARE_SIZE };
				canSnakeMove = false;
			}
			else if (IsKeyPressed(KEY_UP) && snake[0].speed.y == 0)
			{
				snake[0].speed = { 0, -SQUARE_SIZE };
				canSnakeMove = false;
			}
		}

	}
}

void SpawnFood()
{
	if (!shouldSpawnFood)
	{
		return;
	}

	while (shouldSpawnFood)
	{
		fruit.position = {
			float(GetRandomValue(WALL_OFFSET, ((SCREEN_WIDTH - SQUARE_SIZE - 2 * WALL_OFFSET) / SQUARE_SIZE)) * SQUARE_SIZE),
			float(GetRandomValue(WALL_OFFSET, ((SCREEN_HEIGHT - SQUARE_SIZE - 2 * WALL_OFFSET) / SQUARE_SIZE)) * SQUARE_SIZE)
		};

		shouldSpawnFood = false;

		for (int i = 0; i < snakeLength; i++)
		{
			if (fruit.position.x == snake[i].position.x && fruit.position.y == snake[i].position.y)
			{
				shouldSpawnFood = true;
				break;
			}
		}
	}
}

void HandleCollision()
{
	// collision with wall
	if (snake[0].position.x + SQUARE_SIZE > SCREEN_WIDTH - WALL_OFFSET || snake[0].position.x < WALL_OFFSET) // if hit side wall
	{
		isGameOver = true;
	}
	if (snake[0].position.y + SQUARE_SIZE > SCREEN_HEIGHT - WALL_OFFSET || snake[0].position.y < WALL_OFFSET) // if hit upper/lower wall
	{
		isGameOver = true;
	}

	// collision with self
	for (int i = 1; i < snakeLength; i++)
	{
		if (snake[0].position.x == snake[i].position.x && snake[0].position.y == snake[i].position.y)
		{
			isGameOver = true;
			break;
		}
	}

	// collision with food
	if (
		(snake[0].position.x < (fruit.position.x + SQUARE_SIZE) && // 
			(snake[0].position.x + SQUARE_SIZE) > fruit.position.x) &&
		(snake[0].position.y < (fruit.position.y + SQUARE_SIZE) &&
			(snake[0].position.y + SQUARE_SIZE) > fruit.position.y)
		)
	{
		snake[snakeLength].position = snakePreviousPosition[snakeLength - 1];
		snakeLength++;
		score++;
		shouldSpawnFood = true;
	}
}