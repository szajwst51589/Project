#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include "SDL.h"
#include "Button.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *sClock, *sClockSet, *sClockDay[7], *sHourHand, *sMinuteHand, *sSecondHand;
SDL_Texture *tClock, *tClockSet, *tClockDay[7], *tHourHand, *tMinuteHand, *tSecondHand;
SDL_Rect hourHandRect, minuteHandRect, secondHandRect;
SDL_Point centre;
Button *day, *set;
int state = 0, wday = 0;	//state 0: normal;	1: wday;	2: set
double hourHandAngle, minuteHandAngle, secondHandAngle;

void SDLInit() {
	if(SDL_WasInit(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "Init error: SDL is already running." << std::endl;
		return;
	}
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		throw SDL_GetError();
	}
	std::cout << "Init SDL successfully." << std::endl;
}

void SDLQuit() {
	if(SDL_WasInit(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Quit error: SDL is not running." << std::endl;
		return;
	}
	SDL_Quit();
	std::cout << "Quit SDL successfully." << std::endl;
}

void paint(int flag = -2) {
	
	SDL_RenderClear(renderer);
	if(flag == -2)											//normal
		SDL_RenderCopy(renderer, tClock, NULL, NULL);
	else if(flag == -1)										//set
		SDL_RenderCopy(renderer, tClockSet, NULL, NULL);
	else													//day
		SDL_RenderCopy(renderer, tClockDay[flag], NULL, NULL);
	SDL_RenderCopyEx(renderer, tHourHand, NULL, &hourHandRect, hourHandAngle, &centre, SDL_FLIP_NONE);
	SDL_RenderCopyEx(renderer, tMinuteHand, NULL, &minuteHandRect, minuteHandAngle, &centre, SDL_FLIP_NONE);
	SDL_RenderCopyEx(renderer, tSecondHand, NULL, &secondHandRect, secondHandAngle, &centre, SDL_FLIP_NONE);
	SDL_RenderPresent(renderer);

	return;
}

void run() {
	bool endClock = false;
	int flag1 = 0, flag2 = 0, flag3 = 0;
	while(endClock == false) {
		SDL_Event clockEvent;
		int count = 0;
		count++;
		secondHandAngle += 6;
		minuteHandAngle += 0.1;
		if(count >= 60) {
			count -= 60;
			hourHandAngle += 0.5;
		}
		if(secondHandAngle > 360)
			secondHandAngle -= 360;
		if(minuteHandAngle > 360)
			minuteHandAngle -= 360;
		if(hourHandAngle > 360)
			hourHandAngle -= 360;
		SDL_Delay(1000);
		if(flag1 == 1) {
			if(state != 2) {
				paint(wday);
				state = 1;
			}
		}
		else if(flag1 == -1) {
			if(state != 2) {
				paint();
				state = 0;
			}
		}
		else if(flag2 == 2) {
			if(state != 2) {
				state = 2;
				paint(-1);
			}
			else {
				state = 0;
				paint();
			}
		}
		else if(flag3 == 3) {
			if(state == 2) {
				while(minuteHandAngle > 360)
					minuteHandAngle -= 360;
				while(hourHandAngle > 360)
					minuteHandAngle -= 360;
				minuteHandAngle += 6*clockEvent.wheel.y;
				hourHandAngle += 0.5*clockEvent.wheel.y;
			}
			paint(-1);
		}
		else {
			if(state == 1)
				paint(wday);
			else if(state == 2)
				paint(-1);
			else
				paint();
		}
		
		while(SDL_PollEvent(&clockEvent) != 0) {
			/*
			flag1 = day->buttonMotionEvent(clockEvent);
			flag2 = set->buttonClickEvent(clockEvent);
			flag3 = set->buttonWheelEvent(clockEvent);
			*/
			//
			if(clockEvent.type == SDL_MOUSEMOTION)
				flag1 = day->buttonMotionEvent(clockEvent);
			if(clockEvent.type == SDL_MOUSEBUTTONDOWN)
				flag2 = set->buttonClickEvent(clockEvent);
			if(clockEvent.type == SDL_MOUSEWHEEL)
				flag3 = set->buttonWheelEvent(clockEvent);
			//
			if(clockEvent.type == SDL_QUIT)
				endClock = true;
			if(clockEvent.type == SDL_KEYUP)
				if(clockEvent.key.keysym.sym == SDLK_ESCAPE)
					endClock = true;
		}
	}
}

bool loadImage() {
	sClock = NULL;
	try {
		sClock = SDL_LoadBMP("clock.bmp");
		if(sClock == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tClock = NULL;
	try {
		tClock = SDL_CreateTextureFromSurface(renderer, sClock);
		if(tClock == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	//
	sClockSet = NULL;
	try {
		sClockSet = SDL_LoadBMP("clockSetting.bmp");
		if(sClockSet == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tClockSet = NULL;
	try {
		tClockSet = SDL_CreateTextureFromSurface(renderer, sClockSet);
		if(tClockSet == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	//
	sClockDay[0] = NULL;
	try {
		sClockDay[0] = SDL_LoadBMP("clockSun.bmp");
		if(sClockDay[0] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tClockDay[0] = NULL;
	try {
		tClockDay[0] = SDL_CreateTextureFromSurface(renderer, sClockDay[0]);
		if(tClockDay[0] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	//
	sClockDay[1] = NULL;
	try {
		sClockDay[1] = SDL_LoadBMP("clockMon.bmp");
		if(sClockDay[1] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tClockDay[1] = NULL;
	try {
		tClockDay[1] = SDL_CreateTextureFromSurface(renderer, sClockDay[1]);
		if(tClockDay[1] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	//
	sClockDay[2] = NULL;
	try {
		sClockDay[2] = SDL_LoadBMP("clockTues.bmp");
		if(sClockDay[2] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tClockDay[2] = NULL;
	try {
		tClockDay[2] = SDL_CreateTextureFromSurface(renderer, sClockDay[2]);
		if(tClockDay[2] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	//
	sClockDay[3] = NULL;
	try {
		sClockDay[3] = SDL_LoadBMP("clockWed.bmp");
		if(sClockDay[3] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tClockDay[3] = NULL;
	try {
		tClockDay[3] = SDL_CreateTextureFromSurface(renderer, sClockDay[3]);
		if(tClockDay[3] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	//
	sClockDay[4] = NULL;
	try {
		sClockDay[4] = SDL_LoadBMP("clockThur.bmp");
		if(sClockDay[4] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tClockDay[4] = NULL;
	try {
		tClockDay[4] = SDL_CreateTextureFromSurface(renderer, sClockDay[4]);
		if(tClockDay[4] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	//
	sClockDay[5] = NULL;
	try {
		sClockDay[5] = SDL_LoadBMP("clockFri.bmp");
		if(sClockDay[5] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tClockDay[5] = NULL;
	try {
		tClockDay[5] = SDL_CreateTextureFromSurface(renderer, sClockDay[5]);
		if(tClockDay[5] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	//
	sClockDay[6] = NULL;
	try {
		sClockDay[6] = SDL_LoadBMP("clockSat.bmp");
		if(sClockDay[6] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tClockDay[6] = NULL;
	try {
		tClockDay[6] = SDL_CreateTextureFromSurface(renderer, sClockDay[6]);
		if(tClockDay[6] == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	//
	sHourHand = NULL;
	try {
		sHourHand = SDL_LoadBMP("hourHand.bmp");
		if(sHourHand == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tHourHand = NULL;
	try {
		tHourHand = SDL_CreateTextureFromSurface(renderer, sHourHand);
		if(tHourHand == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}
	
	sMinuteHand = NULL;
	try {
		sMinuteHand = SDL_LoadBMP("minuteHand.bmp");
		if(sMinuteHand == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tMinuteHand = NULL;
	try {
		tMinuteHand = SDL_CreateTextureFromSurface(renderer, sMinuteHand);
		if(tMinuteHand == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	sSecondHand = NULL;
	try {
		sSecondHand = SDL_LoadBMP("secondHand.bmp");
		if(sSecondHand == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	tSecondHand = NULL;
	try {
		tSecondHand = SDL_CreateTextureFromSurface(renderer, sSecondHand);
		if(tSecondHand == NULL)
			throw SDL_GetError();
	} catch(const char *s) {
		std::cerr << s << std::endl;
		return false;
	}

	return true;
}

void setInitAngle() {
	hourHandAngle = 180;
	minuteHandAngle = 180;
	secondHandAngle = 180;
	struct tm *nowTime;
	time_t now = time(NULL);
	nowTime = localtime(&now);
	hourHandAngle += (double)nowTime->tm_hour * 30 + (double)nowTime->tm_min / 2;
	minuteHandAngle += (double)nowTime->tm_min * 6 + (double)nowTime->tm_sec / 10;
	secondHandAngle += (double)nowTime->tm_sec * 6;
	wday = nowTime->tm_wday;
}

SDL_Rect getRect(SDL_Surface* background, SDL_Surface* image) {
	SDL_Rect rect;
	rect.x = background->w/2;
	rect.y = background->h/2;
	rect.w = image->w;
	rect.h = image->h;
	return rect;
}

int main(int argc, char** argv) {
	try {
		SDLInit();
	} catch(const char* s) {
		std::cerr << s << std::endl;
		return -1;
	}

	window = NULL;
	try {
		window = SDL_CreateWindow("clock", 400, 400, 300, 300, SDL_WINDOW_SHOWN);
		if(window == NULL)
			throw SDL_GetError();
	} catch(const char* s) {
		std::cerr << s << std::endl;
		return -1;
	}

	renderer = NULL;
	try {
		renderer = SDL_CreateRenderer(window, -1, 0);
		if(renderer == NULL)
			throw SDL_GetError();
	} catch(const char* s) {
		std::cerr << s << std::endl;
		return -1;
	}
	
	try {
		if(loadImage() == false)
			throw SDL_GetError();
	} catch(const char* s) {
		std::cerr << s << std::endl;
		return -1;
	}

	hourHandRect = getRect(sClock, sHourHand);
	minuteHandRect = getRect(sClock, sMinuteHand);
	secondHandRect = getRect(sClock, sSecondHand);
	centre.x = 0; centre.y = 0;
	
	setInitAngle();
	paint();

	day = new Button(198, 146, 21, 10);
	set = new Button(244, 137, 16, 24);

	run();


	atexit(SDLQuit);
	return 0;
}