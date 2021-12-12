#include "LEDCube.h"
#include <string>
#include <csignal>

#ifdef DEBUG
#include <iostream>
#endif

LEDCube* cube; // pas bien

void signalHandler( int signum ) {
   #ifdef DEBUG
    std::cout << "Sigterm signal triggered" <<std::endl;
   #endif
   cube->m_isRunning = false;
   #ifdef DEBUG
    std::cout << "Sigterm signal ended" <<std::endl;
   #endif
   //exit(signum);  
}

int main(int argc, char** argv){
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    std::string file = "growCube";
    if(argc >=2){
        file = argv[1];
    }
    cube = new LEDCube((std::string)"/var/www/html/LED-Cube/WEB/animations/" + file);
    cube->start();
    delete cube;
    return 0;
}


