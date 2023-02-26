/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */
#ifndef PowerUps_h
#define PowerUps_h

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "ScreenItem.h"

#define WOBBLE_0 45
#define WOBBLE_1 75

class Global;
class PowerUp;

//====================================================================
class PowerUps
{
public:
	enum Type { Shields, SuperShields, Repair,
				HeroAmmo00, HeroAmmo01, HeroAmmo02,
				NumPowerUps };

	PowerUps();
	~PowerUps();

	void	addPowerUp(PowerUp*);

	void	update();
	void	drawGL();
	void	clear();

	PowerUp	*getFirst();
	PowerUp	*getNext();
	void	remove(PowerUp*);

	void	loadTextures();
	void	deleteTextures();

private:
	PowerUp	*pwrUpRoot;
	PowerUp	*currentPwrUp;
	float	pwrUpSize[NumPowerUps][2];
	float	pwrUpColor[NumPowerUps][4];

	GLuint	tex[NumPowerUps];
	GLuint	pwrTex;

	float	speed;
	float	wobble_0[WOBBLE_0];
	float	wobble_1[WOBBLE_1];

	int	activeCount;

private:
	Global	*game;
};

//====================================================================
class PowerUp : public ScreenItem
{
public:
	PowerUp(PowerUps::Type t, float p[3], float pwr = 1.0, float *v=0);
	~PowerUp();

	PowerUps::Type	type;

	int thisCount() { return count_this; }
	float power;

	void	seal();
	PowerUp *back;
	PowerUp *next;

friend class PowerUps;

private:
	static int count_this;
};

#endif // PowerUps_h
