#include "LEDCube.h"
#include <string>
#include <csignal>

LEDCube* cube; // pas bien

int main(int argc, char** argv){
    signal(SIGINT, signalHandler);
    std::string file = "test";
    if(argc >=2){
        file = argv[1];
    }
    cube = new LEDCube((std::string)"/var/www/html/LED-Cube/WEB/animations/" + file);
    cube->start();
    delete cube;
    return 0;
}

void signalHandler( int signum ) {
   cube->m_isRunning = false;
   //exit(signum);  
}
