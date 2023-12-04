#include "raylib.h"
#include "iostream"

struct Snake
{
	Vector2 position;
	Vector2 size;
	Vector2 speed;
	Color color;
};

struct Food
{
	Vector2 position;
	Vector2 size;
	Color color;
};

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int SQUARE_SIZE = 32;
const int MAX_SNAKE_LENGTH = 100;

bool isGameOver;
bool shouldSpawnFood;

Food fruit = { 0 };
Snake snake[MAX_SNAKE_LENGTH] = { 0 };
Vector2 snakePreviousPosition[MAX_SNAKE_LENGTH] = { 0 };
int snakeLength = 0;

int frameCounter = 0;

void InitGame();
void Tick();
void DrawGame();
void DrawGameOver();
void UpdateGame();
void UpdateSnake();
void HandleInput();
void HandleCollision();
void SpawnFood();

int main(void)
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");
	SetTargetFPS(60);

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

	snakeLength = 1;

	for (int i = 0; i < MAX_SNAKE_LENGTH; i++)
	{
		snake[i].position = { SQUARE_SIZE , SQUARE_SIZE };
		snake[i].size = { SQUARE_SIZE, SQUARE_SIZE };
		snake[i].speed = { SQUARE_SIZE, 0 };

		if (i == 0) snake[i].color = DARKBLUE;
		else snake[i].color = BLUE;
	}

	for (int i = 0; i < MAX_SNAKE_LENGTH; i++)
	{
		snakePreviousPosition[i] = { 0.0f, 0.0f };
	}

	fruit.size = { SQUARE_SIZE, SQUARE_SIZE };
	fruit.color = RED;

	shouldSpawnFood = true;
}

void Tick()
{
	if (!isGameOver) {
		UpdateGame();
		DrawGame();
		frameCounter++;
	}
	else
	{
		DrawGameOver();
	}
}

void DrawGame()
{
	BeginDrawing();

	ClearBackground(RAYWHITE);

	// draw grid - vertical
	for (int i = 0; i < SCREEN_WIDTH / SQUARE_SIZE; i++)
	{
		DrawLineV({ float(SQUARE_SIZE * i), 0 }, { float(SQUARE_SIZE * i), SCREEN_HEIGHT }, LIGHTGRAY);
	}

	// draw grid - horizontal
	for (int i = 0; i < SCREEN_HEIGHT / SQUARE_SIZE; i++)
	{
		DrawLineV({ 0, float(SQUARE_SIZE * i) }, { SCREEN_WIDTH, float(SQUARE_SIZE * i) }, LIGHTGRAY);
	}

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
	}

	// draw food
	DrawRectangle(
		fruit.position.x,
		fruit.position.y,
		SQUARE_SIZE,
		SQUARE_SIZE,
		fruit.color
	);

	EndDrawing();
}

void DrawGameOver()
{
	BeginDrawing();

	ClearBackground(BLACK);

	DrawText("Game Over", SCREEN_WIDTH / 2 - SQUARE_SIZE, SCREEN_HEIGHT / 2, 20, RED);

	EndDrawing();
}

void UpdateGame()
{
	if (isGameOver)
	{
		// handle game over
		return;
	}

	HandleInput();
	UpdateSnake();
	SpawnFood();
	HandleCollision();
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
	if (IsKeyPressed(KEY_RIGHT))
	{
		snake[0].speed = { SQUARE_SIZE, 0 };
	}
	if (IsKeyPressed(KEY_LEFT))
	{
		snake[0].speed = { -SQUARE_SIZE, 0 };
	}
	if (IsKeyPressed(KEY_DOWN))
	{
		snake[0].speed = { 0, SQUARE_SIZE };
	}
	if (IsKeyPressed(KEY_UP))
	{
		snake[0].speed = { 0, -SQUARE_SIZE };
	}
}

void SpawnFood()
{
	if (!shouldSpawnFood)
	{
		return;
	}

	fruit.position = {
		float(GetRandomValue(0, ((SCREEN_WIDTH-SQUARE_SIZE) / SQUARE_SIZE)) * SQUARE_SIZE),
		float(GetRandomValue(0, ((SCREEN_HEIGHT-SQUARE_SIZE) / SQUARE_SIZE)) * SQUARE_SIZE)
	};

	std::cout << "Fruit position : {" << fruit.position.x << ", " << fruit.position.y << "}\n";

	shouldSpawnFood = false;
}

void HandleCollision()
{
	// collision with wall
	if (snake[0].position.x + SQUARE_SIZE > SCREEN_WIDTH || snake[0].position.x < 0) // if hit side wall
	{
		isGameOver = true;
	}
	if (snake[0].position.y + SQUARE_SIZE > SCREEN_HEIGHT || snake[0].position.y < 0) // if hit upper/lower wall
	{
		isGameOver = true;
	}
	// collision with self

	// collision with food
	if (
		(snake[0].position.x < (fruit.position.x + fruit.size.x) && // 
		(snake[0].position.x + snake[0].size.x) > fruit.position.x) &&
		(snake[0].position.y < (fruit.position.y + fruit.size.y) && 
		(snake[0].position.y + snake[0].size.y) > fruit.position.y)
		)
	{
		snakeLength++;
		shouldSpawnFood = true;
	}
}