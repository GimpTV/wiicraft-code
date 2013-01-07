/*									

	(c)Mojang AB And Filiph Sandstr�m
									
									*/
#ifndef mainAPI_H
#define mainAPI_H

#define APIVERISON "01"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <cstdio>
#include <stdio.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <zlib.h>
//#include "worldAPI.h"
//#include "blockAPI"
//#include "playerAPI.h"
//#include "multiplayerAPI.h"
//#include "mobAPI.h"


using namespace std;

class API {
	private:
		bool APIRunning;
		bool APIVERISONBool;
		
	public:
		API();
		~API();
		void initAPI(int usingsd);
		void stopAPI();
		
		//Public valuse:
		bool existsAPI;
		bool existsTEXTURES;
};



#endif