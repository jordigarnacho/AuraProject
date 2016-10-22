This script will automatically install version 3.8 SDK
Parrot.

This SDK allows you to control all new drone Parrot
(2 Bebop, Bebop, and JumpingSumo EVOS, Spider Roll, Airbornes)
a remote station (maybe an Android or iOS phone or even
a Linux-based controller).

The SDK requires the installation of several packages that will be
installed when launching this script
(If packages are not already installed on your machine).

packages list:
- wget
- automake
- autoconf
- libtool
- yasm
- nasm
- G ++
- gcc
- Libswscale-dev
- Libavformat-dev
- Libavcodec-dev
- Libncurses5-dev
- mplayer
- Libdbus-1-dev
- Libavahi-client-dev
- Libavahi-common-dev
- git

In addition, the script will create 2 files in the root of your home directory:
- Bin: folder containing the files necessary to install the SDK
- SDK_drone: folder containing the SDK

Once the SDK is installed, you can run the examples present in
the directory using the buildAndRun.sh file. To use it simply
to call it as a parameter with the name of the instance.

command: bash buildAndRun.sh sample_name

Examples base fournient are:
- BebopDroneDecodeStream
- BebopDroneReceiveStream
- BebopPilotingNewAPI
- JumpingSumoReceiveStream
- JumpingSumoPilotingNewAPI
- JumpingSumoPiloting
- JumpingSumoChangePosture
