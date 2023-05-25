<?php
// Config - Website (Frontend)
$usernameLogin = "";
$passwordLogin = "";

$maxLoginAttempts = 10;// Definiere die maximale Anzahl von Login-Versuchen
$blockDuration = 3600; // Definiere die Sperrdauer in Sekunden, 3600 = 1 Stunde

// Config - API
$usernameAPI = "";
$passwordAPI = "";
//$secretKey = "tester";

// Config - Datenbank
$host = '';
$dbname = '';
$username = '';
$password = '';

// Verbindung zur Datenbank herstellen
try {
    $db = new PDO("mysql:host=$host;dbname=$dbname", $username, $password);
} catch (PDOException $e) {
    die("Verbindung zur Datenbank fehlgeschlagen: " . $e->getMessage());
}

?>