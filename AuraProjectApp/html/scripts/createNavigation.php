<?php
	/*--------------------------------------
	----------------------------------------
	-- AURA PROJECT APP - CreateNavigation -
	----------------------------------------
	--------------------------------------*/
	
	/* GET Values Acquisition */
	$NorthEastLat = $_GET['NorthEastLat'];
	$NorthEastLng = $_GET['NorthEastLng'];
	$SouthWestLat = $_GET['SouthWestLat'];
	$SouthWestLng = $_GET['SouthWestLng'];
	$Elevation = $_GET['Elevation'];
	$HeightQuadcopter = $_GET['HeightQuadcopter'];
	$StartLat = $_GET['StartLat'];
	$StartLng = $_GET['StartLng'];

	/* Others Data Deduction */
	$NorthWestLat = $_GET['NorthEastLat'];
	$NorthWestLng = $_GET['SouthWestLng'];
	$SouthEastLat = $_GET['SouthWestLat'];
	$SouthEastLng = $_GET['NorthEastLng'];

	/* Camera Parameters */
	$cameraLarger = 0.001;
	$cameraCoeff = 1;

	/* TEST PRINTS */
	echo 'NorthEast= ('.$NorthEastLat.','.$NorthEastLng.')<br/>';
	echo 'NorthWest= ('.$NorthWestLat.','.$NorthWestLng.')<br/>';
	echo 'SouthEast= ('.$SouthEastLat.','.$SouthEastLng.')<br/>';
	echo 'SouthWest= ('.$SouthWestLat.','.$SouthWestLng.')<br/>';
	
	/* Distance Calculation in Kilometers */	
	$rectangleNorthFace = acos(sin(deg2rad($NorthEastLat))*sin(deg2rad($NorthWestLat))+cos(deg2rad($NorthEastLat))*cos(deg2rad($NorthWestLat))*cos(deg2rad		($NorthEastLng-$NorthWestLng)))*6371;
	$rectangleEastFace = acos(sin(deg2rad($NorthEastLat))*sin(deg2rad($SouthEastLat))+cos(deg2rad($NorthEastLat))*cos(deg2rad($SouthEastLat))*cos(deg2rad		($NorthEastLng-$SouthEastLng)))*6371;
	$rectangleSouthFace = acos(sin(deg2rad($SouthWestLat))*sin(deg2rad($SouthEastLat))+cos(deg2rad($SouthWestLat))*cos(deg2rad($SouthEastLat))*cos(deg2rad		($SouthWestLng-$SouthEastLng)))*6371;
	$rectangleWestFace = acos(sin(deg2rad($SouthWestLat))*sin(deg2rad($NorthWestLat))+cos(deg2rad($SouthWestLat))*cos(deg2rad($NorthWestLat))*cos(deg2rad		($SouthWestLng-$NorthWestLng)))*6371;
	
	/* TEST PRINTS */
	echo 'North = '.$rectangleNorthFace.'<br/>';
	echo 'South = '.$rectangleSouthFace.'<br/>';
	echo 'East = '.$rectangleEastFace.'<br/>';
	echo 'West = '.$rectangleWestFace.'<br/>';

	/* Find the biggest face and calculate the number of divisions */
	if ((($rectangleNorthFace + $rectangleSouthFace)/2.0) >= (($rectangleEastFace + $rectangleWestFace)/2.0))
	{
		$division = (($rectangleNorthFace + $rectangleSouthFace)/(2.0*$cameraCoeff*$HeightQuadcopter*$cameraLarger));
		$biggestFace = "NorthSouth";
	}
	else
	{
		$division = (($rectangleEastFace + $rectangleWestFace)/(2.0*$cameraCoeff*$HeightQuadcopter*$cameraLarger)); 
		$biggestFace = "EastWest";
	}

	/* TEST PRINTS */
	echo 'Division = '.$division.'<br/>';

	/* Distance Calculation in Kilometers to find the First Location */
	$startToNorthEast = acos(sin(deg2rad($NorthEastLat))*sin(deg2rad($StartLat))+cos(deg2rad($NorthEastLat))*cos(deg2rad($StartLat))*cos(deg2rad($NorthEastLng-$StartLng)))*6371;

	$startToNorthWest = acos(sin(deg2rad($NorthWestLat))*sin(deg2rad($StartLat))+cos(deg2rad($NorthWestLat))*cos(deg2rad($StartLat))*cos(deg2rad($NorthWestLng-$StartLng)))*6371;

	$startToSouthEast = acos(sin(deg2rad($SouthEastLat))*sin(deg2rad($StartLat))+cos(deg2rad($SouthEastLat))*cos(deg2rad($StartLat))*cos(deg2rad($SouthEastLng-$StartLng)))*6371;

	$startToSouthWest = acos(sin(deg2rad($SouthWestLat))*sin(deg2rad($StartLat))+cos(deg2rad($SouthWestLat))*cos(deg2rad($StartLat))*cos(deg2rad($SouthWestLng-$StartLng)))*6371;

	/* Find the nearest location of the start location */
	if($startToNorthEast <= $startToNorthWest & $startToNorthEast <= $startToSouthEast & $startToNorthEast <= $startToSouthWest)
	{
		echo ("NorthEast");
		$startLocation = "NorthEast";
	}
	else
	{
		if($startToNorthWest <= $startToNorthEast & $startToNorthWest <= $startToSouthEast & $startToNorthWest <= $startToSouthWest)
		{
			echo ("NorthWest");
			$startLocation = "NorthWest";
		}
		else
		{
			if($startToSouthEast <= $startToNorthEast & $startToSouthEast <= $startToNorthWest & $startToSouthEast <= $startToSouthWest)
			{
				echo ("SouthEast");
				$startLocation = "SouthEast";
			}
			else 
			{
				echo ("SouthWest");
				$startLocation = "SouthWest";
			}
		}
	}


	/* Path creation according to the startPosition */

	switch ($startLocation) {

		/* North East Case */
		case "NorthEast":{

			/* First GPS position initialisation */
			$GPSLatPath[0] = $NorthEastLat;
			$GPSLngPath[0] = $NorthEastLng;
			
			/* North/South is the biggest face */
			if $biggestFace = "NorthSouth"
			{
				/*
				for ($i = 0; $i < ($rectangleNorthFace + $rectangleSouthFace)/2.0); $i = $i + $division)
				{

				}
				*/
			}

			else
			{
				
			}
	 	}
        	break;
    		
		/* North West Case */
		case "NorthWest":
			//code to be executed if n=label2;
		break;
	    
		/* South East Case */
		case "SouththEast":
			//code to be executed if n=label3;
		break;
	   
		/* South West Case */
		default:
			//code to be executed if n is different from all labels;
	} 




/*

	$fileName = md5(microtime(TRUE)*100000);
	$type = "application/csv";
	$myFile = fopen('../navigationMaps/'.$fileName.'.csv', 'w');
    
	for ($i = 1 ; $i<=$division ; $i++)
    	{
		$LocationLat = ;
		$LocationLng = ;
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
	readfile('../navigationMaps/'.$fileName.'.csv');
*/

?>
