#include <UIGUI.h>
#include <UIScreen.h>
#include <UITextField.h>
#include <iostream>
#include <stdio.h>

#define CARET_TIME_MS (500)

using namespace std;

Uint32 caretTimerCallback(Uint32 interval, void *param)
{
	UIScreen::setNeedRedraw();
	return interval;
}

UITextField::UITextField(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, uint32_t inMessage, string inCaption, string inFontName, uint32_t inFontSize, SDL_Color &inColor)
 : UIControl(inId, inX, inY, inWidth, inHeight, inMessage)
{
	string fontSuffix = ".ttf";
	mString = inCaption;
	mTextTexture = NULL;
	mTextShadowTexture = NULL;
	mFontSize = inFontSize;
	mColor = inColor;
	mFontPath = "c:\\WINDOWS\\Fonts\\";
	mFontPath += inFontName;
	mFontPath += fontSuffix;
	RenderText();
	mLastTicks = SDL_GetTicks();
	mCaretToggle = 0;
	mTimerId = 0;
}

UITextField::~UITextField()
{
	SDL_DestroyTexture(mTextTexture);
	SDL_DestroyTexture(mTextShadowTexture);
}

void UITextField::RenderText()
{
	if (mTextTexture) {
		SDL_DestroyTexture(mTextTexture);
	}
	if (mTextShadowTexture) {
		SDL_DestroyTexture(mTextShadowTexture);
	}

	mTextTexture = UIGUI::RenderText(mString, mFontPath, mFontSize, mHasFocus ? UIGUI::FocusColor : mColor);
	mTextShadowTexture = UIGUI::RenderText(mString, mFontPath, mFontSize, UIGUI::ShadowColor);
	UIScreen::setNeedRedraw();
}

void UITextField::Render()
{
	SDL_Rect rect = mRect;
	int32_t width = 0, height = 0;
	SDL_QueryTexture(mTextTexture, NULL, NULL, &width, &height);
   	rect.x += 4;
   	rect.y += 4;
   	rect.w = width;
   	rect.h = height;

	SDL_Color *drawColor = (SDL_Color*) &(mHasFocus ? UIGUI::FocusColor : mColor);
	SDL_Color *fillColor = (SDL_Color*) &(UIGUI::BackgroundColor);

	SDL_SetRenderDrawColor(mRenderer, fillColor->r, fillColor->g, fillColor->b, fillColor->a);
	SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(mRenderer, &mRect);

	SDL_SetRenderDrawColor(mRenderer, drawColor->r, drawColor->g, drawColor->b, drawColor->a);
	SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
	SDL_RenderDrawRect(mRenderer, &mRect);

	if (mHasFocus) { // Draw the caret 
		mCaretToggle = mCaretToggle ? 0 : 1;
		SDL_Rect caretRect;
		caretRect.x = mRect.x + width + 6;
		caretRect.y = mRect.y+5;
		caretRect.w = 2;
		caretRect.h = mRect.h-10;
		if (mCaretToggle) {
			SDL_SetRenderDrawColor(mRenderer, UIGUI::FocusColor.r, UIGUI::FocusColor.g, UIGUI::FocusColor.b, UIGUI::FocusColor.a);
			SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
			SDL_RenderFillRect(mRenderer, &caretRect);
		}
	}

	SDL_Rect shadowRect = rect;
	if (mColor.r != UIGUI::ShadowColor.r || mColor.g != UIGUI::ShadowColor.g || mColor.b != UIGUI::ShadowColor.b) {
		if (mFontSize > 25) {
			shadowRect.x += 2;
			shadowRect.y += 2;
		} else {
			shadowRect.x++;
			shadowRect.y++;
		}
		SDL_RenderCopy(mRenderer, mTextShadowTexture, NULL, &shadowRect);
	}
	SDL_RenderCopy(mRenderer, mTextTexture, NULL, &rect);
}

void UITextField::SetCaption(string inNewCaption)
{
	mString = inNewCaption;
	RenderText();
}

string UITextField::GetCaption()
{
	return mString;
}

bool UITextField::MayReceiveFocus()
{
	return true;
}

void UITextField::StartedTracking()
{
	RenderText();
}
void UITextField::StoppedTracking()
{
	RenderText();
}

void UITextField::StartedFocus()
{
	mCaretToggle = 1;
	SDL_StartTextInput();
	mTimerId = SDL_AddTimer(CARET_TIME_MS, caretTimerCallback, NULL);
	RenderText();
}

void UITextField::StoppedFocus()
{
	(void) SDL_RemoveTimer(mTimerId);
	mTimerId = 0;
	RenderText();
	SDL_StopTextInput();
}

uint32_t UITextField::HandleKeyDownEvent(SDL_Event *inEvent)
{
	if (inEvent->type == SDL_KEYDOWN) {
		switch (inEvent->key.keysym.sym) {
			case SDLK_RETURN:
				return mMessage;
				break;
			case SDLK_BACKSPACE:
			case SDLK_DELETE:
				if (mString.length() > 1) {
					mString = mString.substr(0, mString.length()-1);
				} else {
					mString.assign("");
				}
				break;
			// Keys we do not want the text field to listen to
			case SDLK_ESCAPE:
				return msg_None;
				break;

			default:
				break;
				// Accept input

		}
	} else if (inEvent->type == SDL_TEXTINPUT) {
		// TODO: Handle scrolling text fields for arbitrary string lengths
		int32_t width, height;
		SDL_TextInputEvent *event = (SDL_TextInputEvent*) inEvent;
		string newString = mString;
		newString.append(event->text);
		// TODO: User int TTF_SizeUTF8(TTF_Font *font, const char *text, int *w, int *h);
		SDL_Texture *newText = UIGUI::RenderText(newString, mFontPath, mFontSize, mColor);
		SDL_QueryTexture(newText, NULL, NULL, &width, &height);
		SDL_DestroyTexture(newText);
		if (width < mRect.w-2*6) {
			mString.append(event->text);
			mCaretToggle = 1;
		}

	}

	RenderText();
	return msg_None;
}

// We do not want the text field to send its messege simply by getting clicked-
uint32_t UITextField::HandleMouseUpEvent(SDL_Event *inEvent)
{
	return msg_None;
}
