<!doctype html>

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
	<body onload="initMap();">

		<!-- Banner Division -->
		<div id="banner">
			<!-- Banner Icon-->
			<img src="images/logoAura.png" alt="Logo Aura">

			<!-- Banner Title -->
			<p>Aura Project - Web App</p>
		</div>

		<!-- Map Division -->
		<div id="map"></div>

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
				<div id="downloadButton"><input type="button" name="download" value="Téléchager" onclick="wayDrawer();"></div>
			</form>

		</div>
		
		<!-- Include Javascript Scripts-->
		<script type="text/javascript" src="scripts/manageMap.js"></script>
		
		<!-- Google Map API connexion -->
		<script async defer
			src="https://maps.googleapis.com/maps/api/js?key=AIzaSyB2jhV8KMnKTHYngS-SBc-fp3g3cfS-ZEc"></script>
	</body>
</html>
