#ifndef _DISPLAYLIST_H_
#define _DISPLAYLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>

#define POS_LIST_SIZE 1024 * 24
#define COL_LIST_SIZE 1024 * 24


class DisplayList
{
public:
//Constructor
	DisplayList();
//Methods
	int addColor(uint8_t *colP);
	int addColor(float r, float g, float b, float a = 1.0f);
	int addColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	
	int addPosition(float *posP);
	int addPosition(float x, float y, float z = 0.0f);
	
	void renderList();
	
	void cleanColorList();
	void cleanPositionList();

//private:
	uint32_t pos_index;
	uint32_t col_index;
	float ATTRIBUTE_ALIGN(32) pos_list[POS_LIST_SIZE];
	uint8_t ATTRIBUTE_ALIGN(32) col_list[COL_LIST_SIZE];
};


#endif
