#ifndef __UIHUMANINPUT_H__
#define __UIHUMANINPUT_H__

#include <UIElement.h>
#include <UIControl.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

using std::string;

class UIHumanInput : private UIElement
{
	public:
		UIHumanInput(uint32_t inId);
		~UIHumanInput();
		void Render();

		void moveTo(int32_t x, int32_t y);
	protected:
		SDL_Texture *mImageTexture;
		SDL_Surface *mSurface;
	    SDL_Thread *mKeyboardThread;
	    SDL_Thread *mMouseThread;
};

#endif // __UIHUMANINPUT_H__
