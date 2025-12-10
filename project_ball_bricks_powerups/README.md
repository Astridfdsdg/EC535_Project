Intructions on how to run game:

#qmake creates the .pro file called paddle_ball_bricks_powerups.pro , please look at that .pro file and dont change it since it must be as it is for this game to work
qmake -project  

#qmake uses the .pro file to generate the Makefile
qmake

#Uses the Makefile to compile the project and creates the binary
make -j4

#Transfer binary , here paddle_ball_bricks_powerups to the BeagleBone
#In BeagleBone after transfering:

chmod +x paddle_ball_bricks_powerups  
QT_QPA_PLATFORM=linuxfb QT_QPA_FB_HIDECURSOR=1 ./paddle_ball_bricks_powerups
