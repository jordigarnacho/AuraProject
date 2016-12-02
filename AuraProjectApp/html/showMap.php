<!doctype html>

<?php
	// Check if a fileName exist
	if (isset($_GET["fileName"])) {
		$fileName = $_GET["fileName"];
		//Open the file
		$myFile = fopen('navigationMaps/'.$fileName.'.txt', 'r');

		// Check if the file exist
		if ($myFile != null)
		{
			// Read first line <=> altitude
			$altitude = fgets($myFile);
			
			// Variable for count the number of coordonates
			$coordNb = 0;

			// Read line while a line exist
			while(!feof($myFile))
			{
				// Read the latitude
				$lat[$coordNb] = fgets($myFile);
				
				// Read the longitude
				$lng[$coordNb] = fgets($myFile);

				// Counter incrementation
				$coordNb = $coordNb + 1;
			}
			// Close the file
			fclose($myFile);

			$str = '<script type="text/javascript">';
        		$str .= "var lat = ".json_encode($lat).",";
			$str .= "lng = ".json_encode($lng).",";
			$str .= "coordNb = ".json_encode($coordNb).";";
			$str .="</script>";
			echo $str;
		}
	}
?>
<html lang="fr">
	<head>
		<!-- Page Charset Attribute  -->
		<meta charset="utf-8">

		<!-- Include CSS Script -->
		<link rel="stylesheet" href="css/style.css" />

		<!-- Page Icon -->
		<link rel="icon" href="images/favIconAura16x16.ico" type="image/x-icon"/>

		<!-- Page Shortcut Icon -->
		<link rel="shortcut icon" href="images/favIconAura16x16.ico" type="image/x-icon"/>

		<!-- Page Title -->
		<title>Aura Project - WEB App</title>
	</head>
	<body onload="showMap(), addPathPoint(lat,lng,coordNb);">

		<!-- Banner Division -->
		<div id="banner">
			<!-- Banner Icon-->
			<img src="images/logoAura.png" alt="Logo Aura">

			<!-- Banner Title -->
			<p>Aura Project - Web App</p>
		</div>

		<!-- Map Division -->
		<div id="map" style="with:100%;"></div>

		<!-- Tools Division -->
		<div id="tools">
		
			<!-- Step 1 -->
			<p>1. Tracez la zone à parcourir par le drone</p>
		
			<!-- Step 2 -->
			<p>2.Saisissez la hauteur de la prise de vue</p>

			<!-- Step 3 -->
			<p>3. Validez</p>
		
			<!-- Attribute Form -->
			<form name="form"style="padding-top:10%">

				<!-- Localisation Division -->
				<div id="localisationButton"><input type="button" name="localisation" value="Ma position" onclick="findLocation();"></div>

				<!-- Address Division -->
				<div id="address"> ou</br> <input name="addressAttribute" id="addressAttribute" type="text" name="adresse">
					<input id="addressButton"type="button" name="validAddresse" value="OK" onclick="findAddress();">
				</div>

				<!-- Altitude Division -->
				<div id="altitude">Hauteur (m) : <input id="altitudeAttribute" type="text" name="altitude"></div>

				<!-- Validation Division -->
				<div id="validButton"><input type="button" name="validate" value="Valider" onclick="rectangleDrawer();"></div>

				<!-- Download Division -->
				<div id="downloadButton"><input type="button" name="download" value="Téléchager" onclick="addPathPoint(parseFloat(lat),parseFloat(lng),parseFloat(coordNb));"></div>
			</form>

		</div>
		
		<!-- Include Javascript Scripts-->
		<script type="text/javascript" src="scripts/manageMap.js"></script>
		
		<script type="text/javascript">
			coordNb= parseInt(coordNb);
			for(j=0; j<coordNb; j=j+1)
			{
				lat[j] = parseFloat(lat[j]);
				lng[j] = parseFloat(lng[j]);
			}
		</script>
			
		<!-- Google Map API connexion -->
		<script async defer
			src="https://maps.googleapis.com/maps/api/js?key=AIzaSyB2jhV8KMnKTHYngS-SBc-fp3g3cfS-ZEc"></script>
	</body>
</html>