#include "raylib.h"
#include <vector>
#include <deque>

// Grid configuration
const int GRID_SIZE = 20;
const int CELL_SIZE = 30;
const int OFFSET = 60; // Top bar space for score

struct SnakeNode {
    Vector2 position;
};

class Snake {
public:
    std::deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
    Vector2 direction = { 1, 0 };
    bool addSegment = false;

    void Draw() {
        for (size_t i = 0; i < body.size(); i++) {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = {
                OFFSET + x * CELL_SIZE,
                OFFSET + y * CELL_SIZE,
                (float)CELL_SIZE,
                (float)CELL_SIZE
            };
            Color color = (i == 0) ? DARKGREEN : GREEN;
            DrawRectangleRounded(segment, 0.5f, 6, color);
        }
    }

    void Update() {
        body.push_front({ body[0].x + direction.x, body[0].y + direction.y });
        if (addSegment) {
            addSegment = false;
        } else {
            body.pop_back();
        }
    }

    void Reset() {
        body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
        direction = { 1, 0 };
    }
};

class Food {
public:
    Vector2 position;

    Food(const std::deque<Vector2>& snakeBody) {
        position = GenerateRandomPos(snakeBody);
    }

    void Draw() {
        Rectangle foodRect = {
            OFFSET + position.x * CELL_SIZE,
            OFFSET + position.y * CELL_SIZE,
            (float)CELL_SIZE,
            (float)CELL_SIZE
        };
        DrawRectangleRounded(foodRect, 0.8f, 6, RED);
    }

    Vector2 GenerateRandomPos(const std::deque<Vector2>& snakeBody) {
        float x = GetRandomValue(0, GRID_SIZE - 1);
        float y = GetRandomValue(0, GRID_SIZE - 1);

        // Ensure food doesn't spawn on the snake body
        for (auto& segment : snakeBody) {
            if (segment.x == x && segment.y == y) {
                return GenerateRandomPos(snakeBody);
            }
        }
        return Vector2{ x, y };
    }
};

bool ElementInDeque(Vector2 element, const std::deque<Vector2>& deque) {
    for (size_t i = 0; i < deque.size(); i++) {
        if (deque[i].x == element.x && deque[i].y == element.y) {
            return true;
        }
    }
    return false;
}

bool EventTriggered(double interval, double& lastUpdateTime) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

int main() {
    int windowWidth = 2 * OFFSET + CELL_SIZE * GRID_SIZE;
    int windowHeight = 2 * OFFSET + CELL_SIZE * GRID_SIZE;

    InitWindow(windowWidth, windowHeight, "Perfect Snake - Raylib");
    SetTargetFPS(60);

    Snake snake = Snake();
    Food food = Food(snake.body);

    int score = 0;
    bool gameOver = false;
    bool paused = false;

    double lastUpdateTime = 0;

    while (!WindowShouldClose()) {
        // --- INPUT & UPDATE ---
        if (IsKeyPressed(KEY_UP) && snake.direction.y != 1) {
            snake.direction = { 0, -1 };
        }
        if (IsKeyPressed(KEY_DOWN) && snake.direction.y != -1) {
            snake.direction = { 0, 1 };
        }
        if (IsKeyPressed(KEY_LEFT) && snake.direction.x != 1) {
            snake.direction = { -1, 0 };
        }
        if (IsKeyPressed(KEY_RIGHT) && snake.direction.x != -1) {
            snake.direction = { 1, 0 };
        }
        if (IsKeyPressed(KEY_P)) {
            paused = !paused;
        }
        if (gameOver && IsKeyPressed(KEY_ENTER)) {
            snake.Reset();
            food.position = food.GenerateRandomPos(snake.body);
            score = 0;
            gameOver = false;
        }

        // Game Update Tick (Moves every 0.15s)
        if (!gameOver && !paused && EventTriggered(0.15, lastUpdateTime)) {
            snake.Update();

            // Collision with food
            if (snake.body[0].x == food.position.x && snake.body[0].y == food.position.y) {
                food.position = food.GenerateRandomPos(snake.body);
                snake.addSegment = true;
                score += 10;
            }

            // Collision with wall
            if (snake.body[0].x < 0 || snake.body[0].x >= GRID_SIZE ||
                snake.body[0].y < 0 || snake.body[0].y >= GRID_SIZE) {
                gameOver = true;
            }

            // Collision with tail
            std::deque<Vector2> tail(snake.body.begin() + 1, snake.body.end());
            if (ElementInDeque(snake.body[0], tail)) {
                gameOver = true;
            }
        }

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw outer frame border
        DrawRectangleLinesEx(Rectangle{
            (float)OFFSET - 5,
            (float)OFFSET - 5,
            (float)CELL_SIZE * GRID_SIZE + 10,
            (float)CELL_SIZE * GRID_SIZE + 10
        }, 5, DARKGRAY);

        // Draw Top Stats Header
        DrawText("SNAKE GAME", OFFSET, 20, 30, DARKGRAY);
        DrawText(TextFormat("Score: %i", score), windowWidth - OFFSET - 120, 20, 30, DARKGRAY);

        if (gameOver) {
            DrawText("GAME OVER", windowWidth / 2 - 100, windowHeight / 2 - 20, 30, RED);
            DrawText("Press ENTER to Restart", windowWidth / 2 - 130, windowHeight / 2 + 20, 20, DARKGRAY);
        } else if (paused) {
            DrawText("PAUSED", windowWidth / 2 - 60, windowHeight / 2, 30, GRAY);
        } else {
            food.Draw();
            snake.Draw();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
