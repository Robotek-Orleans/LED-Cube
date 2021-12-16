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

# installation
To intall the LEDCube, you need RaspberryPi OS Lite:
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