<?php
	
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
	readfile('../navigationMaps/'.$fileName.'.csv');

?>
