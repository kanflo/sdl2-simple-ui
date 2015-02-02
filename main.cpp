#include <stdio.h>
#include <string>
#include <time.h>

#include <UIGUI.h>
#include <UIScreen.h>
#include <UIElement.h>
#include <UICaption.h>
#include <UIButton.h>
#include <UITextField.h>
#include <UIImage.h>
#include <UIImageButton.h>
#include <UITimer.h>
#include <UIMenu.h>
#include <UICanvasControl.h>


enum screenCommands {
	kMenuButtonPress = 1,
	kScreenButtonPress,
	kReturnButtonPress,
	kTextField1Enter,
	kTextField2Enter,
	kCancel,
	kQuit
};


#define SCREEN_WIDTH     1024
#define SCREEN_HEIGHT     768

void handleSecondScreen(UIGUI *ui, UIScreen *screen)
{
	SDL_Event event;
	UIScreen::setNeedRedraw();
	do {
		ui->run(screen, &event);
	} while(event.user.code != kReturnButtonPress);
}

int main(int argc, char ** argv)
{
	UICaption *gCaption;
	UITextField *gTextField1, *gTextField2;
	UIButton *gMenuButton;
	UIButton *screenButton;

	(void) SDL_RegisterEvents(1); // First call seem to return SDL_USEREVENT :-/

	SDL_Event event;
	SDL_Color bgColor1 = {0, 0, 128, 255};
	SDL_Color bgColor2 = {128, 128, 244, 255};
	

	printf("Creating UI\n");
	UIGUI *ui = new UIGUI(SCREEN_WIDTH, SCREEN_HEIGHT);
	printf("Creating screen\n");
	UIScreen *mainScreen = new UIScreen(ui);
	UIScreen *secondScreen = new UIScreen(ui);
	mainScreen->setBackgroundColor(bgColor1);
	secondScreen->setBackgroundColor(bgColor2);

	// Setup main screen
	gCaption = new UICaption(id_Any,  10,              SCREEN_HEIGHT-65, 0, 0, "", "impact", 24, UIGUI::White, leftAdjust);
	gTextField1       = new UITextField(id_Any,  20, 20, 300, 40, kTextField1Enter, "Edit me & press enter", "impact", 24, UIGUI::White);
	gTextField2       = new UITextField(id_Any,  20, 80, 300, 40, kTextField2Enter, "", "impact", 24, UIGUI::White);
	gMenuButton       = new UIButton(id_Any,  SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-60, 200, 40, kMenuButtonPress, "Menu", "impact", 24, UIGUI::White);
	screenButton      = new UIButton(id_Any,  SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2, 200, 40, kScreenButtonPress, "2nd screen", "impact", 24, UIGUI::White);
	mainScreen->AddElement((UIElement *) gCaption);
	mainScreen->AddElement((UIElement *) gTextField1);
	mainScreen->AddElement((UIElement *) gTextField2);
	mainScreen->AddElement((UIElement *) gMenuButton);
	mainScreen->AddElement((UIElement *) screenButton);

	// Setup secondary screen
	secondScreen->AddElement((UIElement *) new UICaption(id_Any,  10, 10, 0, 0, "This is the second screen", "impact", 24, UIGUI::White, leftAdjust));
	secondScreen->AddElement((UIElement *) new UIButton(id_Any,  SCREEN_WIDTH-210, 10, 200, 40, kReturnButtonPress, "Return", "impact", 24, UIGUI::White));

	do {
		ui->run(mainScreen, &event);
//		printf("App got event %x code %d [0x%x]\n", event.type, event.user.code, event.user.code);
		if (event.type == mainScreen->getEventType()) {
			switch(event.user.code) {
				case kTextField1Enter:
					gCaption->setCaption(gTextField1->GetCaption());
					break;
				case kScreenButtonPress:
					handleSecondScreen(ui, secondScreen);
					UIScreen::setNeedRedraw();
					break;
				case kMenuButtonPress:
					{
						UIMenu *menu = new UIMenu(ui);
						int32_t cmd;
						menu->addCmd("Quit", kQuit);
						menu->addSpacer();
						menu->addCmd("Cancel", kCancel);
						cmd = menu->display();
						delete menu;
						printf("Got %d from menu\n", cmd);
						switch(cmd) {
							case kQuit:
								exit(0);
								break;
						}
					}
					break;
			}
		}
	} while(event.user.code != event_Quit);
	delete ui;
	return 0;
}
