/*

	(c)Mojang AB And WiiCraft theme

									*/

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <wiikeyboard/keyboard.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fat.h>
#include <sdcard/wiisd_io.h>
#include <png.h>
#include <pngu.h>

#include "grass_png.h"
#include "stone_png.h"

//Classes:
#include "debug.h"
#include "init.h"
#include "mainAPI.h"
#include "utils.h"
#include "World.hpp"
#include "Camera.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "Image.hpp"

extern "C" {
	extern void __exception_setreload(int t);
}

//Extern variables
	extern Mtx44 projection;
	extern Mtx view, model, modelview;
	extern float fps;
	extern u32 pressed;
	extern u32 expansion_type;
	extern struct joystick_t *js;
	extern WPADData *wmote_data;
//

//--------------------------------------------------
	//Wiilight
	//--------------------------------------------------
	lwp_t light_thread = 0;
	void *light_loop (void *arg);
	vu32 *light_reg = (u32*) HW_GPIO;
	bool light_on = false;
	u8 light_level = 0;
	struct timespec light_timeon = { 0 };
	struct timespec light_timeoff = { 0 };

	//----------------------------------------------------------
	//Function's:
	//----------------------------------------------------------

	void WIILIGHT_TurnOn(){
	*(u32*)0xCD0000C0 |= 0x20;
}

	void WIILIGHT_TurnOff(){
	*(u32*)0xCD0000C0 &= ~0x20;
}

void WIILIGHT_SetLevel(int level){
	light_level = MIN(MAX(level, 0), 100);
	// Calculate the new on/off times for this light intensity
	u32 level_on;
	u32 level_off;
	level_on = (light_level * 2.55) * 40000;
	level_off = 10200000 - level_on;
	light_timeon.tv_nsec = level_on;
	light_timeoff.tv_nsec = level_off;
}

World world;
void UpdateCamera();
void MoveCamera();

int main(int argc, char **argv)
{	
	// In the event of a code dump, the app will exit after 10 seconds (unless you press POWER)
	__exception_setreload(10);
	
	fatInitDefault();
	fatMountSimple("sd", &__io_wiisd);	
	WPAD_Init();
	InitVideo();
	InitPad();
	initFPS();
	
	int useSD;
	
	
	//Disk light turn on and init
	//WIILIGHT_Init();
	WIILIGHT_SetLevel(255);
	WIILIGHT_TurnOn();
	
	//if(*argv[1] == NULL){
	//	useSD = 3;
	//}
	
	//Check if it uses SD or USB:
	char test;
		sscanf(argv[0], "%c", &test); //read first character from argv[0] into test
		if(test == 115){ useSD = 1; } //first character = s
		else if(test == 117){ useSD = 2; } //first character = u
	
	//Init's that's can't be made in c:
	
	//API:
	API mainAPI;
	mainAPI.initAPI(useSD);
	
	//END OF INIT'S
	
	Image grass((uint8_t *)grass_png);
	Image stone((uint8_t *)stone_png);

	while(1)
	{
		Clean();
		UpdatePad();
		printf("FPS: %f\n", fps);
		printf("posx: %f   posy: %f  posz: %f\n", world.player->position.x, world.player->position.y, world.player->position.z);
		printf("pitch: %f   yaw: %f\n", world.player->pitch, world.player->yaw);
		printf("size: %i\n", world.chunkHandler->chunkList.size());
		
		printf("chunkX: %i  chunkY: %i  chunkZ: %i\n", world.player->chunk_x, world.player->chunk_y, world.player->chunk_z);
		printf("blockX: %i  blockY: %i  blockZ: %i\n", world.player->block_x, world.player->block_y, world.player->block_z);	
		printf("player status: %s  velocity.y: %f\n", world.player->status == ON_AIR ? "AIR" : "GROUND",world.player->velocity.y);


		grass.setGX(GX_TEXMAP0);
		DrawCubeTex(0,0,-5);
		
		world.update();
		world.drawChunks();

			

		MoveCamera();
		UpdateCamera();
		SwapBuffer();
		FPS(&fps);
		if (pressed & WPAD_BUTTON_HOME ) break;
	}
	WPAD_Shutdown();
	EndVideo();
	exit(0);
	return 0;
}

void MoveCamera()
{
	/*if(expansion_type == WPAD_EXP_NUNCHUK)
	{
		if(js->mag >= 0.2f)
		{
			float n_angle = DegToRad(NunchukAngle(js->ang));
			world.player->move( cos(n_angle)*js->mag / 5.0f, world.player->camera->getRightVector());
			world.player->move(-sin(n_angle)*js->mag / 5.0f, world.player->camera->getForwardVector());	
		}
	}*/
	Chunk *cp;
	Block *bp;
	
		/*cp = chunkHandler->chunkList[getWorldIndex(world.player->chunk_x, world.player->chunk_y, world.player->chunk_z)];
		cp = cp->rightNeighbour;
		bp = cp->blockList[3][0][0];
		bp->transparent = true;
		cp->needsUpdate = true;*/
	
	if(pressed & WPAD_BUTTON_A)
	{
		cp = world.chunkHandler->chunkList[getWorldIndex(world.player->chunk_x, world.player->chunk_y, world.player->chunk_z)];
		bp = cp->blockList[world.player->block_z][world.player->block_y][world.player->block_x];
		bp->transparent = true;
		cp->needsUpdate = true;
		if(world.player->block_x == 0)
			world.chunkHandler->setChunkToUpdate(cp->leftNeighbour);
		if(world.player->block_x == (CHUNK_SIZE-1))
			world.chunkHandler->setChunkToUpdate(cp->rightNeighbour);
		if(world.player->block_y == 0)
			world.chunkHandler->setChunkToUpdate(cp->downNeighbour);
		if(world.player->block_y == (CHUNK_SIZE-1))
			world.chunkHandler->setChunkToUpdate(cp->upNeighbour);
		if(world.player->block_z == 0)
			world.chunkHandler->setChunkToUpdate(cp->backNeighbour);
		if(world.player->block_z == (CHUNK_SIZE-1))
			world.chunkHandler->setChunkToUpdate(cp->frontNeighbour);
	}


	if(pressed & WPAD_BUTTON_B)
	{
		cp = world.chunkHandler->chunkList[getWorldIndex(world.player->chunk_x, world.player->chunk_y, world.player->chunk_z)];
		bp = cp->blockList[world.player->block_z][world.player->block_y][world.player->block_x];
		bp->transparent = false;
		cp->needsUpdate = true;
		if(world.player->block_x == 0)
			world.chunkHandler->setChunkToUpdate(cp->leftNeighbour);
		if(world.player->block_x == (CHUNK_SIZE-1))
			world.chunkHandler->setChunkToUpdate(cp->rightNeighbour);
		if(world.player->block_y == 0)
			world.chunkHandler->setChunkToUpdate(cp->downNeighbour);
		if(world.player->block_y == (CHUNK_SIZE-1))
			world.chunkHandler->setChunkToUpdate(cp->upNeighbour);
		if(world.player->block_z == 0)
			world.chunkHandler->setChunkToUpdate(cp->backNeighbour);
		if(world.player->block_z == (CHUNK_SIZE-1))
			world.chunkHandler->setChunkToUpdate(cp->frontNeighbour);
	}
	
	
	/*if(pressed & WPAD_NUNCHUK_BUTTON_C)
		world.player->position.y += 0.25f;
	if(pressed & WPAD_NUNCHUK_BUTTON_Z)
		world.player->position.y -= 0.25f;
		
	if(pressed & WPAD_BUTTON_1)
		world.player->position.y += 0.25f;
	if(pressed & WPAD_BUTTON_2)
		world.player->position.y -= 0.25f;
		
	if(pressed & WPAD_BUTTON_UP)
		world.player->pitch += 0.02f;
	if(pressed & WPAD_BUTTON_DOWN)
		world.player->pitch -= 0.02f;
		
	if(pressed & WPAD_BUTTON_RIGHT)
		world.player->yaw -= 0.02f;
	if(pressed & WPAD_BUTTON_LEFT)
		world.player->yaw += 0.02f;*/
		
}


void UpdateCamera()
{
	guMtxConcat(world.getCameraView(), model, modelview);
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);
}

