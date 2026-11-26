# TASTE-egse-validation

This repository contains a TASTE model in `/model-satll-obc-comms-base`, to run this on an Ubuntu system, enter the project directory and run `taste`.

<p align="center">
  <img src="assets/img/model-interfaceview.png" width = 600>
</p>

The `space datalink` function will periodically send a dummy CCSDS PUS-C TM[3,25] packet over the UART interface of a SAMV71 Xplained evaluation board. 

The `can-ts` adapter function handles incoming CAN data and formats it to a CAN-TS frame for receiving and transmitting.

Documentation on how to use the model and the EGSE validation results can be found [here](https://github.com/KISPE-Space/mbep-cdr-quickstart/tree/master/documentation).

The referenced repo contains the EGSE software and supporting scripts. This model interfaces to the EGSE via CAN and UART. 

All of this software is available on Github: https://github.com/KISPE-Space/mbep-cdr-quickstart

<p align="center">
  <img src="assets/img/kispe-logo.png" width = 100>
</p>

This software has been developed as part of the "Model Based Execution Platform for Space Applications" project (contract ESA Contract No. 4000146882/24/NL/KK) financed by the European Space Agency.