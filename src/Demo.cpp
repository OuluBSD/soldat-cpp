#include "../include/Demo.h"

// Define global variables that are declared as extern in Demo.h
std::unique_ptr<TDemoRecorder> DemoRecorder;
#ifndef SERVER_CODE
std::unique_ptr<TDemoPlayer> DemoPlayer;
#endif
uint16_t RSize;
uint8_t FreeCam;
uint8_t NoTexts;