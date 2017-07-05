#!/bin/bash

DEST=./garden-wifi-pi
TEMP=./garden-wifi-pi-temp

sudo rm ${DEST}
sudo rm ${TEMP}

g++ -o ${TEMP} -lrf24 ./RF_receive.cpp 

sudo chmod 755 ${TEMP}
sudo chown root ${TEMP} 
sudo chmod u+s ${TEMP}
sudo mv ${TEMP} ${DEST}
