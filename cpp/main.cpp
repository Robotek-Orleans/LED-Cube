#include "LEDCube.h"
#include <string>

int main(int argc, char** argv){
    
    std::string file = "test";
    if(argc >=2){
        file = argv[1];
    }
    LEDCube* cube = new LEDCube((std::string)"/var/www/html/LED-Cube/WEB/animations/" + file);
    cube->start();
    return 0;
}
