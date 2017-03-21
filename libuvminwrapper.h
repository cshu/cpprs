#pragma once


//undone
	typedef bool log_t;
#define THROW(x) throw x;
#define LOG_ERR(x) x=true;
	struct loop{
		uv_loop_t *puvl;
		log_t &log;
		loop(log_t log) : log(log), puvl{new uv_loop_t}{
			if(uv_loop_init(puvl)){
				THROW(0)
			}
		}
		//undone
		~loop(){
			if(uv_loop_close(puvl)){
				LOG_ERR(log)
			}
			delete puvl;
		}
		
	};


