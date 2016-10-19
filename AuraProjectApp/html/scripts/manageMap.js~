/* Global variables*/

var globalMap;
var globalPolygon;
var globalPosition;
var globalMarker;
var globalRectangle;
var globalSecurityRectangle;

/* Function to find the user s GPS location via HTML5 */

function findLocation() {
	if (navigator.geolocation)
		navigator.geolocation.getCurrentPosition(successCallback, errorCallback);
	else
		alert("Votre navigateur ne prend pas en compte la géolocalisation HTML5");
	
	function successCallback(position){
		var location = {lat:position.coords.latitude , lng:position.coords.longitude};
		refreshMapCenter(location);
	};  

	function errorCallback(error){
		switch(error.code){
			case error.PERMISSION_DENIED:
				alert("L'utilisateur n'a pas autorisé l'accès à sa position");
				break;          
			case error.POSITION_UNAVAILABLE:
				alert("L'emplacement de l'utilisateur n'a pas pu être déterminé");
				break;
			case error.TIMEOUT:
				alert("Le service n'a pas répondu à temps");
				break;
			}
	};
}


/* Function to find the GPS location of an address via Google Map API */

function findAddress() {
	if (document.forms["form"]["addressAttribute"].value == null || document.forms["form"]["addressAttribute"].value == "")
	{
		alert("Vous devez renseigner l'adresse");
	}
	else {		
		var geocoder = new google.maps.Geocoder();
		// Récupération de l'adresse tapée dans le formulaire
		var adresse = document.getElementById('addressAttribute').value;
		geocoder.geocode( { 'address': adresse}, function(results, status) {

			if (status == google.maps.GeocoderStatus.OK) {

				// Création de la carte et du marqueur du lieu (épingle)
				refreshMapCenter({lat:results[0].geometry.location.lat(), lng:results[0].geometry.location.lng()});
			} 
			else {
				alert('Adresse introuvable: ' + status);
			}
		});
	}
}


/* Map initialisation */
function initMap() {
	var rectangleHeight = 0.0004;
	var rectangleWidth = 0.0004;
	var GPSPosition = {lat:45.91926,lng:6.157999600000039};
	var map = new google.maps.Map(document.getElementById('map'), {
	zoom: 19,
	center: GPSPosition,
	scrollwheel: true,
	mapTypeId: google.maps.MapTypeId.ROADMAP
	});

	var marker = new google.maps.Marker({
	position: GPSPosition,
	map: map,
	title: "Position GPS",
	draggable:true
	});
	
	
	/* Define the LatLng coordinates for the polygon's path */
	var polygonCoords = [
	{lat: GPSPosition.lat-(rectangleHeight/2), lng: GPSPosition.lng-(rectangleWidth/2)},
	{lat: GPSPosition.lat-(rectangleHeight/2), lng: GPSPosition.lng+(rectangleWidth/2)},
	{lat: GPSPosition.lat+(rectangleHeight/2), lng: GPSPosition.lng+(rectangleWidth/2)},
	{lat: GPSPosition.lat+(rectangleHeight/2), lng: GPSPosition.lng-(rectangleWidth/2)}
	];
	
	/* Draw the polygon */
	var polygon = new google.maps.Polygon({
	paths: polygonCoords,
	strokeColor: '#FF0000',
	strokeOpacity: 0.8,
	strokeWeight: 2,
	fillColor: '#FF0000',
	fillOpacity: 0.35,
	draggable:true,
	editable:true
	});
	polygon.setMap(map);


	globalMarker = marker;
	globalPolygon = polygon;
	globalMap = map;
}

/* Function for refresh the center of the map */

function refreshMapCenter(GPSPosition) {
	var rectangleHeight = 0.0004;
	var rectangleWidth = 0.0004;
	
	/* Remove the last polygon */
	globalPolygon.setMap(null);

	var polygonCoords = [
		{lat: GPSPosition.lat-(rectangleHeight/2), lng: GPSPosition.lng-(rectangleWidth/2)},
		{lat: GPSPosition.lat-(rectangleHeight/2), lng: GPSPosition.lng+(rectangleWidth/2)},
		{lat: GPSPosition.lat+(rectangleHeight/2), lng: GPSPosition.lng+(rectangleWidth/2)},
		{lat: GPSPosition.lat+(rectangleHeight/2), lng: GPSPosition.lng-(rectangleWidth/2)}
		];
	
	/* Draw the polygon */
	var polygon = new google.maps.Polygon({
	paths: polygonCoords,
	strokeColor: '#FF0000',
	strokeOpacity: 0.8,
	strokeWeight: 2,
	fillColor: '#FF0000',
	fillOpacity: 0.35,
	draggable:true,
	editable:true
	});

	polygon.setMap(globalMap);
	globalPolygon = polygon;

	/* Move marker, polygon and map center */

	globalMarker.setPosition(GPSPosition);
    	globalMap.panTo(GPSPosition);

	globalMap.setZoom(19);	
}


/* Function for calculate the minimum-bounding-boxes */

function rectangleDrawer() {
	var polygonNewCoords = globalPolygon.getPath();
	var indexMaxLat = 0;
	var indexMinLat = 0;
	var indexMaxLng = 0;
	var indexMinLng = 0;	
	

	// Iterate over the polygonNewCoords.
	for (var i =1; i < polygonNewCoords.getLength(); i++) {
		var newPoint = polygonNewCoords.getAt(i);
		
		if (newPoint.lat() < polygonNewCoords.getAt(indexMinLat).lat())
		{
			indexMinLat = i;
		}
		else
		{
			if (newPoint.lat() > polygonNewCoords.getAt(indexMaxLat).lat())
			{
				indexMaxLat = i;
			}
		}
		
		if (newPoint.lng() < polygonNewCoords.getAt(indexMinLng).lng())
		{
			indexMinLng = i;
		}
		else
		{
			if (newPoint.lng() > polygonNewCoords.getAt(indexMaxLng).lng())
			{
				indexMaxLng = i;
			}
		}		
	}
	
	if (globalRectangle == null)
	{
		var rectangle = new google.maps.Rectangle({
			strokeColor: '#FF0000',
			strokeOpacity: 0.8,
			strokeWeight: 2,
			fillColor: '#FF0000',
			fillOpacity: 0.35,
			map: globalMap,
			bounds: {
				north: polygonNewCoords.getAt(indexMinLat).lat(),
				south: polygonNewCoords.getAt(indexMaxLat).lat(),
				east: polygonNewCoords.getAt(indexMaxLng).lng(),
				west: polygonNewCoords.getAt(indexMinLng).lng()
			}
		});
		
		globalRectangle = rectangle;
		
		var securityRectangle = new google.maps.Rectangle({
			strokeColor: '#FFA500',
			strokeOpacity: 0.3,
			strokeWeight: 1,
			fillColor: '#FFA500',
			fillOpacity: 0.35,
			map: globalMap,
			bounds: {
				north: polygonNewCoords.getAt(indexMinLat).lat()-0.000075,
				south: polygonNewCoords.getAt(indexMaxLat).lat()+0.000075,
				east: polygonNewCoords.getAt(indexMaxLng).lng()+0.0001,
				west: polygonNewCoords.getAt(indexMinLng).lng()-0.0001
			}
		});
		globalSecurityRectangle = securityRectangle;
	}
	else
	{
		
		globalRectangle.setBounds({
				north: polygonNewCoords.getAt(indexMinLat).lat(),
				south: polygonNewCoords.getAt(indexMaxLat).lat(),
				east: polygonNewCoords.getAt(indexMaxLng).lng(),
				west: polygonNewCoords.getAt(indexMinLng).lng()
			}
			);
		
		globalSecurityRectangle.setBounds({
				north: polygonNewCoords.getAt(indexMinLat).lat()-0.0001,
				south: polygonNewCoords.getAt(indexMaxLat).lat()+0.0001,
				east: polygonNewCoords.getAt(indexMaxLng).lng()+0.0001,
				west: polygonNewCoords.getAt(indexMinLng).lng()-0.0001
			}
			);
	}
}

/* Function for send all GPS locations and file download redirection */

function wayDrawer() {

	if (document.forms["form"]["altitudeAttribute"].value == null || document.forms["form"]["altitudeAttribute"].value == "")
	{
		alert("Vous devez renseigner une hauteur de vol");
	}
	else
	{
		var rectangleBounds = globalRectangle.getBounds();
		var rectangleNorthEast = rectangleBounds.getNorthEast();
		var rectangleSouthWest = rectangleBounds.getSouthWest();
		var elevationNorthEast = -123456789;
		var elevationSouthWest = -123456789;
		var locations = [];
		var heightQuadcopter = document.forms["form"]["altitudeAttribute"].value;

		locations.push(new google.maps.LatLng(rectangleNorthEast.lat(),rectangleNorthEast.lng()));
		locations.push(new google.maps.LatLng(rectangleSouthWest.lat(),rectangleSouthWest.lng()));

		var positionalRequest = {
			'locations': locations
		};

		var elevator = new google.maps.ElevationService();
		var statusGlobal = 0;
		elevator.getElevationForLocations(positionalRequest, function(results, status) {
			
			if (google.maps.ElevationStatus.OK && results[0]) {
			  		elevationNorthEast = results[0].elevation;
					elevationSouthWest = results[1].elevation;
			}
			else
			{
				alert("L'altitude de la zone géographique n'a pas pu déterminée");
			}
		});

			setTimeout(function redirect()
			{
				if (elevationNorthEast !=-123456789 & elevationSouthWest!=-123456789)
				{
					var elevation;

					if (elevationSouthWest >= elevationNorthEast)
					{
						elevation = elevationSouthWest;
					}
					else
					{
						elevation = elevationNorthEast;
					}

					var url ="/scripts/createNavigation.php?NorthEastLat=" + rectangleNorthEast.lat() + "&NorthEastLng=" + rectangleNorthEast.lng() 						+ "&SouthWestLat=" + rectangleSouthWest.lat()+ "&SouthWestLng=" + rectangleSouthWest.lng() + "&Elevation=" + elevation + 						"&HeightQuadcopter=" + heightQuadcopter + "&startLat=" + globalMarker.getPosition().lat() + "&startLng=" + 						globalMarker.getPosition().lng();
	
					document.location.href = url;
				}
				else
				{
					alert("L'altitude de la zone géographique n'a pas pu déterminée");
				}
			},1000);
	}
}



