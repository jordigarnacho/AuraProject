<?php
	
	$NorthEastLat = $_GET['NorthEastLat'];
	$NorthEastLng = $_GET['NorthEastLng'];
	$SouthWestLat = $_GET['SouthWestLat'];
	$SouthWestLng = $_GET['SouthWestLng'];
	$Elevation = $_GET['Elevation'];
	$HeightQuadcopter = $_GET['HeightQuadcopter'];
	
	$NorthWestLat = $_GET['NorthEastLat'];
	$NorthWestLng = $_GET['SouthWestLng'];
	$SouthEastLat = $_GET['SouthWestLat'];
	$SouthEastLng = $_GET['NorthEastLng'];

	$cameraLarger = 0.001;
	$coeff = 1;
	
	/* Km */

	$rectangleNorthFace = acos(sin(deg2rad($NorthEastLat))*sin(deg2rad($NorthWestLat))+cos(deg2rad($NorthEastLat))*cos(deg2rad($NorthWestLat))*cos(deg2rad		($NorthEastLng-$NorthWestLng)))*6371;
	$rectangleEastFace = acos(sin(deg2rad($NorthEastLat))*sin(deg2rad($SouthEastLat))+cos(deg2rad($NorthEastLat))*cos(deg2rad($SouthEastLat))*cos(deg2rad		($NorthEastLng-$SouthEastLng)))*6371;
	$rectangleSouthFace = acos(sin(deg2rad($SouthWestLat))*sin(deg2rad($SouthEastLat))+cos(deg2rad($SouthWestLat))*cos(deg2rad($SouthEastLat))*cos(deg2rad		($SouthWestLng-$SouthEastLng)))*6371;
	$rectangleWestFace = acos(sin(deg2rad($SouthWestLat))*sin(deg2rad($NorthWestLat))+cos(deg2rad($SouthWestLat))*cos(deg2rad($NorthWestLat))*cos(deg2rad		($SouthWestLng-$NorthWestLng)))*6371;
	echo 'North = '.$rectangleNorthFace.'<br/>';
	echo 'South = '.$rectangleSouthFace.'<br/>';
	echo 'East = '.$rectangleEastFace.'<br/>';
	echo 'West = '.$rectangleWestFace.'<br/>';

	if ((($rectangleNorthFace + $rectangleSouthFace)/2.0) >= (($rectangleEastFace + $rectangleWestFace)/2.0))
	{
		$division = (($rectangleNorthFace + $rectangleSouthFace)/(2.0*$coeff*$HeightQuadcopter*$cameraLarger));
	}
	else
	{
		$division = (($rectangleEastFace + $rectangleWestFace)/(2.0*$coeff*$HeightQuadcopter*$cameraLarger));
	}

	echo 'Division = '.$division;

	


	/*
	$fileName = md5(microtime(TRUE)*100000);
	$type = "application/csv";
	$myFile = fopen('../navigationMaps/'.$fileName.'.csv', 'w');
    
	for ($i = 0 ; isset($_GET['lat'.$i]) or isset($_GET['lng' . $i]); $i++)
    {
		fwrite ($myFile, $_GET['lat'.$i]); // On y met les données
		fwrite ($myFile, ',');
		fwrite ($myFile, $_GET['lng'.$i]); // On y met les données
		fputs($myFile, "\n"); 
    }
	
	fclose($myFile);
	
	// Création des headers, pour indiquer au navigateur qu'il s'agit d'un fichier à télécharger
	header('Content-Transfer-Encoding: binary'); //Transfert en binaire (fichier)
	header('Content-Disposition: attachment; filename='.$fileName.'.csv'); //Nom du fichier
	header('Content-Length: '.filesize('../navigationMaps/'.$fileName.'.csv')); //Taille du fichier
	header("Cache-Control: must-revalidate, post-check=0, pre-check=0, public");
	header("Content-Transfer-Encoding: $type\n"); // Surtout ne pas enlever le \n
	header("Pragma: no-cache");
	header("Expires: 0");
	
	//Envoi du fichier dont le chemin est passé en paramètre
	readfile('../navigationMaps/'.$fileName.'.csv');*/

?>
