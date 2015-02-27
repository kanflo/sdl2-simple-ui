#include <SDL_thread.h>
#include <SDL_timer.h>
#include <UIGUI.h>
#include <UIScreen.h>
#include <UIHumanInput.h>
#include <iostream>
#include <stdio.h>
#include <stdio.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;


#define KEYBOARD_PATH "/dev/input/event0"
#define MOUSE_PATH "/dev/input/mouse0"

/*
struct input_event {
 26         struct timeval time;
 27         __u16 type;
 28         __u16 code;
 29         __s32 value;
 30 };
 */

static int humanInputKeyboardThread(void *ptr)
{
	size_t file;
	struct input_event event;

	if((file = open(KEYBOARD_PATH, O_RDWR)) < 0) {
		printf("Failed to open %s\n", KEYBOARD_PATH);
		return 0;
	}

	while (1) {
        size_t reader;
        reader = read(file, &event, sizeof(struct input_event));
        if (reader) {
	        printf("E0: %d %d %d\n", event.type, event.code, event.value);
        }
	}

	close(file);
}

static int humanInputMouseThread(void *ptr)
{
	size_t file;
	struct input_event event;

	if((file = open(MOUSE_PATH, O_RDWR)) < 0) {
		printf("Failed to open %s\n", MOUSE_PATH);
		return 0;
	}

	while (1) {
        size_t reader;
        reader = read(file, &event, sizeof(struct input_event));
        if (reader) {
	        printf("E1: %d %d %d\n", event.type, event.code, event.value);
        }
	}

	close(file);
}


UIHumanInput::UIHumanInput(uint32_t inId)
 : UIElement(inId, 40, 40, 0, 0)
{
	SDL_Surface *temp;
	temp = IMG_Load("cursor.png");
	if (!temp) {
		printf("UIHumanInput: Failed to load cursor\n");
		printf("IMG Error: %s / %s\n", IMG_GetError(), SDL_GetError());
		assert(0);
	}
	mImageTexture = SDL_CreateTextureFromSurface(mRenderer, temp);
	assert(mImageTexture);
	SDL_QueryTexture(mImageTexture, NULL, NULL, &mRect.w, &mRect.h);
	mRect.w /= 10;
	mRect.h /= 10;
	SDL_FreeSurface(temp);
	mKeyboardThread = SDL_CreateThread(humanInputKeyboardThread, "HumanInputKeyboardThread", (void*) this);
	mMouseThread = SDL_CreateThread(humanInputMouseThread, "HumanInputMouseThread", (void*) this);
}

UIHumanInput::~UIHumanInput()
{
	if (mImageTexture) {
		SDL_DestroyTexture(mImageTexture);
	}
}

void UIHumanInput::Render()
{
	SDL_RenderCopy(mRenderer, mImageTexture, NULL, &mRect);
}

void UIHumanInput::moveTo(int32_t x, int32_t y)
{
	mRect.x = x;
	mRect.y = y;
	UIScreen::needsRedraw();
}
