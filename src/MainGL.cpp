/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 * Copyright 2008 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "gettext.h"

#include "MainGL.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <sys/stat.h>

#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || (defined(HAVE_OPENGL_GL_H) && defined(HAVE_OPENGL_GLU_H))
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#if IMAGE_GLPNG
#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_GLPNG_GLPNG_H)
#include <glpng/glpng.h>
#else
#include <GL/glpng.h>
#endif
#endif

#if defined(TEXT_GLC)
#include "TextGLC.h"
#endif
#if defined(TEXT_FTGL)
#include "TextFTGL.h"
#endif

#include "Config.h"

#include "extern.h"
#include "Global.h"
#include "HiScore.h"
#include "EnemyFleet.h"
#include "HeroAmmo.h"
#include "EnemyAmmo.h"
#include "HeroAircraft.h"
#include "Explosions.h"
#include "PowerUps.h"
#include "Audio.h"
#include "MenuGL.h"
#include "StatusDisplay.h"
#include "ScreenItemAdd.h"

#include "GroundMetal.h"
#include "GroundSea.h"

//====================================================================
MainGL::MainGL()
{
	game = Global::getInstance();
	initGL();
	loadTextures();
}


MainGL::~MainGL()
{
	deleteTextures();
}

//----------------------------------------------------------
int MainGL::initGL()
{
	Config *config = Config::instance();
	if( config->debug() ) fprintf(stderr, _("initGL()\n"));
	reshapeGL(config->screenW(), config->screenH());

	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
//	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(config->blend())
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
	}
	else
	{
		glAlphaFunc(GL_GREATER, 0.2);

		glDisable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
	}

//	glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	glDisable(GL_NORMALIZE);

	glPointSize(1.0);
	glLineWidth(1.0);
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

#ifdef IMAGE_GLPNG
	pngSetViewingGamma(config->viewGamma());
#endif

	return 0;
}

//----------------------------------------------------------
void MainGL::loadTextures()
{
	try {
#if defined(TEXT_GLC) && defined(TEXT_FTGL)
		Config *config = Config::instance();
		if(config->textType() == Config::TextGLC)
			game->text = new TextGLC();
		else
			game->text = new TextFTGL();
#elif defined(TEXT_GLC)
		game->text = new TextGLC();
#elif defined(TEXT_FTGL)
		game->text = new TextFTGL();
#else
		#error "TEXT_GLC or TEXT_FTGL must be defined"
#endif
	}
	catch (char* str)
	{
		fprintf(stderr, _("error loading font: %s\n"), str);
		exit(1);
	}
	catch (...)
	{
		fprintf(stderr, _("error loading font\n"));
		exit(1);
	}
}

//----------------------------------------------------------
void MainGL::deleteTextures()
{
	delete game->text;
	game->text = 0;
}

//----------------------------------------------------------
void MainGL::drawGL()
{
	switch(game->gameMode)
	{
		case Global::Game:
			drawGameGL();
			break;
		case Global::HeroDead:
			drawDeadGL();
			break;
		case Global::LevelOver:
			drawSuccessGL();
			break;
		case Global::Menu:
			game->menu->drawGL();
			break;
		default:
			fprintf(stderr, _("!!MainGL::drawGL() HUH?\n"));
			break;
	}
}

//----------------------------------------------------------
void MainGL::drawGameGL()
{
	Config *config = Config::instance();
	//-- Clear buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClear( GL_COLOR_BUFFER_BIT );

	//-- Place camera
	glLoadIdentity();
	glTranslatef(0.0, 0.0, config->zTrans());
//	glTranslatef(0.0, 5.0, -12.0);

	if(!game->game_pause)
	{
		//-- Add items to scene
		game->itemAdd->putScreenItems();
		//addItems();

		//-- Update scene
		game->enemyFleet->update();
		game->powerUps->update();
		game->heroAmmo->updateAmmo();
		game->enemyAmmo->updateAmmo();
		game->heroAmmo->checkForHits(game->enemyFleet);
		if(game->gameMode == Global::Game)
		{
			game->enemyAmmo->checkForHits(game->hero);
			game->hero->checkForCollisions(game->enemyFleet);
			game->hero->checkForPowerUps(game->powerUps);
		}
		game->explosions->update();
		game->audio->update();

		game->hero->update();
		game->gameFrame++;
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//-- Draw background
	game->ground->drawGL();

	//-- Draw actors
	game->enemyFleet->drawGL();
	game->hero->drawGL();

	if(config->gfxLevel() > 0)
		game->statusDisplay->darkenGL();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	game->powerUps->drawGL();

	//-- Draw ammo
	game->heroAmmo->drawGL();
	game->enemyAmmo->drawGL();

	//-- Draw explosions
	game->explosions->drawGL();

	//-- Draw stats
	game->statusDisplay->drawGL(game->hero);

}

//----------------------------------------------------------
void MainGL::drawDeadGL()
{
	Config *config = Config::instance();
	game->heroDeath--;

	//-- Clear buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//-- Place camera
	glLoadIdentity();
	if(game->heroDeath > 0)
	{
		float z = 1.0*game->heroDeath/DEATH_TIME;
		glTranslatef(0.0, 0.0, config->zTrans()-z*z);
	}
	else
		glTranslatef(0.0, 0.0, config->zTrans());

	//-- Add items to scene
	game->itemAdd->putScreenItems();
	//-- Update scene
	game->explosions->update();
	game->powerUps->update();
	game->enemyFleet->update();
	game->heroAmmo->updateAmmo();
	game->enemyAmmo->updateAmmo();
	game->heroAmmo->checkForHits(game->enemyFleet);
	game->audio->update();
	game->hero->update();
	game->gameFrame++;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//-- Draw background
	game->ground->drawGL();
	//-- Draw actors
	game->enemyFleet->drawGL();

	if(config->gfxLevel() > 0)
		game->statusDisplay->darkenGL();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	game->powerUps->drawGL();
	//-- Draw ammo
	game->heroAmmo->drawGL();
	game->enemyAmmo->drawGL();
	//-- Draw explosions
	game->explosions->drawGL();
	//-- Draw stats
	game->statusDisplay->drawGL(game->hero);

	int		skill = config->intSkill();
	float	heroScore = game->hero->getScore();
	HiScore *hiScore = HiScore::getInstance();
	char buffer[256];
	if(hiScore->check(skill, heroScore) == 1)
	{
		sprintf(buffer, _("new high score!\n\n%d"), (int)heroScore);
		drawTextGL(buffer, game->heroDeath, 0.15);
	}
	else if(hiScore->check(skill, heroScore) > 1)
	{
		sprintf(buffer, _("n o t   b a d !\nrank : %d\n\n%d"), hiScore->check(skill, heroScore), (int)heroScore);
		drawTextGL(buffer, game->heroDeath, 0.15);
	}
	else
	{
		drawTextGL(_("l o s e r"), game->heroDeath, 0.25);
	}
}

//----------------------------------------------------------
void MainGL::drawSuccessGL()
{
	Config *config = Config::instance();
	game->heroSuccess--;

	if(game->heroSuccess < -500)
	{
		game->gotoNextLevel();
		game->gameMode = Global::Game;
		game->audio->setMusicMode(Audio::MusicGame);
		game->audio->setMusicVolume(config->volMusic());
		return;
	}

	float f	= -game->heroSuccess/450.0;
	if(game->heroSuccess < 0)
	{
		float vol = config->volMusic() - (config->volMusic()*f);
		game->audio->setMusicVolume(vol);
	}

	//-- Clear buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//-- Place camera
	glLoadIdentity();
	glTranslatef(0.0, 0.0, config->zTrans());

	//-- Update scene
	game->enemyFleet->update();
	game->explosions->update();
	game->heroAmmo->updateAmmo();
	game->hero->update();
	game->audio->update();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//-- Draw background
	game->ground->drawGL();
	//-- Draw actors
	game->hero->drawGL();

	if(config->gfxLevel() > 0)
		game->statusDisplay->darkenGL();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//-- Draw ammo
	game->heroAmmo->drawGL();
	//-- Draw explosions
	game->explosions->drawGL();
	//-- Draw stats
	game->statusDisplay->drawGL(game->hero);

	char	buffer[512];
	sprintf(buffer, _("congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \n"), game->gameLevel);
//	if(game->hero->getScore() > game->hiScore[config->intSkill()][0])
//	{
//		sprintf(buffer, _("congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \n"), game->gameLevel);
//	}
//	else
//	{
//		sprintf(buffer, _("congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \nn e w   h i g h   s c o r e : \n %g \n"), game->gameLevel, game->hero->getScore());
//	}

	drawTextGL(buffer, game->heroSuccess, 0.15);
}

//----------------------------------------------------------
void MainGL::drawTextGL(const char *string, float pulse, float scale)
{
	int i, l, lines = 1;
	float	aa, ca;
	float	x_sin, y_sin, y, min_y;
	float	width, height;
	char	*walker,*newline;
	char	*index[10] = { 0,0,0,0,0,0,0,0,0,0 };
	char	buffer[128];

	//-- alpha
	float tmp = 0.5+0.5*(sin(pulse*0.02));
	aa = 0.7-0.5*tmp;
	if(pulse > -50.0)
		aa *= (-pulse/50.0);
	ca = 1.0-tmp;

	height = 1.5 * game->text->LineHeight();

	strncpy(buffer, string, 128);
	index[0] = buffer;
	walker   = buffer;
	while( (newline = strchr(walker, '\n')) != NULL )
	{
		walker = newline+1;
		index[lines] = newline+1;
		*newline = '\0';
		lines++;
	}

	min_y = 0.5*height*lines;
	for(l = 0; l< lines; l++)
	{
		y = min_y-height*(l+1);
		if(index[l] && strlen(index[l]) > 0)
		{
			for(i = 0; i < 6; i++)
			{
				glColor4f(1.0, ca*ca*0.3, ca*0.3, aa*aa);
				x_sin = 1.75*sin(i+game->frame*0.06);
				y_sin = 0.75*sin(i+game->frame*0.09);

				glPushMatrix();
				glScalef(scale, scale*0.75, 1.0);
				width = game->text->Advance(index[l]);
				glTranslatef(-(width/2.0)-x_sin, y+y_sin, 0.0);
				game->text->Render(index[l]);
				glPopMatrix();

			}
		}
	}
}

/**
 * incoming width and height are ignored - Config values are always used
 */
//----------------------------------------------------------
void MainGL::reshapeGL(int , int )
{
	Config *config = Config::instance();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( config->screenFOV(),
					config->screenA(),
					config->screenNear(),
					config->screenFar());
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, config->screenW(), config->screenH());
}
