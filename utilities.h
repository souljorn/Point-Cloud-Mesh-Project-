//----------------------------------------
// Copyright 2015 Corey Toler-Franklin, 
// University of Florida
// utility functions
// utilities.h
//----------------------------------------



#ifndef __UTILITIES_H__
#define __UTILITIES_H__


#include <string>

#define RENORMCOUNT 97
const double EPSILON = 1.0e-6; 



// conversions
float               UcharToFloat                (unsigned char c);
unsigned char       floatToUchar                (float f);


char				*loadFile					(const char *filename);
std::string			pad							(int x);
std::string			number_format				(int x);

#endif //__UTILITIES_H__

