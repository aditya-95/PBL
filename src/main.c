// gcc main.c -Wall -g -o ../bin/prog -lm -lSDL2 -lSDL2_image -lSDL2_ttf && ./../bin/prog
// for windows: gcc main.c -Wall -g -o ../bin/prog -lm -lSDL2 -lSDL2_image -lSDL_ttf && ./../bin/prog

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

const int TIME_FACTOR = 8;				// (1000ms/FPS)
const float REDUCING_FACTOR = 0.02;		// Multiplying this with all physics calculations to fit values of screen
const float PI = 3.141592;

typedef struct {
	float x, y;
} vec2;

// THIS IS A CIRCLE AND NOT A RECTANGLE
typedef struct {
	SDL_Rect circle_target;

	float v1, v2, v3, a1, a2, a3, dv1, dv2, dv3, da1, da2, da3;	
	vec2 vel1, vel2, vel3, acc1, acc2, acc3;		// velocity and acceleration
	vec2 rvel, racc;								// resultatn
	double rvel_dir, racc_dir;

} RectangleRB;

vec2 vec2Resolve(float magnitude, float direction) {
	vec2 target_vector;
	target_vector.x = magnitude * cos(direction * (PI/180));
	target_vector.y = -1 * (magnitude * sin(direction * (PI/180)));
	return target_vector;
}

vec2 calculateResultant(vec2 a, vec2 b, vec2 c) {
	vec2 resultant;
	resultant.x = a.x + b.x + c.x;
	resultant.y = a.y + b.y + c.y;

	return resultant;
}

// Read config file for velocity/acceleration magnitudes and angles and assign it to the added
// objects
void readConfig(const char* path, RectangleRB *rrb, int rrb_counter) {
	FILE *config = fopen(path, "r");
	if (config == NULL) {
		printf("could not open config");
	}

	for (int i = 0; i < rrb_counter; i++) {
		float temp[12];
		for (int i = 0; i < 12; i++) {
			fscanf(config, "%f", &temp[i]);
		}

		rrb[i].v1 = temp[0];
		rrb[i].v2 = temp[1];
		rrb[i].v3 = temp[2];
		rrb[i].a1 = temp[3];
		rrb[i].a2 = temp[4];
		rrb[i].a3 = temp[5];
		rrb[i].dv1 = temp[6];
		rrb[i].dv2 = temp[7];
		rrb[i].dv3 = temp[8];
		rrb[i].da1 = temp[9];
		rrb[i].da2 = temp[10];
		rrb[i].da3 = temp[11];

		rrb[i].vel1 = vec2Resolve(rrb[i].v1, rrb[i].dv1);
		rrb[i].vel2 = vec2Resolve(rrb[i].v2, rrb[i].dv2);
		rrb[i].vel3 = vec2Resolve(rrb[i].v3, rrb[i].dv3);
		rrb[i].acc1 = vec2Resolve(rrb[i].a1, rrb[i].da1);
		rrb[i].acc2 = vec2Resolve(rrb[i].a2, rrb[i].da2);
		rrb[i].acc3 = vec2Resolve(rrb[i].a3, rrb[i].da3);

		rrb[i].rvel = calculateResultant(rrb[i].vel1, rrb[i].vel2, rrb[i].vel3);
		rrb[i].racc = calculateResultant(rrb[i].acc1, rrb[i].acc2, rrb[i].acc3);
		rrb[i].rvel_dir = atan(rrb[i].rvel.y / rrb[i].rvel.x) * (180/PI);
	}

	fclose(config);
}


void pointMassPhysics(RectangleRB *rrb) {
	rrb->rvel.x += ((rrb->racc.x * TIME_FACTOR) * REDUCING_FACTOR);
	rrb->rvel.y += ((rrb->racc.y * TIME_FACTOR) * REDUCING_FACTOR);

	rrb->circle_target.x += ((rrb->rvel.x * TIME_FACTOR) * REDUCING_FACTOR);
	rrb->circle_target.y += ((rrb->rvel.y * TIME_FACTOR) * REDUCING_FACTOR);

	rrb->rvel_dir = atan(rrb->rvel.y / rrb->rvel.x) * (180/PI);
}

void AddRectRB(int x, int y, RectangleRB* rectRB) {
	// for showing the object
	rectRB->circle_target.w = 30;
	rectRB->circle_target.h = 30;
	rectRB->circle_target.x = x-15;
	rectRB->circle_target.y = y-15;
}

void InitialSetup() {
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	if (TTF_Init() == -1) {
		printf("SDL2_ttf not initialized. ERROR: %s\n", TTF_GetError());
	}
}

// for testing
void printRRB(RectangleRB *rrb, int rrb_counter) {
	for (int i = 0; i < rrb_counter; i++) {
		printf("rrb[%d]:\n %f %f %f %f %f %f\n %f %f %f %f %f %f\n\n",i, rrb[i].v1,
				rrb[i].v2,rrb[i].v3, rrb[i].a1, rrb[i].a2, rrb[i].a3, rrb[i].dv1, 
				rrb[i].dv2, rrb[i].dv3, rrb[i].da1, rrb[i].da2, rrb[i].da3);

		printf(" vec1: %f %f \n vec2: %f %f \n vec3: %f %f \n acc1: %f %f \n acc2: %f %f \n acc3: %f %f\n\n resultant velocity: %f %f\n resultant acceleration: %f %f\n resultant velocity direction: %f \n",
				rrb[i].vel1.x, 	rrb[i].vel1.y, rrb[i].vel2.x, rrb[i].vel2.y, rrb[i].vel3.x, rrb[i].vel3.y, 
				rrb[i].acc1.x, rrb[i].acc1.y, rrb[i].acc2.x, rrb[i].acc2.y, rrb[i].acc3.x, rrb[i].acc3.y,
				rrb[i].rvel.x, rrb[i].rvel.y, rrb[i].racc.x, rrb[i].racc.y, rrb[i].rvel_dir);
	}
}


bool rrbFloorClip(RectangleRB *rrb) {
	if (rrb->circle_target.y >= 600) {
		return false;
	}
	else {
		return true;
	}
}

// Using this to display instantaneous values on the window (incomplete)
void rrbGetInfo(RectangleRB *rrb) {
	float vel_resultant = sqrt((rrb->rvel.x * rrb->rvel.x) + (rrb->rvel.y * rrb->rvel.y));
	float vel_resultant_dir = atan(rrb->rvel.y / rrb->rvel.x) * (180/PI);
	float x_pos = rrb->circle_target.x;
	float y_pos = rrb->circle_target.y;

	printf("Resultant velocity = %f @ %f\tPosition: (%f, %f)\n", vel_resultant, vel_resultant_dir,
			x_pos, y_pos);
}

int main() {
	InitialSetup();

	const int screen_width = 1280, screen_height = 720;
	SDL_Window* window = SDL_CreateWindow("another try", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// IMAGE
	SDL_Texture* circle_image = IMG_LoadTexture(renderer, "../images/circle.png");
	SDL_Texture* background_image = IMG_LoadTexture(renderer, "../images/bg.png");
	SDL_Texture* ground_image = IMG_LoadTexture(renderer, "../images/ground.png");
	SDL_Texture* vector_arrow_image = IMG_LoadTexture(renderer, "../images/vector_arrow.png");
	SDL_Texture* resultant_arrow_image = IMG_LoadTexture(renderer, "../images/resultant_arrow.png");

	SDL_Rect bg_rect = {0, 0, 1920, 1080};
	SDL_Rect ground_rect = {0, screen_height - 100, 1920, 1080};
	SDL_Rect resultant_arrow_rect = {50, 300, 100, 4};
	SDL_Rect vector_arrow_rect = {50, 300, 100, 4};
	SDL_Point arrow_pivot_point = {0, 1};

	// TEXT
	unsigned int font_size = 100;
	SDL_Color font_color = {2, 2, 2};
	TTF_Font* font = TTF_OpenFont("../res/fonts/Ubuntu-Bold.ttf", font_size);
	SDL_Surface* text_surface;
	SDL_Texture* text_texture;
	SDL_Rect text_rect = {5, 5, 300, 70};

	char velocity_x[100];
	char velocity_y[100];
	char rrb_resultant[100];
	char r_direction[100];
	char final_string[400];
	// char *result;
	// int len = 0;

	// [IMPORTANT] KEEP TRACK OF RECTANGLE RBs'
	RectangleRB rrb[11];
	int rrb_counter = 0;

	// Set all values of rrb bodies to 0;
	for (int i = 0; i < 10; i++) {
		rrb[i].circle_target.x = 0;
		rrb[i].circle_target.y = 0;

		rrb[i].v1 = 0;
		rrb[i].v2 = 0;
		rrb[i].v3 = 0;

		rrb[i].a1 = 0;
		rrb[i].a2 = 0;
		rrb[i].a3 = 0;

		rrb[i].dv1 = 0;
		rrb[i].dv2 = 0;
		rrb[i].dv3 = 0;

		rrb[i].da1 = 0;
		rrb[i].da2 = 0;
		rrb[i].da3 = 0;
	}

	bool running = true, read_config = false, pause = false; 
	int return_count = 0, info_counter = 0;
	float resultant = 0;

	while(running) {
		Uint32 start = SDL_GetTicks();

		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}

			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && read_config == false) {
				if (rrb_counter < 10) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					AddRectRB(x, y, &rrb[rrb_counter]);	// testing

					rrb_counter += 1;
				}
				else {
					printf("can't create more objects\n");
				}
			}
			if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {

				// using o and p for pausing and playing because space is registering both pause
				// and play
				if (event.key.keysym.scancode == SDL_SCANCODE_O) {
					pause = true;
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_P && pause == true) {
					pause = false;
				}

				if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
					return_count += 1;
				}

				// RESET
				if (event.key.keysym.scancode == SDL_SCANCODE_R) {
					rrb_counter = 0;
					return_count = 0;
				}

				// Circle through object data
				if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
					if (info_counter > 10) {
						info_counter = 0;
					}
					else {
						info_counter++;
					}
				}

			}
		} // end user event loop

		// Read config
		if (return_count == 1) {
			readConfig("config", rrb, rrb_counter);
			// printRRB(rrb, rrb_counter);
			read_config = false;
		}

		// DRAWING
		SDL_RenderClear(renderer);

		// draw background and forground
		SDL_RenderCopy(renderer, background_image, NULL, &bg_rect);
		SDL_RenderCopy(renderer, ground_image, NULL, &ground_rect);

		// text drawing (THIS IS REDUCING PERFORANCE)
		resultant = sqrt((rrb[info_counter].rvel.x * rrb[info_counter].rvel.x) + (rrb[info_counter].rvel.y * rrb[info_counter].rvel.y));

		/*
		len = snprintf(NULL, 0, "%d\nvelocity: (%f, %f)\nResultant velocity: %f @ %f", 
				info_counter, rrb[info_counter].rvel.x, rrb[info_counter].rvel.y, resultant, rrb[info_counter].rvel_dir);
		result = malloc(len + 1);
		snprintf(result, len + 1, "%d\nvelocity: (%f, %f)\nResultant velocity: %f @ %f", 
				info_counter, rrb[info_counter].rvel.x, rrb[info_counter].rvel.y, resultant, rrb[info_counter].rvel_dir);


		strcat(velocity_x, "\n");
		gcvt(rrb[info_counter].rvel.y, 6, velocity_y);
		strcat(velocity_y, "\n");
		gcvt(resultant, 6, rrb_resultant);
		strcat(rrb_resultant, "\n");
		gcvt(rrb[info_counter].rvel_dir, 6, r_direction);

		strcat(final_string, "Resultant velocity_x: ");
		strcat(final_string, velocity_x);
		strcat(final_string, "Resultnat velocity_y:");
		strcat(final_string, velocity_y);
		strcat(final_string, "Resultant: ");
		strcat(final_string, rrb_resultant);
		strcat(final_string, " @ ");
		strcat(final_string, r_direction);

		text_surface = TTF_RenderText_Blended_Wrapped(font, final_string, font_color, 0);

		memset(final_string, 0, sizeof(final_string));

		text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		SDL_FreeSurface(text_surface);
		SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
		*/

		printf("%d. Resultant velocity = %f @ %f | Resultant Velocity Components = (%f, %f)\r", 
				info_counter, resultant, rrb[info_counter].rvel_dir, rrb[info_counter].rvel.x, rrb[info_counter].rvel.y);

		for (int i = 0; i < rrb_counter; i++) {
			if (return_count >= 2 && !pause && rrbFloorClip(&rrb[i])) {
				pointMassPhysics(&rrb[i]);
				// rrbGetInfo(&rrb[i]);
			}

			// SHOW VELOCITY VECTOR ARROWS
			if (return_count == 1) {
				vector_arrow_rect.x = rrb[i].circle_target.x + 15;
				vector_arrow_rect.y = rrb[i].circle_target.y + 13;

				if (rrb[i].v1 != 0) {
				SDL_RenderCopyEx(renderer, vector_arrow_image, NULL, &vector_arrow_rect, 
						rrb[i].dv1 * -1, &arrow_pivot_point, SDL_FLIP_NONE);
				}

				if (rrb[i].v2 != 0) {
				SDL_RenderCopyEx(renderer, vector_arrow_image, NULL, &vector_arrow_rect, 
						rrb[i].dv2 * -1, &arrow_pivot_point, SDL_FLIP_NONE);
				}

				if (rrb[i].v3 != 0) {
				SDL_RenderCopyEx(renderer, vector_arrow_image, NULL, &vector_arrow_rect, 
						rrb[i].dv3 * -1, &arrow_pivot_point, SDL_FLIP_NONE);
				}
			}

			// SHOW RESULTANT ARROW
			if (return_count >= 1 && truncf(sqrt((rrb[i].rvel.x * rrb[i].rvel.x) + 
							(rrb[i].rvel.y * rrb[i].rvel.y))) != 0) {

				resultant_arrow_rect.x = rrb[i].circle_target.x + 15;
				resultant_arrow_rect.y = rrb[i].circle_target.y + 13;

				// RESOLVE THIS
				SDL_RenderCopyEx(renderer, resultant_arrow_image, NULL, &resultant_arrow_rect, 
						rrb[i].rvel_dir, &arrow_pivot_point, SDL_FLIP_NONE);
			}

			SDL_RenderCopy(renderer, circle_image, NULL, &rrb[i].circle_target);

		}
		SDL_RenderPresent(renderer);
		// END DRAWING

		Uint32 elapsed_time = SDL_GetTicks() - start;
		if (elapsed_time < TIME_FACTOR) {
			SDL_Delay(TIME_FACTOR - elapsed_time);
		}

	} // end main loop
	// printRRB(&rrb, rrb_counter);

	SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	SDL_Quit();
	return 0;
}
