#ifndef PROJECTILE_C34_H_
#define PROJECTILE_C34_H_

#include "GameObject.h"

#include <list>

class Projectile;

typedef std::shared_ptr<Projectile> ProjectilePtr;
typedef std::list<ProjectilePtr> ProjectileList;


class Projectile
: public GameObject
{
public:
	Projectile(const Coord2d<float>&, 
				  const Coord2d<float>&);
	
	// Draw
	bool update();
};

#endif
                                                                                                                                                                                                                                                                                                                                                  