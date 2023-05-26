<?php
require_once 'config.php';

function getJsonString($count, $rankNumber, $rang_up_nummer_min, $rang_up_nummer_max, $rankName, $rankProgressInPercent,
 $playRankUpAudio, $isRankUpAudioPlayed, $audioVolumeWatch, $timestamp){

	return json_encode([
	'count' => $count, 	
	'rankNumber' => $rankNumber, 	
	'rangMin' => $rang_up_nummer_min,
	'rangMax' => $rang_up_nummer_max,	
	'rankName' => utf8_encode($rankName), 
	'rankProgress' => $rankProgressInPercent, 
	'playRankUpAudio' => $playRankUpAudio, 
	'audioVolumeWatch' => $audioVolumeWatch,
	'isRankUpPlayed' => $isRankUpAudioPlayed,
	'last_timestamp' => $timestamp, 
	'now_timestamp' => getNowTimestamp()
	]);
} 

function getData($db) {
	
 	$countBottles = getNumberOfAllMate($db);
	list($rang, $rang_up_nummer_min, $rang_up_nummer_max, $rang_name, $percent_to_next, $playRankUpAudio, $isRankUpAudioPlayed) = getRank($db, $countBottles);
	
	$audioVolumeWatch = "20"; // max. = 30
	$lastTimestamp = getLastTimestamp($db);

	return getJsonString($countBottles, $rang, $rang_up_nummer_min, $rang_up_nummer_max, $rang_name, $percent_to_next, $playRankUpAudio, $isRankUpAudioPlayed, $audioVolumeWatch, $lastTimestamp);
}

function getRank($db, $drunkBottles) {

    $stmt = $db->prepare("SELECT * FROM Mate_Rang WHERE rang_up_nummer_min <= :drunkBottles AND rang_up_nummer_max >= :drunkBottles LIMIT 1");
    $stmt->bindParam(':drunkBottles', $drunkBottles, PDO::PARAM_INT);
    $stmt->execute();
    $row = $stmt->fetch(PDO::FETCH_ASSOC);	

	$rang = $row['rang'];
	$rang_up_nummer_min = $row['rang_up_nummer_min'];
	$rang_up_nummer_max = $row['rang_up_nummer_max'];
	$rang_name = $row['rang_name'];
	$isRankUpAudioPlayed = $row['wurde_rang_up_gespielt'];

	$playRankUpAudio = 0;

	if($rang_up_nummer_min == $drunkBottles && $isRankUpAudioPlayed == 0) {
		$playRankUpAudio = 1; 
	}

	// Berechne wieviel Prozent, es noch bis zum nächsten Rang sind
	$max = $rang_up_nummer_max + 1; // hier immer um 1 hochzählen, damit es nie 100% werden
	$diff = ($drunkBottles - $rang_up_nummer_min) / ($max - $rang_up_nummer_min) * 100;
	$percent_to_next = floor($diff);

	return [$rang, $rang_up_nummer_min, $rang_up_nummer_max, $rang_name, $percent_to_next, $playRankUpAudio, $isRankUpAudioPlayed];
}

// Setzt den aktuellen Rang auf "true" und alle anderen Ränge auf "false", damit das Audio nochmal abgespielt, 
// falls man wieder eine Flasche zurück geht oder wieder von vorne anfängt
function setRankUpAudioPlayed($db, $rang) {
	try {
    	$stmt = $db->prepare("UPDATE Mate_Rang
						  SET wurde_rang_up_gespielt = CASE 
							  WHEN rang = :rang THEN 1 
							  ELSE 0 
						  END");
		$stmt->bindParam(':rang', $rang, PDO::PARAM_INT);
		$stmt->execute();
	} catch(PDOException $e) {
		echo "SQL-Fehler: " . $e->getMessage();
	}
} 

function getRowAfterNum($db, $num) {
    $sql = "SELECT * FROM Mate_Rang WHERE rang_ab_nummer > $num ORDER BY rang_ab_nummer ASC LIMIT 1";
    $result = $db->query($sql);
    $row = $result->fetch_assoc();
	echo "Rang After: " . $row['rang'] . ", Rang-Ab-Nummer: " . $row['rang_ab_nummer'] . ", Rang-Name: " . $row['rang_name'] . PHP_EOL;
    return $row;
} 

function setPlusOne($db, $count) {
	$timestamp = date('Y-m-d H:i:s');
    $stmt = $db->prepare("INSERT INTO Mate_Counter (count, timestamp) VALUES (?, ?)");
    $stmt->execute([$count++, $timestamp]);
}

function deleteLastItem($db) {
	$stmt = $db->prepare("DELETE FROM Mate_Counter ORDER BY timestamp DESC LIMIT 1");
    $stmt->execute();
}

function getNumberOfAllMate($db) {
 	$stmt_counter = $db->query("SELECT COUNT(*) as count FROM Mate_Counter");	
	$row_counter = $stmt_counter->fetch(PDO::FETCH_ASSOC);
	$count = (int)$row_counter['count'];
	return $count;	
} 

function getLastTimestamp($db) {
	$stmt_last_timestamp = $db->query("SELECT timestamp FROM Mate_Counter ORDER BY timestamp DESC LIMIT 1");	
	$row_last_timestamp = $stmt_last_timestamp->fetch(PDO::FETCH_ASSOC);
	$timestamp = new DateTime($row_last_timestamp['timestamp']);
	$timestamp->format('Y-m-d H:i:s');
	return $timestamp;
}

function getNowTimestamp() {
	$now = new DateTime();
	$date = $now->format('Y-m-d H:i:s.u');
	$timezoneName = $now->getTimezone()->getName();
	$timezoneType = $now->getTimezone()->getOffset($now) / 3600;
	$output = array('date' => $date, 'timezone_type' => $timezoneType, 'timezone' => $timezoneName);
	return $output;
}

function getAuthorizationHeader(){
  	if (!isset(apache_request_headers()['Authorization'])) {
		http_response_code(402);
		echo json_encode(array("message" => "Kein JWT-Token angegeben."));
		exit();
	}  

	// Extrahiere das JWT-Token aus dem Anfrage-Header
	$authHeader = apache_request_headers()['Authorization'];
	$jwtToken = substr($authHeader, 7);

	// Entschlüssle das JWT-Token mit dem geheimen Schlüssel
	$jwtDecoded = base64_decode(str_replace('_', '/', str_replace('-','+',explode('.', $jwtToken)[1]))); 

	// Überprüfe, ob das JWT-Token erfolgreich entschlüsselt werden konnte
 	if (!$jwtDecoded) {
		http_response_code(401);
		echo json_encode(array("message" => "Ungültiges JWT-Token."));
		exit();
	} 

	return $jwtDecoded;
}

function getUserName($jwtDecoded){
	$jsonObj = json_decode($jwtDecoded);
	$key = "username";
	$username = $jsonObj->$key;

	return $username;
} 

function getPassword($jwtDecoded){
	$jsonObj = json_decode($jwtDecoded);
	$key = "password";
	$password = $jsonObj->$key;

	return $password;
}

function isLoginOK($username, $password){
	$validUser = $username == $usernameAPI && $password == $passwordAPI;
	if(!$validUser) {
		echo json_encode(array("message" => "Falsche username oder password."));
		http_response_code(401);
		exit();
	} else {
		http_response_code(200);	
	}
}

function checkAuthentication(){
	// lese den JWT-Token aus dem Header
	$jwtDecoded = getAuthorizationHeader();
	
	// Suche aus dem JWT-Token, username und password
	$username = getUserName($jwtDecoded);
	$password = getPassword($jwtDecoded);
	
	// vergleiche username und password
	isLoginOK($username, $password);
}

// Mit dieser Funktion bekomme ich als 0 zurück, wieso auch immer
function getDataAsJson(){
	// Lese alle Einträge aus der Datenbank und gib sie als JSON aus
	$count = getNumberOfAllMate($db);
	$timestamp = getLastTimestamp($db);

	// encode Json
	return json_encode(['count' => $count, 'last_timestamp' => $timestamp, 'now_timestamp' => getNowTimestamp()]);
}

if (isset($_POST['action'])) {
	if ($_POST['action'] === 'increase') {
		global $db;
		setPlusOne($db, getNumberOfAllMate($db));
		echo getNumberOfAllMate($db);
	}
}

if (isset($_POST['action'])) {
	if ($_POST['action'] === 'decrease') {
		global $db;
		deleteLastItem($db);
		echo getNumberOfAllMate($db);
	}
}

if (isset($_POST['action'])) {
	if ($_POST['action'] === 'getData') {
		global $db;
		echo getData($db);
	}
}

?>