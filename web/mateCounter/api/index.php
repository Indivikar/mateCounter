<?php

require_once 'config.php';
require_once 'functions.php';

header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Headers: access, Origin, Content-Type, Authorization, X-Auth-Token");
header("Access-Control-Allow-Methods: GET, POST");
header("Access-Control-Allow-Credentials: true");
header("Content-Type: application/json; charset=UTF-8");

// Überprüfe die HTTP-Methode
$method = $_SERVER['REQUEST_METHOD'];

// Überprüfe den Pfad der Anfrage
$path = $_SERVER['REQUEST_URI'];

// Führe die entsprechende Aktion aus, je nach HTTP-Methode und Pfad
if ($method === 'GET' && $path === '/counter') {
	
	// ist die Anfrage genehmigt, wenn nicht wird hier abgebrochen
	checkAuthentication();

	$json_string = getData($db);
	
	// Ausgabe für Web-API
	header('Content-Type: application/json');
    echo $json_string; 
} else if ($method === 'GET' && $path === '/setrankupplayed') {
	$countBottles = getNumberOfAllMate($db);
	list($rang, $rang_up_nummer_min, $rang_up_nummer_max, $rang_name, $percent_to_next, $playRankUpAudio, $isRankUpAudioPlayed) = getRank($db, $countBottles);
     
	if (!isset($countBottles)) {
		echo json_encode(array("message" => "Variable countBottles ist null"));
	}
	 
 	if (isset($rang)) {
        setRankUpAudioPlayed($db, $rang);
    } else {
		echo json_encode(array("message" => "Variable rang ist null"));
	} 	
} else if ($method === 'POST' && $path === '/counter/increase') {
	// ist die Anfrage genehmigt, wenn nicht wird hier abgebrochen
	checkAuthentication();
	
    // Erhöhe den Zählerwert und füge einen neuen Eintrag in die Datenbank ein
	$count = getNumberOfAllMate($db);
	$timestamp = getLastTimestamp($db);
	
	setPlusOne($db, $count);
	
	$count_new = getNumberOfAllMate($db);
	
    // encode Json
	$json_string = getJsonString($count, $timestamp);
	
	// Ausgabe für Web-API
	header('Content-Type: application/json');
    echo $json_string;

} else if ($method === 'POST' && $path === '/counter/decrease') {
	// ist die Anfrage genehmigt, wenn nicht wird hier abgebrochen
	checkAuthentication();

	// Verringere den Zählerwert und entferne den letzten Eintrag aus der Datenbank
	deleteLastItem($db);

	$count = getNumberOfAllMate($db);
	$timestamp = getLastTimestamp($db);

	// encode Json
	$json_string = getJsonString($count, $timestamp);
	
	// Ausgabe für Web-API
	header('Content-Type: application/json');
    echo $json_string;
	
} else {
    // Falls der Pfad nicht erkannt wird, gib eine Fehlermeldung aus
    header("HTTP/1.0 404 Not Found");
    echo "404 Not Found";
}

