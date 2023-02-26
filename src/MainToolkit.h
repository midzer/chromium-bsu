/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */
#ifndef MainToolkit_h
#define MainToolkit_h

//====================================================================
class MainToolkit
{
public:
	MainToolkit(int argc = 0, char **argv = 0);
	virtual ~MainToolkit();

	enum Key { KeyEnter, KeySpace, KeyTab, KeyEscape,
				KeyUp, KeyDown, KeyLeft, KeyRight };

	enum Button { Left, Middle, Right };

	virtual bool run() = 0;
	virtual bool checkErrors() = 0;

	virtual bool setVideoMode() = 0;

	virtual void grabMouse(bool,bool=true) = 0;
};




#endif // MainToolkit_h



