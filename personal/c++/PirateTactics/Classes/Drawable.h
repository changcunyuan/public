/*
 *  Drawable.h
 *  Pirate Tactics
 *
 *  Created by Jonathan Chandler on 6/1/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include "Include.h"

class Drawable{	
public:
	Drawable();
	virtual ~Drawable();	
	
	virtual void draw(bool) = 0;	
};
	
#endif DRAWABLE_H_