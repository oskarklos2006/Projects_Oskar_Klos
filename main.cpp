#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH_LEFT 600
#define WINDOW_WIDTH_RIGHT 800


enum Direction {
    UP, DOWN, LEFT, RIGHT
};

typedef struct {
    int length;
    int dir;
    int bit_size;
    int small_bit_size;
    int game_speed;
}Constants;

void printText(SDL_Renderer* renderer, SDL_Texture* charset, int x, int y, const char* text, int fontSize) {
    SDL_Rect s, d;
    s.w = 8;
    s.h = 8;
    d.w = fontSize;
    d.h = fontSize;
    while (*text) {
        int c = *text & 255;
        s.x = (c % 16) * 8;
        s.y = (c / 16) * 8;
        d.x = x;
        d.y = y;
        SDL_RenderCopy(renderer, charset, &s, &d);
        x += fontSize;
        text++;
    }
}

void gameInit(SDL_Rect* snakeBody, Constants* config, SDL_Rect* blueApple, SDL_Rect* redApple, int* score) {
    config->length = 5;
    config->bit_size = 25;
    config->small_bit_size = 20;
    config->game_speed = 100;
    config->dir = UP;

    // STARTING PARAMETERS OF SNAKE
    for (int i = 0; i < config->length; i++) {
        snakeBody[i].x = config->bit_size * 4;
        snakeBody[i].y = WINDOW_HEIGHT - config->bit_size;
        snakeBody[i].w = config->bit_size;
        snakeBody[i].h = config->bit_size;
    }
}

void textDisplay(SDL_Renderer* renderer, SDL_Texture* charset, float time, int score) {

    // BASIC TEXT OPTIONS
    printText(renderer, charset, WINDOW_WIDTH_LEFT + WINDOW_WIDTH_RIGHT / 2 - (strlen("SNAKE GAME") * 40) / 2, WINDOW_HEIGHT / 2 - 40 * 2, "SNAKE GAME", 40);
    printText(renderer, charset, (WINDOW_WIDTH_LEFT + WINDOW_WIDTH_RIGHT / 2 - (strlen("A,B,C,D,G") * 24) / 2), WINDOW_HEIGHT / 2 + 24 * 6, "A,B,C,D,G", 24);
    printText(renderer, charset, WINDOW_WIDTH_LEFT + WINDOW_WIDTH_RIGHT / 2 - (strlen("Press N to play") * 18) / 2, WINDOW_HEIGHT / 2, "Press N to play", 18);
    printText(renderer, charset, WINDOW_WIDTH_LEFT + WINDOW_WIDTH_RIGHT / 2 - (strlen("Press Esc to exit") * 18) / 2, WINDOW_HEIGHT / 2 + 18 * 2, "Press Esc to exit", 18);

    // TEXT WHEN THE GAME IS FINISHED
    if (time != 0) {
        printText(renderer, charset, (WINDOW_WIDTH_LEFT / 2 - (strlen("GAME OVER") * 40) / 2), WINDOW_HEIGHT / 2 - 40 * 3, "GAME OVER", 40);

        char scoreText[20];
        snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
        printText(renderer, charset, (WINDOW_WIDTH_LEFT / 2 - (strlen(scoreText) * 18) / 2), WINDOW_HEIGHT / 2 - 18, scoreText, 18);

        char tmp[50];
        snprintf(tmp, sizeof(tmp), "Playing time: %.1fs ", time);
        printText(renderer, charset, (WINDOW_WIDTH_LEFT / 2 - (strlen(tmp) * 18) / 2), WINDOW_HEIGHT / 2 + 18 * 2, tmp, 18);

        printText(renderer, charset, (WINDOW_WIDTH_LEFT / 2 - (strlen("You can do better than that!") * 15) / 2), WINDOW_HEIGHT / 2 + 15 * 6, "You can do better than that!", 15);
    }
}

void openDisplay(SDL_Renderer* renderer, SDL_Texture* charset, Constants* config, int* game_over, float* time, int* score) {
    SDL_Rect startingScreen{ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect leftRect = { 0, 0, 500, 500 };
    SDL_RenderFillRect(renderer, &leftRect);

    SDL_SetRenderDrawColor(renderer, 200, 200, 155, 255);
    SDL_Rect rightRect = { 500, 0, 600, 500 };
    SDL_RenderFillRect(renderer, &rightRect);

    textDisplay(renderer, charset, *time, *score);

    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    *game_over = 0;
                    return;
                }
                if (event.key.keysym.sym == SDLK_n) {
                    *game_over = 1;
                    return;
                }
            }
        }
    }
}

void snakeBorder(SDL_Rect* snakeBody, Constants* config) {
    const int GAME_WIDTH = WINDOW_WIDTH_LEFT / config->bit_size;
    const int GAME_HEIGHT = WINDOW_HEIGHT / config->bit_size;

    // RIGHT SIDE BORDER
    if (snakeBody[0].x >= GAME_WIDTH * config->bit_size) {
        if (snakeBody[0].y == (GAME_HEIGHT - 1) * config->bit_size) {
            snakeBody[0].x = (GAME_WIDTH - 1) * config->bit_size;
            snakeBody[0].y -= config->bit_size;
            config->dir = UP;
        }
        else {
            snakeBody[0].x = (GAME_WIDTH - 1) * config->bit_size;
            snakeBody[0].y += config->bit_size;
            config->dir = DOWN;
        }
    }

    // LEFT SIDE BORDER
    if (snakeBody[0].x < 0) {
        if (snakeBody[0].y == 0) {
            snakeBody[0].x = 0;
            snakeBody[0].y += config->bit_size;
            config->dir = DOWN;
        }
        else {
            snakeBody[0].x = 0;
            snakeBody[0].y -= config->bit_size;
            config->dir = UP;
        }
    }

    // DOWN SIDE BORDER
    if (snakeBody[0].y >= GAME_HEIGHT * config->bit_size) {
        if (snakeBody[0].x == 0) {
            snakeBody[0].x += config->bit_size;
            snakeBody[0].y = (GAME_HEIGHT - 1) * config->bit_size;
            config->dir = RIGHT;
        }
        else {
            snakeBody[0].x -= config->bit_size;
            snakeBody[0].y = (GAME_HEIGHT - 1) * config->bit_size;
            config->dir = LEFT;
        }
    }

    // UP SIDE BORDER
    if (snakeBody[0].y < 0) {
        if (snakeBody[0].x == (GAME_WIDTH - 1) * config->bit_size) {
            snakeBody[0].x -= config->bit_size;
            snakeBody[0].y = 0;
            config->dir = LEFT;
        }
        else {
            snakeBody[0].x += config->bit_size;
            snakeBody[0].y = 0;
            config->dir = RIGHT;
        }
    }
}

void moveSnake(SDL_Rect* snakeBody, SDL_Renderer* renderer, SDL_Texture* charset, Constants* config, int* game_over) {
    for (int i = config->length - 1; i > 0; --i) {
        snakeBody[i].x = snakeBody[i - 1].x;
        snakeBody[i].y = snakeBody[i - 1].y;
    }

    switch (config->dir) {
    case UP:
        snakeBody[0].y -= config->bit_size;
        break;
    case DOWN:
        snakeBody[0].y += config->bit_size;
        break;
    case LEFT:
        snakeBody[0].x -= config->bit_size;
        break;
    case RIGHT:
        snakeBody[0].x += config->bit_size;
        break;
    }

    // SNAKE COLLISION 
    for (int i = 1; i < config->length; ++i) {
        if (snakeBody[0].x == snakeBody[i].x && snakeBody[0].y == snakeBody[i].y) {
            *game_over = 0;
            return;
        }
    }

    snakeBorder(snakeBody, config);
}

void textureSnakeBody(SDL_Rect* snakeBody, SDL_Renderer* renderer, Constants* config) {
    for (int i = 0; i < config->length; i++) {
        if (i % 2 == 0) {
            SDL_SetRenderDrawColor(renderer, 200, 0, 50, 255);
            SDL_RenderFillRect(renderer, &snakeBody[i]);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 150, 0, 150, 255);
            SDL_Rect smallCell = {
                snakeBody[i].x + (config->bit_size - config->small_bit_size) / 2,
                snakeBody[i].y + (config->bit_size - config->small_bit_size) / 2,
                config->small_bit_size, config->small_bit_size
            };
            SDL_RenderFillRect(renderer, &smallCell);
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 100, 50, 255);
    SDL_RenderFillRect(renderer, &snakeBody[0]);
    SDL_RenderFillRect(renderer, &snakeBody[config->length - 1]);
}

void generateBlueApple(SDL_Rect* blueApple, Constants* config) {
    const int GAME_WIDTH = WINDOW_WIDTH_LEFT / config->bit_size;
    const int GAME_HEIGHT = WINDOW_HEIGHT / config->bit_size;

    // BASIC PARAMETERS OF BLUE APPLE
    blueApple->x = (rand() % GAME_WIDTH) * config->bit_size;
    blueApple->y = (rand() % GAME_HEIGHT) * config->bit_size;
    blueApple->w = config->bit_size;
    blueApple->h = config->bit_size;
}

void gameBlueApple(SDL_Rect* snakeBody, SDL_Rect* blueApple, SDL_Renderer* renderer, Constants* config, int* score, int* isLarge) {

    // SNAKE EATS BLUE APPLE
    if (snakeBody[0].x == blueApple->x && snakeBody[0].y == blueApple->y) {
        config->length++;
        *score += 10;

        snakeBody[config->length - 1] = snakeBody[config->length - 2];

        generateBlueApple(blueApple, config);
    }

    // PULSATING EFFECT
    if (*isLarge) {
        SDL_SetRenderDrawColor(renderer, 100, 0, 255, 255);
        SDL_RenderFillRect(renderer, blueApple);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 100, 0, 255, 255);
        SDL_Rect smallApple = {
            blueApple->x + (config->bit_size - config->small_bit_size) / 2,
            blueApple->y + (config->bit_size - config->small_bit_size) / 2,
            config->small_bit_size,
            config->small_bit_size
        };
        SDL_RenderFillRect(renderer, &smallApple);
    }
}

void generateRedApple(SDL_Renderer* renderer, SDL_Texture* charset, SDL_Rect* redApple, Constants* config, int* score, float* time) {
    const int GAME_WIDTH = WINDOW_WIDTH_LEFT / config->bit_size;
    const int GAME_HEIGHT = WINDOW_HEIGHT / config->bit_size;

    // BASIC PARAMETERS OF RED DOT
    redApple->x = (rand() % GAME_WIDTH) * config->bit_size;
    redApple->y = (rand() % GAME_HEIGHT) * config->bit_size;
    redApple->w = config->bit_size;
    redApple->h = config->bit_size;
}

void gameRedApple(SDL_Rect* snakeBody, SDL_Rect* redApple, SDL_Renderer* renderer, SDL_Texture* charset, Constants* config, int* score, int* isLarge, float* time, int* activeRedApple) {

    // TIMER OF RED APPLE
    if (fmod(*time, 15.0f) < 0.1f && activeRedApple && *activeRedApple == 0 && *time > 0) {
        generateRedApple(renderer, charset, redApple, config, score, time);
        *activeRedApple = 1;
    }

    if (fmod(*time, 15.0f) > 5.0f && activeRedApple && *activeRedApple == 1) {
        *activeRedApple = 0;
    }

    // SNAKE EATS RED APPLE
    if (activeRedApple && *activeRedApple == 1 && snakeBody[0].x == redApple->x && snakeBody[0].y == redApple->y) {
        config->length -= 3;
        if (config->length < 1) config->length = 1;
        *score += 40;
        *activeRedApple = 0;
    }

    // PULSATING EFFECT
    if (activeRedApple && *activeRedApple == 1) {
        if (*isLarge) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, redApple);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect smallApple = {
                redApple->x + (config->bit_size - config->small_bit_size) / 2,
                redApple->y + (config->bit_size - config->small_bit_size) / 2,
                config->small_bit_size,
                config->small_bit_size
            };
            SDL_RenderFillRect(renderer, &smallApple);
        }
    }
}

void textDuringGame(SDL_Renderer* renderer, SDL_Texture* charset, Constants* config, float time, int score, int* activeRedApple) {
    char scoreText[20];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    printText(renderer, charset, WINDOW_WIDTH_LEFT + (WINDOW_WIDTH_RIGHT / 2 - (strlen(scoreText) * 24) / 2), config->bit_size * 5, scoreText, 24);

    char tmp[50];
    snprintf(tmp, sizeof(tmp), "Playing time: %.1fs ", time);
    printText(renderer, charset, WINDOW_WIDTH_LEFT + (WINDOW_WIDTH_RIGHT / 2 - (strlen(tmp) * 24) / 2), config->bit_size * 7, tmp, 24);

    if (fmod(time, 15.0f) > 0.1f && fmod(time, 15.0f) < 5.0f && time > 6 && *activeRedApple == 1) {
        int passedtime = fmod(time, 5.0f);
        printText(renderer, charset, WINDOW_WIDTH_LEFT + WINDOW_WIDTH_RIGHT / 2 - (strlen("Red apple disappears in:") * 24) / 2, WINDOW_HEIGHT / 2, "Red apple disappears in:", 24);
        if (passedtime == 0) {
            printText(renderer, charset, WINDOW_WIDTH_LEFT + WINDOW_WIDTH_RIGHT / 2 - (strlen("O O O O O") * 24) / 2, WINDOW_HEIGHT / 2 + 24 * 2, "O O O O O", 24);
        }
        if (passedtime == 1) {
            printText(renderer, charset, WINDOW_WIDTH_LEFT + WINDOW_WIDTH_RIGHT / 2 - (strlen("O O O O  ") * 24) / 2, WINDOW_HEIGHT / 2 + 24 * 2, "O O O O  ", 24);
        }
        if (passedtime == 2) {
            printText(renderer, charset, WINDOW_WIDTH_LEFT + WINDOW_WIDTH_RIGHT / 2 - (strlen("O O O    ") * 24) / 2, WINDOW_HEIGHT / 2 + 24 * 2, "O O O    ", 24);
        }
        if (passedtime == 3) {
            printText(renderer, charset, WINDOW_WIDTH_LEFT + WINDOW_WIDTH_RIGHT / 2 - (strlen("O O      ") * 24) / 2, WINDOW_HEIGHT / 2 + 24 * 2, "O O      ", 24);
        }
        if (passedtime == 4) {
            printText(renderer, charset, WINDOW_WIDTH_LEFT + WINDOW_WIDTH_RIGHT / 2 - (strlen("O        ") * 24) / 2, WINDOW_HEIGHT / 2 + 24 * 2, "O        ", 24);
        }
    }
}

void snakeGame(SDL_Rect* snakeBody, SDL_Renderer* renderer, SDL_Texture* charset, Constants* config, int* score, float* time, int* isLarge) {
    const int GAME_WIDTH = WINDOW_WIDTH_LEFT / config->bit_size;
    const int GAME_HEIGHT = WINDOW_HEIGHT / config->bit_size;
    int game_over = 1;
    int activeRedApple = 0;
    int speedchange = 0;


    SDL_Event event;
    SDL_Rect blueApple;
    SDL_Rect redApple;
    generateBlueApple(&blueApple, config);

    while (game_over == 1) {
        SDL_RenderClear(renderer);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    exit(0);
                case SDLK_UP:
                    if (config->dir != DOWN) config->dir = UP;
                    break;
                case SDLK_DOWN:
                    if (config->dir != UP) config->dir = DOWN;
                    break;
                case SDLK_LEFT:
                    if (config->dir != RIGHT) config->dir = LEFT;
                    break;
                case SDLK_RIGHT:
                    if (config->dir != LEFT) config->dir = RIGHT;
                    break;
                }
                break;
            }
        }

        moveSnake(snakeBody, renderer, charset, config, &game_over);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect leftRect = { 0, 0, WINDOW_WIDTH_LEFT, WINDOW_HEIGHT };
        SDL_RenderFillRect(renderer, &leftRect);

        gameBlueApple(snakeBody, &blueApple, renderer, config, score, isLarge);
        gameRedApple(snakeBody, &redApple, renderer, charset, config, score, isLarge, time, &activeRedApple);

        if (*isLarge == 0) {
            *isLarge = 1;
        }
        else *isLarge = 0;

        textureSnakeBody(snakeBody, renderer, config);

        SDL_SetRenderDrawColor(renderer, 200, 200, 155, 255);
        SDL_Rect rightRect = { 500, 0, WINDOW_WIDTH_RIGHT, WINDOW_HEIGHT };
        SDL_RenderFillRect(renderer, &rightRect);

        textDuringGame(renderer, charset, config, *time, *score, &activeRedApple);

        SDL_RenderPresent(renderer);

        // GAME SPEED 
        *time += ((float)config->game_speed / 1000);
        if (fmod(*time, 20.0f) < 0.1f && speedchange == 0 && *time > 0) {
            if (config->game_speed >= 50) {
                config->game_speed -= 10;
            }
            speedchange = 1;
        }

        if (fmod(*time, 20.0f) >= 0.1f) {
            speedchange = 0;
        }

        SDL_Delay(config->game_speed);
    }
}

void resetGame(SDL_Renderer* renderer, SDL_Texture* charset, SDL_Rect* snakeBody, SDL_Rect* blueApple, SDL_Rect* redApple, Constants* config, int* score, float* time, int* isLarge, int* game_over) {
    gameInit(snakeBody, config, blueApple, redApple, score);
    openDisplay(renderer, charset, config, game_over, time, score);
    *score = 0;
    *time = 0;
}

int main(int argc, char* argv[]) {
    srand(time(0));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Oskar Klos 203708", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Surface* charsetSurface = SDL_LoadBMP("cs8x8.bmp");
    SDL_Texture* charset = SDL_CreateTextureFromSurface(renderer, charsetSurface);
    SDL_FreeSurface(charsetSurface);
    SDL_ShowCursor(SDL_DISABLE);

    SDL_Rect snakeBody[1000]{ 0 };
    SDL_Rect blueApple{ 0 };
    SDL_Rect redApple{ 0 };

    Constants config;

    int game_over = 1;
    int score = 0;
    float time = 0;
    int isLarge = 0;

    while (true) {
        resetGame(renderer, charset, snakeBody, &blueApple, &redApple, &config, &score, &time, &isLarge, &game_over);
        if (game_over == 0) {
            break;
        }
        if (game_over == 1) {
            snakeGame(snakeBody, renderer, charset, &config, &score, &time, &isLarge);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}