#ifndef __UITEXTFIELD_H__
#define __UITEXTFIELD_H__

#include <UIElement.h>
#include <UIControl.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

using std::string;

class UITextField : public UIControl
{
	public:
		UITextField(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, uint32_t inMessage, string inCaption, string inFontName, uint32_t inFontSize, SDL_Color &inColor);
		void Render();
		~UITextField();
		void SetCaption(string inNewCaption);
		string GetCaption();

		bool MayReceiveFocus();
		void StartedTracking();
		void StoppedTracking();
		void StartedFocus();
		void StoppedFocus();

		void ToggleCaret() {mCaretToggle = !mCaretToggle; };

		uint32_t HandleKeyDownEvent(SDL_Event *inEvent);
		uint32_t HandleMouseUpEvent(SDL_Event *inEvent);

	protected:
		void RenderText();

		string mString;
		SDL_Texture *mTextTexture;
		SDL_Texture *mTextShadowTexture;
		string mFontPath;
		uint32_t mFontSize;
		SDL_Color mColor;
		uint32_t mLastTicks;
		uint32_t mCaretToggle;
		SDL_TimerID mTimerId;
};
#endif // __UITEXTFIELD_H__
