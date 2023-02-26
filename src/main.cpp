/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "gettext.h"

#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>

#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#endif // _WIN32

#include <sys/stat.h>

#ifdef ENABLE_NLS
	#include <locale.h>
	#include <libintl.h>
#endif

#include "main.h"
#include "define.h"
#include "MainSDL.h"
#include "MainGLUT.h"

#include "Config.h"
#include "extern.h"
#include "Global.h"
#include "HiScore.h"

#include "EnemyAircraft.h"

//----------------------------------------------------------
int main(int argc, char **argv)
{
	int i;
	int	tmp;
	int	vm = 2;

#ifdef ENABLE_NLS
	setlocale(LC_ALL, "");
        bindtextdomain(PACKAGE, LOCALEDIR);
        bind_textdomain_codeset(PACKAGE, "UTF-8");
        textdomain(PACKAGE);
#endif

#ifdef __linux__
	// by default, disable SDL's use of DGA mouse. If SDL_VIDEO_X11_DGAMOUSE is
	// set however, use default value.
	setenv("SDL_VIDEO_X11_DGAMOUSE", "0", false);
#endif

	Config	*config	= Config::init();
	Global	*game	= Global::init();
	HiScore	*hiScore = HiScore::init();

	for ( i=1; i < argc; i++ )
	{
		if ( strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fullscreen") == 0 )
		{
			config->setFullScreen(true);
		}
		else if ( strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--window") == 0 )
		{
			config->setFullScreen(false);
		}
		else if ( strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--vidmode") == 0 )
		{
			if((i+1) < argc)
			{
				sscanf(argv[i+1], "%d", &tmp);
				if(tmp <= MAX_SCREEN_SIZE && tmp >= 0)
				{
					vm = tmp;
					i++;
				}
			}
			config->setScreenSize(vm);
		}
		else if( strcmp(argv[i], "-na") == 0 || strcmp(argv[i], "--noaudio") == 0)
		{
			config->setAudio(false);
		}
		else if( strcmp(argv[i], "-nb") == 0 || strcmp(argv[i], "--noblend") == 0)
		{
			config->setBlend(false);
		}
		else if( strcmp(argv[i], "-nt") == 0 || strcmp(argv[i], "--notexborder") == 0)
		{
			config->setTexBorder(false);
		}
		else if( strcmp(argv[i], "--debug") == 0)
		{
			config->setDebug(true);
		}
		else if( strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0)
		{
			printf("%s\n", PACKAGE_STRING);
			exit(0);
		}
		else
		{
			fprintf(stderr, _(
				"\n"
				"--------------------------------------------------\n"
				"Chromium B.S.U. options\n"
				"--------------------------------------------------\n"
				"   -f/--fullscreen     : run in fullscreen mode\n"
				"   -w/--window         : run in windowed mode\n"
				"   -v/--vidmode <mode> : mode 0 =  512 x  384\n"
				"                       :      1 =  640 x  480\n"
				"                       :      2 =  800 x  600\n"
				"                       :      3 = 1024 x  768\n"
				"                       :      4 = 1280 x  960\n"
				"  -na/--noaudio        : do not initialize audio\n"
				"  -nb/--noblend        : disable blending (OpenGL)\n"
				"  -nt/--notexborder    : do not set tex border color (OpenGL)\n"
				"  -V/--version         : show version information\n"
				"--------------------------------------------------\n\n"));
			exit(0);
		}
	}

	if( config->debug() ) atexit(EnemyAircraft::printNumAllocated);

	srand(time(NULL));

	game->generateRandom();

#ifdef USE_SDL
	game->toolkit = new MainSDL(argc, argv);
#elif USE_GLUT
	game->toolkit = new MainGLUT(argc, argv);
#else
#error "USE_SDL or USE_GLUT must be defined"
#endif

	if( config->debug() ) hiScore->print(config->intSkill());
	game->toolkit->run();

	hiScore->destroy();
	config->destroy();
	game->destroy();

	return 0;
}

// Changes '/' to ':' for Mac paths, is a no-op for other platforms.
//----------------------------------------------------------
const char* alterPathForPlatform(char* filename)
{
#ifdef macintosh
	char    *walker = filename;
	while( *walker )
	{
		if(*walker == '/' )
			*walker =  ':';
		walker++;
	}
#endif
#ifdef _WIN32
	char    *walker = filename;
	while( *walker )
	{
		if(*walker == '/' )
			*walker =  '\\';
		walker++;
	}
#endif
	return filename;
}

//----------------------------------------------------------
const char* dataLoc(const char* filename, bool doCheck)
{
	static char buffer[256];
	struct	stat sbuf;

	if(getenv("CHROMIUM_BSU_DATA") != NULL && ((strlen(getenv("CHROMIUM_BSU_DATA"))+strlen(filename)) < 254) )
	{
		sprintf(buffer, "%s/%s", getenv("CHROMIUM_BSU_DATA"), filename);
		if(stat(buffer, &sbuf) == 0) return buffer;
	}

	if(getenv("HOME") != NULL && ((strlen(getenv("HOME"))+strlen(filename)) < 239) )
	{
		sprintf(buffer, "%s/." PACKAGE "-data/%s", getenv("HOME"), filename);
		if(stat(buffer, &sbuf) == 0) return buffer;
	}

#ifdef PKGDATADIR
	if( ((strlen(PKGDATADIR)+strlen(filename)) < 254) )
	{
		sprintf(buffer, "%s/%s", PKGDATADIR, filename);
		if(stat(buffer, &sbuf) == 0) return buffer;
	}
#endif

#ifdef macintosh
	#define DATADIR "::data"
#else
	#define DATADIR "../data"
#endif

	if( (strlen(DATADIR)+strlen(filename)) < 254)
	{
		sprintf(buffer, "%s/%s", DATADIR, filename);
	}

	alterPathForPlatform(buffer);

#ifndef _WIN32 // WIN32 users don't get error checks...
	if(doCheck)
	{
		if(stat(buffer, &sbuf) == -1)
		{
			fprintf(stderr, _("!! ERROR !! - "));
			perror(buffer);
		}
	}
#endif

#ifdef CHECK_ERRORS
//	fprintf(stderr, "dataLoc(\"%s\")\n", buffer);
#endif
	return buffer;
}

//----------------------------------------------------------
void printExtensions(FILE *fstream, const char* extstr_in)
{
	char	*extstr;
	char	*walker;
	char	*space;
	int		len = strlen(extstr_in);
	int		cnt = 0;
	extstr = new char[len+1];
	strcpy(extstr, extstr_in);
	walker = extstr;

	fprintf(fstream, _("Extensions :"));
	while( (walker-extstr) < len )
	{
		space = strchr(walker, ' ');
		if(space)
		{
			if( (cnt%2) == 0)
				fprintf(fstream, "\n");
			*space = '\0';
			fprintf(fstream, "%-31s ", walker);
			walker = space+1;
			cnt++;
		}
		else
			break;
	}
	fprintf(fstream, "\n");

	delete [] extstr;
}
