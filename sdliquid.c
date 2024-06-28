#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <math.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
int CHAR_SIZE = 32;

float GRAVITY = 0.9f;
int RADIUS = 15;
float FRICTION = 0.9f;

int mouseX = 0;
int mouseY = 0;
int MOUSE_CLICKED = 1;

float averageSpeed = 1;

int entity_count = 0;

Uint32 start_time;
int frame_count = 0;
float fps = 0.0f;

struct entity {
	float posX;
	float posY;
	float velX;
	float velY;
	int radius;
	float gravity;
    float friction;
};

struct entity entity_list[16536];

void addBall() {
	struct entity ett;
	ett.posX = SCREEN_WIDTH / 2.0f;
	ett.posY = 100.0f;
	ett.velX = 10.0f;
	ett.velY = 0.0f;
	ett.radius = RADIUS;
	ett.gravity = GRAVITY;
    ett.friction = FRICTION;
	entity_list[entity_count] = ett;
	entity_count++;
}

float dotProduct(float x1, float y1, float x2, float y2) {
    return x1 * x2 + y1 * y2;
}

int checkCollision(struct entity ett1, struct entity ett2) {
    int dx = ett1.posX - ett2.posX;
    int dy = ett1.posY - ett2.posY;
    int distance = sqrt(dx * dx + dy * dy);

    return distance < (ett1.radius + ett2.radius);
}


float calculateSpeed(struct entity* ett) {
    return sqrt(ett->velX * ett->velX + ett->velY * ett->velY);
}

void speedToColor(float speed, Uint8* r, Uint8* g, Uint8* b) {
    float normalizedSpeed = fmin(speed / averageSpeed, 1.0);

    *r = (Uint8)(normalizedSpeed * 255);
    *g = 0;
    *b = (Uint8)((1.0 - normalizedSpeed) * 255);
}

void updateFPS() {
    frame_count++;
    Uint32 current_time = SDL_GetTicks();
    if (current_time - start_time >= 1000) {
        fps = frame_count * 1000.0f / (current_time - start_time);
        frame_count = 0;
        start_time = current_time;
    }
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


float calculateAverageSpeed() {
    float totalSpeed = 0.0f;
    for (int i = 0; i < entity_count; i++) {
        totalSpeed += calculateSpeed(&entity_list[i]);
    }
    return totalSpeed / entity_count;
}

int checkParticleDistance(struct entity ett1, struct entity ett2) {
    float dx = ett1.posX - ett2.posX;
    float dy = ett1.posY - ett2.posY;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < (ett1.radius + ett2.radius + 5);
}


// Function to check and handle collision with screen boundaries
void checkCollisionWithScreenBoundaries(struct entity* ett) {
    float* positions[2] = { &ett->posX, &ett->posY };
    float* velocities[2] = { &ett->velX, &ett->velY };
    float bounds[2] = { SCREEN_WIDTH, SCREEN_HEIGHT };

    for (int i = 0; i < 2; i++) {
        if (*positions[i] - ett->radius < 0) {
            *positions[i] = ett->radius;
            *velocities[i] *= -1 * FRICTION;
        }
        if (*positions[i] + ett->radius > bounds[i]) {
            *positions[i] = bounds[i] - ett->radius;
            *velocities[i] *= -1 * FRICTION;
        }
    }
}

void handleCollision(struct entity* ett1, struct entity* ett2) {
    float dx = ett1->posX - ett2->posX;
    float dy = ett1->posY - ett2->posY;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance == 0) {
        distance = ett1->radius + 0.5;

        ett1->posX += 10;
        ett1->posY += 10;

        ett2->posX -= 10;
        ett2->posY -= 10;
    }

    float nx = dx / distance;
    float ny = dy / distance;

    float dvx = ett1->velX - ett2->velX;
    float dvy = ett1->velY - ett2->velY;

    float dotProductResult = dotProduct(dvx, dvy, nx, ny);

    if (dotProductResult > 0) return;

    float restitution = FRICTION;

    float impulse = (2.0 * dotProductResult) / (1.0 + 1.0) * restitution;

    ett1->velX -= impulse * nx;
    ett1->velY -= impulse * ny;

    ett2->velX += impulse * nx;
    ett2->velY += impulse * ny;

    float percent = 0.5;
    float correction = (ett1->radius + ett2->radius - distance) / 2.0;
    float correctionX = nx * correction * percent;
    float correctionY = ny * correction * percent;

    ett1->posX += correctionX;
    ett1->posY += correctionY;

    ett2->posX -= correctionX;
    ett2->posY -= correctionY;
}

void drawBall(SDL_Renderer* renderer, int centerX, int centerY, int radius, Uint8 r, Uint8 g, Uint8 b) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance <= radius) {
                // Exponential fade, the farther from the center, the more transparent
                float alpha = 255 * exp(-2 * (distance / radius));

                SDL_SetRenderDrawColor(renderer, r, g, b, (Uint8)alpha);
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

int main(int argc, char* args[]) {
    if (argc != 3) {
	    SCREEN_WIDTH = 640;
	    SCREEN_HEIGHT = 480;
    } else {
		SCREEN_WIDTH = atoi(args[1]);
		SCREEN_HEIGHT = atoi(args[2]);
	}

	addBall();

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	TTF_Init();

	TTF_Font* font = TTF_OpenFont("font.ttf", CHAR_SIZE);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("SDLIQUID", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    int quit = 0;

    SDL_Event e;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_q:
                        quit = 1;
                        break;
                    case SDLK_a:
                        addBall();
                        break;
                    case SDLK_u:
                        GRAVITY += 0.1f;
                        break;
                    case SDLK_j:
                        GRAVITY -= 0.1f;
                        break;
                    case SDLK_i:
                        RADIUS += 1;
                        break;
                    case SDLK_k:
                        RADIUS -= 1;
                        break;
                    case SDLK_o:
                        FRICTION += 0.1f;
                        break;
                    case SDLK_l:
                        FRICTION -= 0.1f;
                        break;
                }
            }
            else if (e.button.button == SDL_BUTTON_LEFT) {
                mouseX = e.button.x;
                mouseY = e.button.y;

                MOUSE_CLICKED = 1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 44, 62, 80, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < entity_count; i++) {
            struct entity ett = entity_list[i];

            if (MOUSE_CLICKED) {
                float dx = mouseX - ett.posX;
                float dy = mouseY - ett.posY;
                float distance = sqrt(dx * dx + dy * dy);
                if (distance == 0) distance = 0.1f;
                if (distance < 10 * RADIUS) {
                    float force = 2.0f / distance;
                    ett.velX += force * dx;
                    ett.velY += force * dy;
                }
            }

            ett.gravity = GRAVITY;
            ett.radius = RADIUS;

            ett.velY += ett.gravity;

            ett.posX += ett.velX;
            ett.posY += ett.velY;

            // Check for collision with walls
            checkCollisionWithScreenBoundaries(&ett);

            // Check for collision with other entities
            for (int y = 0; y < entity_count; y++) {
                if (i != y) { // Skip self-collision check
                    struct entity ett2 = entity_list[y];
                    if (checkParticleDistance(ett, ett2)) {
                        if (checkCollision(ett, ett2)) {
                            handleCollision(&ett, &ett2);
                        }
                    }
                }
            }

            entity_list[i] = ett;

            float speed = calculateSpeed(&ett);
            Uint8 r, g, b;
            speedToColor(speed, &r, &g, &b);

            drawBall(renderer, (int)ett.posX, (int)ett.posY, ett.radius, r, g, b);
        }

        updateFPS();
        averageSpeed = calculateAverageSpeed();
        char debugText[128];
        sprintf(debugText, "FPS: %.2f   ENTITIES: %d   AVERAGE SPEED: %.2f", fps, entity_count, averageSpeed);
        renderText(renderer, font, debugText, 10, 10);

        sprintf(debugText, "GRAVITY:%.1f   RADIUS:%d   FRICTION:%.1f", GRAVITY, RADIUS, FRICTION);
        renderText(renderer, font, debugText, 10, CHAR_SIZE + 10);
        SDL_RenderPresent(renderer);

        MOUSE_CLICKED = 0;

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    SDL_Quit();

    return 0;
}
