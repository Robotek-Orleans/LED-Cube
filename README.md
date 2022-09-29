# LED-Cube
LED-Cube is a 8x8x8 RGB LED-Cube.

The project is in different communicating parts:

- /WEB : enables to run and execute animations. 
    - select animation
    - preview animation
    - create and edit animations
    - connexion interface
- /cpp : executes and run animations
    - converts the animation in a format readable for LED Drivers (TLC5940)
    - runs the animation
    - runs the server

You can find PCBs and schematics in CAO and PCBs

# Installation
To intall the LED-Cube, you need RaspberryPi OS Lite:
https://www.raspberrypi.com/software/operating-systems/

*You can enable ssh at this step.
Also, connect the Raspberry Pi to your network.*

The project is available on Linux and Windows for tests.

You can install the VSCode extension CMake Tools to build the project.

A ledcube.env file is created where you start the server. You can use it to edit some configurations of the server.

### Linux

```sh
# 1) Install Make and CMake
sudo apt install git cmake make

# 2) Git clone
git clone https://github.com/Jiogo18/Led-Cube
cd Led-Cube
git submodule update --init --recursive

# 3) Install BCM2835 library (if you are on a Raspberry Pi), required to control the LEDs
cd cpp/include/bcm2835-1.70
sudo ./configure
sudo make
sudo make install
cd ../../..

# 4) Initialize the project (for a Raspberry Pi with BCM2835)
mkdir build
cd build
cmake ..
cd ..
# 4 bis) If you don't have a Raspberry Pi (or no BCM2835 library), you can use the following command instead of the previous one
# You won't be able to see the cube but you can still compile and run the server.
mkdir build
cd build
cmake -DNOBCM2835=1 ..
cd ..

# 5) Compile and run the project
# sudo is required to access the GPIO pins and to create the WebSocket
cmake --build ./build
sudo ./build/Led-Cube

# 6) Enjoy
```
### Adding the process as a service
```bash
sudo cp ledcube.service /etc/systemd/system/
sudo systemctl enable ledcube.service
# To start the service
sudo systemctl start ledcube.service
```


### Windows
(for tests)
```powershell
# 1) Install CMake and MinGW
https://cmake.org/download/
https://www.mingw-w64.org/downloads/ (via Cygwin, LLVM, Qt...)

# 2) Git clone
git clone https://github.com/Jiogo18/Led-Cube
cd Led-Cube
git submodule update --init --recursive

# 3) Initialize the project
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cd ..

# 4) Compile and run the project
cmake --build ./build
./build/Led-Cube

# 5) Enjoy
```

# Usage

## HCI Controls

| Function                         | Key                   |
| -------------------------------- | --------------------- |
| Select all LEDs between 2 points | <kbd>Shift</kbd>      |
| Select more than 1 LED           | <kbd>Control</kbd>    |
| Previous layer                   | <kbd>ArrowDown</kbd>  |
| Next Layer                       | <kbd>ArrowUp</kbd>    |
| Switch axis                      | <kbd>Tab</kbd>        |
| Previous frame                   | <kbd>ArrowLeft</kbd>  |
| Next frame                       | <kbd>ArrowRight</kbd> |

## HCI usage

All HCI is divided in 3 different pages:
 - animation/from_image
 - animation/edit
 - animation/open

from_image has for role to create a new animation by uploading a picture. It is composed of 3 parts: 
 - picture manager to upload your picture to the LED-Cube
 - 2D viewer to see the uploaded image
 - formula to change some parameters on the image (more information about formulas at https://github.com/Robotek-Orleans/LED-Cube/blob/main/WEB/math.md)

edit has for role to create from scratch or edit an existing animation. It is composed of 6 parts:
 - file manager window which the following elements:
    - a text input with the animation-name
    - a button to send the animation to execute.php to be saved and played to the LED-Cube
    - a button to send the animation to save.php to be saved
    - a reset button to clear all animation
 - frame manager window with the following elements:
    - the number of frames on the animation
    - a number input with the frame duration in milliseconds
    - a button to add a frame before the number written in the input box at the right 
    - a button to add a frame after the number written in the input box at the right
    - a slider to select a specific frame
    - 3 buttons to remove copy and paste a frame
 - viewer 3D to see the 3D led matrix and the selected 2D matrix.
 - 2D matrix choice with the following elements:
    - 3 radio buttons to select the side/axis of the plan
    - a slider to select the plan number
    - a button to remove the  highlight on the 3D viewer
 - 2D matrix to modify and view the colour of some LED's 
 - colour choice with the following elements:
    - 3 buttons to change colours on selected LED with a combination of red green and blue.
    - a colour picker to select a colour more precisely
    - 2 buttons to copy and paste a 2D matrix


## Direction

x : left-right

y : front-back

z : bottom-top

