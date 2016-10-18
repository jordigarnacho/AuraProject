﻿<html>
	<head>
		<meta charset="utf-8">
		<link rel="stylesheet" href="css/style.css" />
		<link rel="icon" href="images/favIconAura16x16.ico" type="image/x-icon"/>
		<link rel="shortcut icon" href="images/favIconAura16x16.ico" type="image/x-icon"/>
		<title>Aura Project - WEB App</title>
	</head>
	<body onload="initMap();">
		<!-- Map -->
		<div id="banner">
			<img src="images/logoAura.png" alt="Logo Aura">
			<p>Aura Project - Web App</p>
		</div>
		<div id="map"></div>
		<div id="tools">
			<p>1. Tracez la zone à parcourir par le drone</p>
			<p>2.Saisissez la hauteur de la prise de vue</p>
			<p>3. Validez</p>
			
			<form name="form"style="padding-top:10%">
				<div id="localisationButton"><input type="button" name="localisation" value="Ma position" onclick="findLocation();"></div>
				<div id="address"> ou</br> <input name="addressAttribute" id="addressAttribute" type="text" name="adresse">
					<input id="addressButton"type="button" name="validAddresse" value="OK" onclick="findAddress();">
				</div>
				<div id="altitude">Hauteur (m) : <input id="altitudeAttribute" type="text" name="altitude"></div>
				<div id="validButton"><input type="button" name="validate" value="Valider" onclick="rectangleDrawer();"></div>
				<div id="downloadButton"><input type="button" name="download" value="Téléchager" onclick="wayDrawer();"></div>
			</form>

		</div>
		<!-- Javascript GPS Location and Map -->
		<script type="text/javascript" src="scripts/manageMap.js"></script>
		<!-- Footer -->
		<div id="GPSLocations"></div>
		<script async defer
			src="https://maps.googleapis.com/maps/api/js?key=AIzaSyB2jhV8KMnKTHYngS-SBc-fp3g3cfS-ZEc"></script>
	</body>
</html>
