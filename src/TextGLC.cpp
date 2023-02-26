/*
 * Copyright 2009 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#ifdef TEXT_GLC

#include "gettext.h"

#include "TextGLC.h"

#include <cstdlib>

using namespace std;

//====================================================================
TextGLC::TextGLC() : Text()
{
	// Setup a GLC context and use it
	context = glcGenContext();
	glcContext(context);
	if( GLC_NONE != glcGetError() )
		throw _("GLC: couldn't set context");

	// Get default font
	defaultFont = glcGenFontID();
	/*
	TRANSLATORS: If using the GLC backend, this should be the font
	name of a font that contains all the Unicode characters in use in
	your translation. The GLC renderer will fall back on other fonts
	if some characters are not available though, so choose a font
	that your language looks good in.
	*/
	glcNewFontFromFamily(defaultFont, _("Gothic Uralic"));
	if( GLC_NONE == glcGetError() ) {
		glcFontFace(defaultFont, "Bold");
		glcFont(defaultFont);
	}

#ifdef FONT_NAME
	// Get distributor-specified font
	distroFont = glcGenFontID();
	glcNewFontFromFamily(distroFont, FONT_NAME);
	if( GLC_NONE == glcGetError() )	glcFont(distroFont);
#else
	distroFont = 0;
#endif

	// Get user-specified font
	if( NULL != getenv("CHROMIUM_BSU_FONT") ){
		userFont = glcGenFontID();
		glcNewFontFromFamily(userFont, getenv("CHROMIUM_BSU_FONT"));
		if( GLC_NONE == glcGetError() ) glcFont(userFont);
	} else {
		userFont = 0;
	}

	// Use the texture-based font renderer
	glcRenderStyle(GLC_TEXTURE);

	// Use UTF-8 encoded strings
	glcStringType(GLC_UTF8_QSO);
}

TextGLC::~TextGLC()
{
	glcFont(0);
	if(defaultFont){ glcDeleteFont(defaultFont); defaultFont = 0; }
	if(distroFont){ glcDeleteFont(distroFont); distroFont = 0; }
	if(userFont){ glcDeleteFont(userFont); userFont = 0; }

	glcContext(0);
	if(context){ glcDeleteContext(context); context = 0; }
}

void TextGLC::Render(const char* str, const int len)
{
	glPushMatrix();
	glScalef(24.f, 24.f, 1.f);

	if( len == -1 )
		glcRenderString(str);
	else
		glcRenderCountedString(len, str);

	glPopMatrix();
}

float TextGLC::Advance(const char* str, const int len)
{
	GLfloat advance[4] = {0.f, 0.f, 0.f, 0.f};

	if( len == -1 )
		glcMeasureString(GL_FALSE, str);
	else
		glcMeasureCountedString(GL_FALSE, len, str);

	glcGetStringMetric(GLC_BASELINE, advance);

	return (advance[2] - advance[0]) * 24.f;
}

float TextGLC::LineHeight(const char* str, const int len)
{
	GLfloat bbox[8] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

	if( len == -1 )
		glcGetMaxCharMetric(GLC_BOUNDS, bbox);
	else {
		glcMeasureCountedString(GL_FALSE, len, str);
		glcGetStringMetric(GLC_BOUNDS, bbox);
	}

	return (bbox[7] - bbox[1]) * 24.f;
}

#endif // TEXT_GLC
