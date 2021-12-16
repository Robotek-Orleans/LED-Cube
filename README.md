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

You can find PCBs and schematics in CAO and PCBs

# Installation
To intall the LED-Cube, you need RaspberryPi OS Lite:
https://www.raspberrypi.com/software/operating-systems/

*You can enable ssh at this step.
Also, connect the Raspberry Pi to your network.*

Then you need to install different packages:
```sh
sudo apt update
sudo apt upgrade
sudo apt install apache2 php git
```

add www-data user to sudo group:
```sh
sudo usermod -aG sudo www-data
```

Then allow sudo without password for all sudo group: 
```sh
sudo su
echo '%sudo ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers
exit
```

Restart your raspberry pi, then git clone the repo into /var/www/html:
```sh
cd /var/www/html
sudo git clone https://github.com/Robotek-Orleans/LED-Cube
```

Install bcm2835-1.70 driver:
```sh
cd LED-Cube/cpp/bcm2835-1.70
sudo ./configure
sudo make
sudo make install
```

Compile C++ code:
```sh
cd ../
sudo make
```

Change root directory of the web server
```sh
sudo sed -i 's+/var/www/html+/var/www/html/LED-Cube/WEB+g' /etc/apache2/sites-enabled/000-default.conf
```
Finally restart web server
```sh
sudo systemctl restart apache2
```


# Usage

## HCI Controls

| Function                          | Key         |
| --------------------------------- | ----------- |
| Select all LEDs between 2 points  | <kbd>Shift</kbd>       |
| Select more than 1 LED            | <kbd>Control</kbd>     |

## HCI usage

All HCI is divded in 3 different pages:
 - from_image.php
 - edit.php
 - openfile.php

from_image.php as for role to create a new animation by uploading a picture. It is composed of 3 parts: 
 - picture manager to upload your picture to the LED-Cube
 - 2D viewer to see the uploaded image
 - formula to change some parameters on the image (more information about formulas at https://github.com/Robotek-Orleans/LED-Cube/blob/main/WEB/math.md)

edit.php as for role to create from scratch or edit an existing animation. It is composed of 6 parts:
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

