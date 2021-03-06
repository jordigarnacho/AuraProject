#!/bin/bash

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do 
  TARGET="$(readlink "$SOURCE")"
  if [[ $TARGET == /* ]]; then
    SOURCE="$TARGET"
  else
    DIR="$( dirname "$SOURCE" )"
    SOURCE="$DIR/$TARGET"
  fi
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

path=`dirname $0`;
error=0;
green='\e[1;32m';
red='\e[0;31m';
white='\033[0m';
orange='\e[0;33m';

clear
echo "----------------------------------------------------------------"
echo "----------  SDK Parrot Installation by Jordi GARNACHO  ---------"
echo "----------------------------------------------------------------"

echo "----------------------------------------------------------------"
echo "---------------------  Packets Installation --------------------"
echo "----------------------------------------------------------------"

if [ $(dpkg-query -W -f='${Status}' wget 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "WGET INSTALLATION\n";
  sudo apt-get install wget -y;
  if [ $(dpkg-query -W -f='${Status}' wget 2>
ull | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}WGET INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}WGET HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}WGET HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' automake 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "AUTOMAKE INSTALLATION\n";
  sudo apt-get install automake -y;
  if [ $(dpkg-query -W -f='${Status}' automake 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}AUTOMAKE INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}AUTOMAKE HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}AUTOMAKE HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' autoconf 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "AUTOCONF INSTALLATION\n";
  sudo apt-get install autoconf -y;
  if [ $(dpkg-query -W -f='${Status}' autoconf 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}AUTOCONF INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}AUTOCONF HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}AUTOCONF HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' libtool 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "LIBTOOL INSTALLATION\n";
  sudo apt-get install libtool -y;
  if [ $(dpkg-query -W -f='${Status}' libtool 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}LIBTOOL INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}LIBTOOL HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}LIBTOOL HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' yasm 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "YASM INSTALLATION \n";
  sudo apt-get install yasm -y;
  if [ $(dpkg-query -W -f='${Status}' yasm 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}YASM INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}YASM HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}YASM HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' nasm 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "NASM INSTALLATION\n";
  sudo apt-get install nasm -y;
  if [ $(dpkg-query -W -f='${Status}' nasm 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}NASM INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}NASM HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}NASM HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' g++ 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "G++ INSTALLATION\n";
  sudo apt-get install g++ -y;
  if [ $(dpkg-query -W -f='${Status}' g++ 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}G++ INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}G++ HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}G++ HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' gcc 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "GCC INSTALLATION\n";
  sudo apt-get install gcc -y;
  if [ $(dpkg-query -W -f='${Status}' gcc 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}GCC INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}GCC HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}GCC HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' libswscale-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "${red}LIBSWSCALE-DEV INSTALLATION\n";
  sudo apt-get install libswscale-dev -y;
  if [ $(dpkg-query -W -f='${Status}' libswscale-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}LIBSWSCALE-DEV INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}LIBSWSCALE-DEV HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}LIBSWSCALE-DEV HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' libavformat-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "LIBAVFORMAT-DEV INSTALLATION\n";
  sudo apt-get install libavformat-dev -y;
  if [ $(dpkg-query -W -f='${Status}' libavformat-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}LIBAVFORMAT-DEV INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}LIBAVFORMAT-DEV HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}LIBAVFORMAT-DEV HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' libavcodec-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "LIBAVCODEC-DEV INSTALLATION\n";
  sudo apt-get install libavcodec-dev -y;
  if [ $(dpkg-query -W -f='${Status}' libavcodec-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}LIBAVCODEC-DEV INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}LIBAVCODEC-DEV HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}LIBAVCODEC-DEV HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' libncurses5-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "LIBNCURSES5-DEV INSTALLATION\n";
  sudo apt-get install ncurses-dev -y;
  if [ $(dpkg-query -W -f='${Status}' libncurses5-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e "${red}LIBNCURSES5-DEV INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}LIBNCURSES5-DEV HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}LIBNCURSES5-DEV HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' mplayer 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "MPLAYER INSTALLATION\n";
  sudo apt-get install mplayer -y;
  if [ $(dpkg-query -W -f='${Status}' mplayer 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e"${red}MPLAYER INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}MPLAYER HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}MPLAYER HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' libdbus-1-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "LIBDBUS-1-DEV INSTALLATION\n";
  sudo apt-get install libdbus-1-dev -y;
  if [ $(dpkg-query -W -f='${Status}' libdbus-1-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e"${red}LIBDBUS-1-DEV INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}LIBDBUS-1-DEV HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}LIBDBUS-1-DEV HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' libavahi-client-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "LIBAVAHI-CLIENT-DEV INSTALLATION\n";
  sudo apt-get install libavahi-client-dev -y;
  if [ $(dpkg-query -W -f='${Status}' libavahi-client-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e"${red}LIBAVAHI-CLIENT-DEV INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}LIBAVAHI-CLIENT-DEV HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}LIBAVAHI-CLIENT-DEV HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' libavahi-common-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "LIBAVAHI-COMMON-DEV INSTALLATION\n";
  sudo apt-get install libavahi-common-dev -y;
  if [ $(dpkg-query -W -f='${Status}' libavahi-common-dev 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e"${red}LIBAVAHI-COMMON-DEV INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}LIBAVAHI-COMMON-DEV HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}LIBAVAHI-COMMON-DEV HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

if [ $(dpkg-query -W -f='${Status}' git 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo -e "GIT INSTALLATION\n";
  sudo apt-get install git -y;
  if [ $(dpkg-query -W -f='${Status}' git 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
	echo -e"${red}GIT INSTALLATION HAS BEEN FAILED\n\n${white}";
	((error++));
  else
	echo -e "${green}GIT HAS BEEN INSTALLED\n\n${white}";
  fi
else
  echo -e "${green}GIT HAS ALREADY BEEN INSTALLED\n\n${white}";
fi

echo "----------------------------------------------------------------"
echo "---------------------  Repo Installation -----------------------"
echo "----------------------------------------------------------------"

cd ~/
if [ -d "bin" ];then
	echo -e "${orange}Bin REPERTORY ALREADY EXIST!${white}";
else
	mkdir ~/bin;
	if [ -d "bin" ];then
		echo -e "${green}Bin REPERTORY HAS BEEN CREATED${white}";
		chmod a+rwx ~/bin;
		PATH=~/bin:$PATH;
		curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
		chmod a+x ~/bin/repo
	else
		echo -e "${red}Bin REPERTORY CREATION FAILED${white}";
		((error++));
	fi
fi

echo "----------------------------------------------------------------"
echo "-----------------------  SDK Installation ----------------------"
echo "----------------------------------------------------------------"

if [ -d "SDK_drone" ];then
	echo -e "${orange}SDK_drone REPERTORY ALREADY EXIST!${white}";
else
	mkdir ~/SDK_drone;
	if [ -d "SDK_drone" ];then
		echo -e "${green}SDK_drone REPERTORY HAS BEEN CREATED${white}";
		cp $DIR/buildAndRun.sh ~/SDK_drone;
		cd ~/SDK_drone;
		git config --global user.name "Jordi GARNACHO";
		git config --global user.email "jordigarnacho@aol.com";
		repo init -u https://github.com/Parrot-Developers/arsdk_manifests.git;
		repo sync;
		./build.sh -p native -t build-sdk;
		chmod -R a+rwx ~/SDK_drone;
	else
		echo "${red}SDK_drone REPERTORY CREATION FAILED${white}";
		((error++));
	fi
fi


if [ "$error" -ne "0"  ]
then
	echo -e "${red}----------------------------------------------------------------"
	echo -e "----------  SDK INSTALLATION FAILED  with" $error" errors  -----------"
	echo -e "----------------------------------------------------------------${white}"
else
	echo -e "${green}----------------------------------------------------------------"
	echo -e "-----------------  SDK HAS BEEN INSTALLED  ---------------------"
	echo -e "----------------------------------------------------------------${white}"
fi
echo "----------------------------------------------------------------"
echo "------------  Contact: jordigarnacho@aol.com   -----------------"
echo "----------------------------------------------------------------"



