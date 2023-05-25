<?php

function reset_blocked_IP_if_out_of_time() {
	if(isBlockedIPOutOfTime()) {
		global $db;
		$user_ip = $_SERVER['REMOTE_ADDR'];
		$stmt = $db->prepare("UPDATE Mate_Blocklist SET attempts = 0 WHERE ip_address = :ip_address");
		$stmt->execute(array(":ip_address" => $user_ip));
		return true;
	}
	return false;
}

function isBlockedIPOutOfTime() {
	global $blockDuration;
	
	return getDiffSecondsBlockedIP() >= $blockDuration;
}

function getDiffSecondsBlockedIP() {
	$lastAttempt = getLastAttempt();
	$current_datetime = date('Y-m-d H:i:s');

	$datetime1 = new DateTime($lastAttempt);
	$datetime2 = new DateTime($current_datetime);

	$diff = $datetime1->getTimestamp() - $datetime2->getTimestamp();

	$diff_seconds = abs($diff);

	return $diff_seconds;
}

// hat der User noch versuche zum einloggen
function isAttemptsEnd() {
	return getRemainingAttempts() <= 0;
}

// wieviel versuche hat der user noch, bis er geblockt wird
function getRemainingAttempts() {
	global $db;
	global $maxLoginAttempts;
	
	$attempts = getAttempts();
	
	return $maxLoginAttempts - $attempts;
}

// Gibt die Zeit vom letzen Login-Versuch zurück
function getLastAttempt() {
	global $db;
  
  	// eigene IP Adresse
	$user_ip = $_SERVER['REMOTE_ADDR'];
  
	$stmt = $db->prepare('SELECT last_attempt FROM Mate_Blocklist WHERE ip_address = :ip_address');
	$stmt->execute(['ip_address' => $user_ip]);
	$result = $stmt->fetch(PDO::FETCH_ASSOC);
	return $result['last_attempt'];
}

// Gibt zurück, wie oft der user ein Login versucht hat
function getAttempts() {
	global $db;
	
	// eigene IP Adresse
	$user_ip = $_SERVER['REMOTE_ADDR'];
	
	$stmt = $db->prepare('SELECT attempts FROM Mate_Blocklist WHERE ip_address = :ip_address');
	$stmt->execute(['ip_address' => $user_ip]);
	$result = $stmt->fetch(PDO::FETCH_ASSOC);
	
	if ($result) {
		return $result['attempts'];
	} else {
		return 0;
	}
}

// Trägt die IP-Adresse und Datum mit Uhrzeit in die DB ein
function addLoginInDB($isLoginSuccess) {
	global $db;

	// eigene IP Adresse
	$user_ip = $_SERVER['REMOTE_ADDR'];

	// Wenn die Anmeldung erfolgreich war
    if ($isLoginSuccess) {
        // Eintrag in die Datenbank hinzufügen oder aktualisieren (wenn es den PrimaryKey schon gibt)
		// beim aktualisieren wird "attempts" wieder auf 0 gesetzt
        $now = date('Y-m-d H:i:s');
        $stmt = $db->prepare("INSERT INTO Mate_Blocklist (ip_address, last_attempt, isLoginSuccess, attempts) 
								VALUES (:ip_address, :last_attempt, :isLoginSuccess, 0) 
								ON DUPLICATE KEY UPDATE last_attempt = :last_attempt, attempts = 0");
        $stmt->bindParam(':ip_address', $user_ip);
        $stmt->bindParam(':last_attempt', $now);
		$stmt->bindParam(':isLoginSuccess', $isLoginSuccess);
		//$stmt->bindParam(':attempts', 0);
        $stmt->execute();
    } else {
        // Eintrag in die Datenbank hinzufügen oder aktualisieren (wenn es den PrimaryKey schon gibt)
		// beim aktualisieren wird "attempts" um 1 erhöht, wenn der Login falsch war
		// ON DUPLICATE KEY = wenn es den PrimaryKey schon gibt, dann führe den sql-befehl aus, 
		// der hinter "ON DUPLICATE KEY"steht, PrimaryKey = ip_address		
        $now = date('Y-m-d H:i:s');
        $stmt = $db->prepare("INSERT INTO Mate_Blocklist (ip_address, last_attempt, isLoginSuccess, attempts) 
									VALUES (:ip_address, :last_attempt, :isLoginSuccess, 1) 
									ON DUPLICATE KEY UPDATE last_attempt = :last_attempt, attempts = attempts + 1");
        $stmt->bindParam(':ip_address', $user_ip);
        $stmt->bindParam(':last_attempt', $now);		
		$stmt->bindParam(':isLoginSuccess', $isLoginSuccess);
        $stmt->execute();

        // Fehlermeldung ausgeben
        $error = "Benutzername oder Passwort falsch.";
    }
}
?>
