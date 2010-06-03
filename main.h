#ifndef __SP_MAIN_H_INCLUDED__
#define __SP_MAIN_H_INCLUDED__

using namespace std;

#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <cstdio>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include <pthread.h>
#include <jack/jack.h>

#include <sys/types.h>
#include <unistd.h>

/*
#include "log4cxx/logger.h"
#include "log4cxx/basicconfiguration.h"
#include "log4cxx/basicconfiguration.h"
*/

#define SRC_WAV 0
#define SRC_JACK_ONE 1
#define SRC_JACK_AUTO 2
#define ACTION_DUMP 0
#define ACTION_CATCH 1

#define BUFFERSIZE 1

#endif //__SP_MAIN_H_INCLUDED__
