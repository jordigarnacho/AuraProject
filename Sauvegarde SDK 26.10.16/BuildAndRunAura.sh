#!/bin/bash
./build.sh -p Unix-base -t build-sdk
source out/Unix-base/staging/native-wrapper.sh
out/Unix-base/staging/usr/bin/AuraProject
chmod a+rwx video_fifo_AuraProject.h264



#Transfert + suppression mais sans les %

#cd Images
#ftp -n 192.168.42.1 21 <<EOD
#cd internal_000/Bebop_Drone/media
#binary
#prompt
#mget -a

#ls
#mdelete -a
#ls
#EOD

#A utiliser pour la demo car transfert avec touche w donc %
#ftp -n 192.168.42.1 21 <<EOD
#cd internal_000/Bebop_Drone/media
#prompt
#ls
#mdelete -a
#ls
#EOD
#cd ../
#chmod -R a+rwx SDK_Drone*/


echo "Vider le drone (Y/N) ?"
valide=0
read
while [ "$valide" -ne "1" ]; do
	if [ $REPLY = Y ]
	then
		ftp -n 192.168.42.1 <<-EOF
		cd internal_000/Bebop_Drone/media
		prompt
		mdelete -a
		bye
		EOF
		echo "Drone vidé"
		valide=1
	elif [ $REPLY = N ]
	then
		echo "Drone NON vidé"
		valide=1
	else
		echo "choisir entre Y ou N"
		valide=0
		read
	fi
done
cd ../
chmod -R a+rwx SDK_drone
