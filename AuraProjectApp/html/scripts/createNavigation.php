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

	/* GPS Location Interval Calculation*/
	$GPSInterval = 0.000001;

	/* TEST PRINTS */
	//echo 'NorthEast= ('.$NorthEastLat.','.$NorthEastLng.')<br/>';
	//echo 'NorthWest= ('.$NorthWestLat.','.$NorthWestLng.')<br/>';
	//echo 'SouthEast= ('.$SouthEastLat.','.$SouthEastLng.')<br/>';
	//echo 'SouthWest= ('.$SouthWestLat.','.$SouthWestLng.')<br/>';
	
	/* Distance Calculation in Kilometers */	
	$rectangleNorthFace = acos(sin(deg2rad($NorthEastLat))*sin(deg2rad($NorthWestLat))+cos(deg2rad($NorthEastLat))*cos(deg2rad($NorthWestLat))*cos(deg2rad		($NorthEastLng-$NorthWestLng)))*6371;
	$rectangleEastFace = acos(sin(deg2rad($NorthEastLat))*sin(deg2rad($SouthEastLat))+cos(deg2rad($NorthEastLat))*cos(deg2rad($SouthEastLat))*cos(deg2rad		($NorthEastLng-$SouthEastLng)))*6371;
	$rectangleSouthFace = acos(sin(deg2rad($SouthWestLat))*sin(deg2rad($SouthEastLat))+cos(deg2rad($SouthWestLat))*cos(deg2rad($SouthEastLat))*cos(deg2rad		($SouthWestLng-$SouthEastLng)))*6371;
	$rectangleWestFace = acos(sin(deg2rad($SouthWestLat))*sin(deg2rad($NorthWestLat))+cos(deg2rad($SouthWestLat))*cos(deg2rad($NorthWestLat))*cos(deg2rad		($SouthWestLng-$NorthWestLng)))*6371;
	
	/* TEST PRINTS */
	//echo 'North = '.$rectangleNorthFace.'<br/>';
	//echo 'South = '.$rectangleSouthFace.'<br/>';
	//echo 'East = '.$rectangleEastFace.'<br/>';
	//echo 'West = '.$rectangleWestFace.'<br/>';

	/* Find the biggest face and calculate the number of divisions */
	if ((($rectangleNorthFace + $rectangleSouthFace)/2.0) >= (($rectangleEastFace + $rectangleWestFace)/2.0))
	{
		$division = (($rectangleEastFace + $rectangleWestFace)/(2.0*$cameraCoeff*$HeightQuadcopter*$cameraLarger));
		$biggestFace = "NorthSouth";
	}
	else
	{
		$division = (($rectangleNorthFace + $rectangleSouthFace)/(2.0*$cameraCoeff*$HeightQuadcopter*$cameraLarger)); 
		$biggestFace = "EastWest";
	}

	/* TEST PRINTS */
	//echo 'Division = '.$division.'<br/>';

	/* Distance Calculation in Kilometers to find the First Location */
	$startToNorthEast = acos(sin(deg2rad($NorthEastLat))*sin(deg2rad($StartLat))+cos(deg2rad($NorthEastLat))*cos(deg2rad($StartLat))*cos(deg2rad($NorthEastLng-$StartLng)))*6371;

	$startToNorthWest = acos(sin(deg2rad($NorthWestLat))*sin(deg2rad($StartLat))+cos(deg2rad($NorthWestLat))*cos(deg2rad($StartLat))*cos(deg2rad($NorthWestLng-$StartLng)))*6371;

	$startToSouthEast = acos(sin(deg2rad($SouthEastLat))*sin(deg2rad($StartLat))+cos(deg2rad($SouthEastLat))*cos(deg2rad($StartLat))*cos(deg2rad($SouthEastLng-$StartLng)))*6371;

	$startToSouthWest = acos(sin(deg2rad($SouthWestLat))*sin(deg2rad($StartLat))+cos(deg2rad($SouthWestLat))*cos(deg2rad($StartLat))*cos(deg2rad($SouthWestLng-$StartLng)))*6371;

	/* Find the nearest location of the start location */
	if($startToNorthEast <= $startToNorthWest & $startToNorthEast <= $startToSouthEast & $startToNorthEast <= $startToSouthWest)
	{
		$startLocation = "NorthEast";
	}
	else
	{
		if($startToNorthWest <= $startToNorthEast & $startToNorthWest <= $startToSouthEast & $startToNorthWest <= $startToSouthWest)
		{
			$startLocation = "NorthWest";
		}
		else
		{
			if($startToSouthEast <= $startToNorthEast & $startToSouthEast <= $startToNorthWest & $startToSouthEast <= $startToSouthWest)
			{
				$startLocation = "SouthEast";
			}
			else 
			{
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
			
			/* East/West is the biggest face */
			if ($biggestFace == "EastWest")
			{		
				/* Sweeping Interval Calculation */
				$IntervalSweeping = ((($NorthEastLng + $SouthEastLng)/2) - (($NorthWestLng + $SouthWestLng)/2))/$division;
				
				/* GPSPath Index Initialisation */
				$IndexGPSPath = 1;

				for ($Lng = $NorthEastLng; $Lng >= $NorthWestLng; $Lng = $Lng - ($IntervalSweeping*2))
				{
					for ($Lat = $NorthEastLat; $Lat >= $SouthEastLat; $Lat = $Lat - $GPSInterval)
					{
						$GPSLatPath[$IndexGPSPath] = $Lat;
						$GPSLngPath[$IndexGPSPath] = $Lng;
						$IndexGPSPath = $IndexGPSPath + 1;
					}

					if (($Lng - $IntervalSweeping) >= $NorthWestLng)
					{
						$LngLimit = $Lng- $IntervalSweeping;
						for ($Lng = $Lng; $Lng > $LngLimit; $Lng = $Lng - $GPSInterval)
						{
							$GPSLatPath[$IndexGPSPath] = $Lat;
							$GPSLngPath[$IndexGPSPath] = $Lng;
							$IndexGPSPath = $IndexGPSPath + 1;	
						}
						$Lng = $LngLimit;
						$GPSLatPath[$IndexGPSPath] = $Lat;
						$GPSLngPath[$IndexGPSPath] = $Lng;
						$IndexGPSPath = $IndexGPSPath + 1;

						
						for ($Lat = $Lat; $Lat <= $NorthWestLat; $Lat = $Lat + $GPSInterval)
						{
							$GPSLatPath[$IndexGPSPath] = $Lat;
							$GPSLngPath[$IndexGPSPath] = $Lng;
							$IndexGPSPath = $IndexGPSPath + 1;
						}
					
						if (($Lng - $IntervalSweeping) >= $NorthWestLng)
						{
							$LngLimit = $Lng- $IntervalSweeping;
							for ($Lng = $Lng; $Lng > $LngLimit; $Lng = $Lng - $GPSInterval)
							{
								$GPSLatPath[$IndexGPSPath] = $Lat;
								$GPSLngPath[$IndexGPSPath] = $Lng;
								$IndexGPSPath = $IndexGPSPath + 1;
							}
						}				
					}
				}	
			}

			/* North/South is the biggest face */
			else
			{		
				/* Sweeping Interval Calculation */
				$IntervalSweeping = ((($NorthEastLng + $SouthEastLng)/2) - (($NorthWestLng + $SouthWestLng)/2))/$division;
				
				/* GPSPath Index Initialisation */
				$IndexGPSPath = 1;

				for ($Lat = $NorthEastLat; $Lat >= $SouthEastLat; $Lat = $Lat - ($IntervalSweeping*2))
				{
					for ($Lng = $NorthEastLng; $Lng >= $NorthWestLng; $Lng = $Lng - $GPSInterval)
					{
						$GPSLatPath[$IndexGPSPath] = $Lat;
						$GPSLngPath[$IndexGPSPath] = $Lng;
						$IndexGPSPath = $IndexGPSPath + 1;
					}

					if (($Lat - $IntervalSweeping) >= $SouthEastLat)
					{
						$LatLimit = $Lat- $IntervalSweeping;
						for ($Lat = $Lat; $Lat > $LatLimit; $Lat = $Lat - $GPSInterval)
						{
							$GPSLatPath[$IndexGPSPath] = $Lat;
							$GPSLngPath[$IndexGPSPath] = $Lng;
							$IndexGPSPath = $IndexGPSPath + 1;	
						}
						$Lat = $LatLimit;
						$GPSLatPath[$IndexGPSPath] = $Lat;
						$GPSLngPath[$IndexGPSPath] = $Lng;
						$IndexGPSPath = $IndexGPSPath + 1;

						
						for ($Lng = $Lng; $Lng <= $SouthEastLng; $Lng = $Lng + $GPSInterval)
						{
							$GPSLatPath[$IndexGPSPath] = $Lat;
							$GPSLngPath[$IndexGPSPath] = $Lng;
							$IndexGPSPath = $IndexGPSPath + 1;
						}
					
						if (($Lat - $IntervalSweeping) >= $SouthEastLat)
						{
							$LatLimit = $Lat- $IntervalSweeping;
							for ($Lat = $Lat; $Lat > $LatLimit; $Lat = $Lat - $GPSInterval)
							{
								$GPSLatPath[$IndexGPSPath] = $Lat;
								$GPSLngPath[$IndexGPSPath] = $Lng;
								$IndexGPSPath = $IndexGPSPath + 1;
							}
						}				
					}
				}	
			}
		}
        	break;

		/* North West Case */
		case "NorthWest":{

			/* First GPS position initialisation */
			$GPSLatPath[0] = $NorthWestLat;
			$GPSLngPath[0] = $NorthWestLng;
			
			/* East/West is the biggest face */
			if ($biggestFace == "EastWest")
			{		
				/* Sweeping Interval Calculation */
				$IntervalSweeping = ((($NorthEastLng + $SouthEastLng)/2) - (($NorthWestLng + $SouthWestLng)/2))/$division;
				
				/* GPSPath Index Initialisation */
				$IndexGPSPath = 1;

				for ($Lng = $NorthWestLng; $Lng <= $NorthEastLng; $Lng = $Lng + ($IntervalSweeping*2))
				{
					for ($Lat = $NorthWestLat; $Lat >= $SouthWestLat; $Lat = $Lat - $GPSInterval)
					{
						$GPSLatPath[$IndexGPSPath] = $Lat;
						$GPSLngPath[$IndexGPSPath] = $Lng;
						$IndexGPSPath = $IndexGPSPath + 1;
					}

					if (($Lng + $IntervalSweeping) <= $NorthWestLng)
					{
						$LngLimit = $Lng + $IntervalSweeping;
						for ($Lng = $Lng; $Lng < $LngLimit; $Lng = $Lng - $GPSInterval)
						{
							$GPSLatPath[$IndexGPSPath] = $Lat;
							$GPSLngPath[$IndexGPSPath] = $Lng;
							$IndexGPSPath = $IndexGPSPath + 1;	
						}
						$Lng = $LngLimit;
						$GPSLatPath[$IndexGPSPath] = $Lat;
						$GPSLngPath[$IndexGPSPath] = $Lng;
						$IndexGPSPath = $IndexGPSPath + 1;

						
						for ($Lat = $Lat; $Lat <= $NorthWestLat; $Lat = $Lat + $GPSInterval)
						{
							$GPSLatPath[$IndexGPSPath] = $Lat;
							$GPSLngPath[$IndexGPSPath] = $Lng;
							$IndexGPSPath = $IndexGPSPath + 1;
						}
					
						if (($Lng + $IntervalSweeping) <= $NorthWestLng)
						{
							$LngLimit = $Lng + $IntervalSweeping;
							for ($Lng = $Lng; $Lng < $LngLimit; $Lng = $Lng + $GPSInterval)
							{
								$GPSLatPath[$IndexGPSPath] = $Lat;
								$GPSLngPath[$IndexGPSPath] = $Lng;
								$IndexGPSPath = $IndexGPSPath + 1;
							}
						}				
					}
				}	
			}

			/* North/South is the biggest face */
			else
			{		
				/* Sweeping Interval Calculation */
				$IntervalSweeping = ((($NorthEastLng + $SouthEastLng)/2) - (($NorthWestLng + $SouthWestLng)/2))/$division;

				/* GPSPath Index Initialisation */
				$IndexGPSPath = 1;

				for ($Lat = $NorthEastLat; $Lat >= $SouthEastLat; $Lat = $Lat - ($IntervalSweeping*2))
				{
					for ($Lng = $NorthEastLng; $Lng <= $NorthWestLng; $Lng = $Lng + $GPSInterval)
					{
						$GPSLatPath[$IndexGPSPath] = $Lat;
						$GPSLngPath[$IndexGPSPath] = $Lng;
						$IndexGPSPath = $IndexGPSPath + 1;
					}

					if (($Lat - $IntervalSweeping) >= $SouthEastLat)
					{
						$LatLimit = $Lat- $IntervalSweeping;
						for ($Lat = $Lat; $Lat > $LatLimit; $Lat = $Lat - $GPSInterval)
						{
							$GPSLatPath[$IndexGPSPath] = $Lat;
							$GPSLngPath[$IndexGPSPath] = $Lng;
							$IndexGPSPath = $IndexGPSPath + 1;	
						}
						$Lat = $LatLimit;
						$GPSLatPath[$IndexGPSPath] = $Lat;
						$GPSLngPath[$IndexGPSPath] = $Lng;
						$IndexGPSPath = $IndexGPSPath + 1;

						
						for ($Lng = $Lng; $Lng <= $NorthWestLng; $Lng = $Lng - $GPSInterval)
						{
							$GPSLatPath[$IndexGPSPath] = $Lat;
							$GPSLngPath[$IndexGPSPath] = $Lng;
							$IndexGPSPath = $IndexGPSPath + 1;
						}
					
						if (($Lat - $IntervalSweeping) >= $SouthWestLat)
						{
							$LatLimit = $Lat- $IntervalSweeping;
							for ($Lat = $Lat; $Lat > $LatLimit; $Lat = $Lat - $GPSInterval)
							{
								$GPSLatPath[$IndexGPSPath] = $Lat;
								$GPSLngPath[$IndexGPSPath] = $Lng;
								$IndexGPSPath = $IndexGPSPath + 1;
							}
						}				
					}
				}	
			}
		}
        	break;
	    
		/* South East Case */
		case "SouthEast":
			//code to be executed if n=label3;
		break;
	   
		/* South West Case */
		default:
			//code to be executed if n is different from all labels;
	}
	
	$fileName = md5(microtime(TRUE)*100000);
	$type = "application/csv";
	$myFile = fopen('../navigationMaps/'.$fileName.'.csv', 'w');
    
	for ($Index = 0 ; $Index<count($GPSLngPath) ; $Index++)
    	{
		fwrite ($myFile, $GPSLatPath[$Index]); // On y met les données
		fwrite ($myFile, ',');
		fwrite ($myFile, $GPSLngPath[$Index]); // On y met les données
		fputs($myFile, "\n"); 
    	}
	
	fclose($myFile);
	
	// Création des headers, pour indiquer au navigateur qu'il s'agit d'un fichier à télécharger
	header('Content-Transfer-Encoding: binary'); //Transfert en binaire (fichier)
	header('Content-Disposition: attachment; filename="MyAuraProjectPath".csv'); //Nom du fichier
	header('Content-Length: '.filesize('../navigationMaps/'.$fileName.'.csv')); //Taille du fichier
	header("Cache-Control: must-revalidate, post-check=0, pre-check=0, public");
	header("Content-Transfer-Encoding: $type\n"); // Surtout ne pas enlever le \n
	header("Pragma: no-cache");
	header("Expires: 0");
	
	//Envoi du fichier dont le chemin est passé en paramètre
	readfile('../navigationMaps/'.$fileName.'.csv'); 

?>
