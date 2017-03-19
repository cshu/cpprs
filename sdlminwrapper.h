#pragma once

void sdlgeterrorwritelog(void){
	auto sdlerr=SDL_GetError();
	if(sdlerr[0]){
		std::clog<<"SDL Error: "<<sdlerr<<'\n';
		SDL_ClearError();
	}
	std::clog<<std::flush;
}

struct sdl{
	sdl(Uint32 flags){
		if(SDL_Init(flags)){
			LOG_ERR_TRY_GET_SDL_ERRs
			throw 0;//undone
		}
	}
	~sdl(){
		SDL_Quit();
	}
};

struct window{
	SDL_Window *sdlw;
	window(const char* title,int x,int y,int w,int h,Uint32 flags):sdlw(SDL_CreateWindow(title,x,y,w,h,flags)){
		if(!sdlw){
			LOG_ERR_TRY_GET_SDL_ERRs
			throw 0;//undone
		}
	}
	~window(){
		SDL_DestroyWindow(sdlw);
	}
};

