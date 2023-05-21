// gcc main.c -g -o ../bin/prog -lSDL2 -lSDL2_image -lSDL2_ttf && ./../bin/prog

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdbool.h>

// THIS IS A CIRCLE AND NOT A RECTANGLE
typedef struct {
	SDL_Rect circle_target;
	float mass, velocity, acceleration, force;
} RectangleRB;

void AddRectRB(int w, int h, int x, int y, float mass, float velocity, float acceleration,
		float force, RectangleRB* rectRB) {
	// take in 8 parameters
	rectRB->circle_target.w = w;
	rectRB->circle_target.h = h;
	rectRB->circle_target.x = x;
	rectRB->circle_target.y = y;

	rectRB->mass = mass;
	rectRB->velocity = velocity;
	rectRB->acceleration= acceleration;
	rectRB->force = force;
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	const int screen_width = 1280, screen_height = 720;
	SDL_Window* window = SDL_CreateWindow("another try", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// IMAGE
	SDL_Texture* circle_image = IMG_LoadTexture(renderer, "../images/circle.png");
	SDL_Rect circle_src;
	circle_src.x = 0;
	circle_src.y = 0;
	circle_src.w = 1000;
	circle_src.h = 1000;

	// keep track of all rectangle rigid bodies that are being created
	RectangleRB rrb[10];
	int rrb_counter = 0;

	bool running = true; 
	while(running) {
		Uint32 start = SDL_GetTicks();

		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}

			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				if (rrb_counter < 10) {
					// TODO: prompt to enter width, height, mass, acceleration, force
					int x, y;
					SDL_GetMouseState(&x, &y);
					AddRectRB(30, 30, x-15, y-15, 1, 1, 1, 1, &rrb[rrb_counter]);	// testing
					rrb_counter+=1;
				}
				else {
					printf("can't create more objects\n");
				}
			}
			/* Deleting objects is causing unexpected behavior, will deal will it later
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
				if (rrb_counter > 0) {
					rrb_counter-=1;
				}
			}
			*/

		} // end event loop

		// DRAWING
		SDL_SetRenderDrawColor(renderer, 28, 28, 28, 255);
		SDL_RenderClear(renderer);
		// TODO:  Draw Rectange rigid bodies
		for (int i = 0; i <= rrb_counter; i++) {
			SDL_RenderCopy(renderer, circle_image, &circle_src, &rrb[i].circle_target);
		}

		SDL_RenderPresent(renderer);
		// END DRAWING

		Uint32 elapsed_time = SDL_GetTicks() - start;
		if (elapsed_time < 16) {
			SDL_Delay(16 - elapsed_time);
		}
	} // end main loop

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
