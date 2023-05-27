// gcc main.c -g -o ../bin/prog -lSDL2 -lSDL2_image -lSDL2_ttf && ./../bin/prog

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

const float REDUCING_FACTOR = 0.02;		// Multiplying this with all physics calculations

SDL_Color TEXTCOLOR = {190, 190, 190};
SDL_Color BG = {46, 50, 61};

void DrawTextBox(const char* text, int x, int y, TTF_Font* font, SDL_Surface* text_surface, 
		SDL_Texture* text_texture, SDL_Renderer* renderer) {

	text_surface = TTF_RenderText_Shaded(font, text, TEXTCOLOR, BG);	// Create Surface
	text_texture = SDL_CreateTextureFromSurface(renderer, text_surface); // Create texture from Surface

	SDL_Rect text_target;
	text_target.x = x;
	text_target.y = y;
	text_target.w = text_surface->w;
	text_target.h = text_surface->h;

	SDL_FreeSurface(text_surface);

	SDL_RenderCopy(renderer, text_texture, NULL, &text_target);
}

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

// CIRCLE
void PointMassRBPhysics(RectangleRB* rrb) {
	// TODO: resolve into x and y components and then and to x and y displacements
	rrb->velocity += ((rrb->acceleration * 16) * REDUCING_FACTOR);
	rrb->circle_target.y += ((rrb->velocity * 16) * REDUCING_FACTOR);
}

void InitialSetup() {
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	if (TTF_Init() == -1) {
		printf("Failed to intialize SDL_ttf: %s\n", SDL_GetError());
	}
}

int main() {
	InitialSetup();

	const int screen_width = 1280, screen_height = 720;
	SDL_Window* window = SDL_CreateWindow("another try", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// IMAGE
	SDL_Texture* circle_image = IMG_LoadTexture(renderer, "../images/circle.png");

	// TEXT
	TTF_Font* font;
	font = TTF_OpenFont("../res/fonts/SourceCodePro-Regular.ttf", 17);
	SDL_Surface* text_surface;
	SDL_Texture* text_texture;

	char input_text[101];
	memset(input_text, 0, strlen(input_text));

	// keep track of all rectangle rigid bodies that are being created
	RectangleRB rrb[11];
	int rrb_counter = 0;

	bool running = true, input_mode = false, add_mode = true; 
	SDL_StartTextInput();
	while(running) {
		Uint32 start = SDL_GetTicks();

		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}

			// Text when '/' is pressed
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_SLASH) {
					input_mode = true;
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_RETURN && input_mode) {
					input_mode = false;
				}
			}
			if (strlen(input_text) < 100) {
				if (event.type == SDL_TEXTINPUT && input_mode) {
					strcat(input_text, event.text.text);
				}
			}

			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				if (rrb_counter < 10) {
					// TODO: prompt to enter width, height, mass, acceleration, force
					int x, y;
					SDL_GetMouseState(&x, &y);
					//AddRectRB(w, h, x, y, mass, velocity, acceleration, force)
					AddRectRB(30, 30, x-15, y-15, 1, -40, 9.81, 1,&rrb[rrb_counter]);	// testing
					rrb_counter+=1;
				}
				else {
					printf("can't create more objects\n");
				}
			}
		} // end user event loop

		// DRAWING
		SDL_SetRenderDrawColor(renderer, 46, 50, 61, 255);
		SDL_RenderClear(renderer);

		for (int i = 0; i <= rrb_counter; i++) {
			PointMassRBPhysics(&rrb[i]);
			SDL_RenderCopy(renderer, circle_image, NULL, &rrb[i].circle_target);
		}
		//DrawTextBox(text, text_target, font, text_surface, text_texture, renderer)
		if (input_mode) {
			DrawTextBox(input_text, 0, 700, font, text_surface, text_texture, renderer);
		}

		SDL_RenderPresent(renderer);
		// END DRAWING

		Uint32 elapsed_time = SDL_GetTicks() - start;
		if (elapsed_time < 16) {
			SDL_Delay(16 - elapsed_time);
		}

	} // end main loop

	SDL_StopTextInput();

	TTF_CloseFont(font);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
