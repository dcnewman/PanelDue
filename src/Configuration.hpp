/*
 * Configuration.hpp
 *
 * Created: 16/01/2015 13:18:16
 *  Author: David
 */ 


#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#define VERSION_TEXT		"1.09"

#define DISPLAY_TYPE_ITDB02_32WD		(0)		// Itead 3.2 inch widescreen display (400x240) - NO LONGER SUPPORTED
#define DISPLAY_TYPE_ITDB02_43			(1)		// Itead 4.3 inch display (480 x 272) or alternative 4.3 inch display with board on the right
#define DISPLAY_TYPE_INVERTED_43		(2)		// 4.3 inch display inverted, e.g. SainSmart with the controller board on the left
#define DISPLAY_TYPE_ITDB02_50			(3)		// Itead 5.0 inch display (800 x 480)

// Define DISPLAY_TYPE to be one of the above 3 types of display

#ifndef DISPLAY_TYPE
#define DISPLAY_TYPE	DISPLAY_TYPE_ITDB02_43
#endif

//#define DISPLAY_TYPE	DISPLAY_TYPE_INVERTED_43
//#define DISPLAY_TYPE	DISPLAY_TYPE_ITDB02_50

#define DEFAULT_BAUD_RATE	(57600)

#endif /* CONFIGURATION_H_ */
