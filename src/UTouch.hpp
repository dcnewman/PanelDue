/*
  UTouch.h - Arduino/chipKit library support for Color TFT LCD Touch screens 
  Copyright (C)2010-2014 Henning Karlsen. All right reserved
  
  Basic functionality of this library are based on the demo-code provided by
  ITead studio. You can find the latest version of the library at
  http://www.henningkarlsen.com/electronics

  If you make any modifications or improvements to the code, I would appreciate
  that you share the code with me so that I might include it in the next release.
  I can be contacted through http://www.henningkarlsen.com/electronics/contact.php

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
*/

#ifndef UTouch_h
#define UTouch_h

#define UTOUCH_VERSION	124

#include "asf.h"
#include "OneBitPort.hpp"
#include "DisplayOrientation.hpp"

enum TouchPrecision { TpLow, TpMedium, TpHigh, TpExtreme };

class UTouch
{
public:
	int16_t	TP_X ,TP_Y;

	UTouch(unsigned int tclk, unsigned int tcs, unsigned int tdin, unsigned int dout, unsigned int irq);

	void	InitTouch(uint16_t xp, uint16_t yp, DisplayOrientation orientation = Landscape, TouchPrecision p = TpMedium);
	void	read();
	bool	dataAvailable() const;
	int16_t	getX() const;
	int16_t	getY() const;
	void	setPrecision(TouchPrecision precision);

	void	calibrateRead();
    
private:
	OneBitPort portCLK, portCS, portDIN, portDOUT, portIRQ;
	DisplayOrientation orient, nativeOrientation;
	uint8_t	prec;
//	uint8_t	display_model;
	uint16_t disp_x_size, disp_y_size;
	int16_t	touch_x_left, touch_x_right, touch_y_top, touch_y_bottom;

	void	touch_WriteData(uint8_t data);
	uint16_t	touch_ReadData();
};

#endif