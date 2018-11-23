//----------------------------------------
// Copyright 2015 Corey Toler-Franklin, 
// University of Florida
// utility functions
// utilities.cpp
//----------------------------------------



#include "utilities.h"
#include <limits>
#include <fstream>
#include <sstream>
#include <vector>

//------------------------------------
// conversions
//------------------------------------

// converts a unsigned char to a float
float UcharToFloat(unsigned char c) 
{
    return(c/255.0f);
}


// converts a float to unsigned char 
unsigned char floatToUchar (float f) 
{
    return((unsigned char)(f * 255));
}


//-----------------------------------------
// File Loader: by Keith Lantz
//-----------------------------------------
char* loadFile(const char *filename) {
    char* data;
    int len;
    std::ifstream ifs(filename, std::ifstream::in);

    ifs.seekg(0, std::ios::end);
    len = ifs.tellg();

    ifs.seekg(0, std::ios::beg);
    data = new char[len + 1];

    ifs.read(data, len);
    data[len] = 0;

    ifs.close();

    return data;
}

std::string pad(int x) {
    std::stringstream s;
    if (x < 10)   s << "00" << x;
    else if (x < 100)  s << "0" << x;
    else if (x < 1000) s << "" << x;
    return s.str();
}

std::string number_format(int x) {
    std::stringstream s;
    std::vector<int> sep; int t; bool flag = false;
    if (x < 0) { flag = true; x = -x; }
    while (x > 1000) {
        t = x % 1000;
        sep.push_back(t);
        x = x / 1000;
    };
    sep.push_back(x);
    for (int i = sep.size() - 1; i > 0; i--) {
        if (i == sep.size() - 1) s << (flag ? "-" : "") << sep[i] << ",";
        else s << pad(sep[i]) << ",";
    }
    if (sep.size() > 1)
        s << pad(sep[0]);
    else
        s << (flag ? "-" : "") << sep[0];
    return s.str();
}
