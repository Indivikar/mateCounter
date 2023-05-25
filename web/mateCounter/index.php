<?php
session_start();

require_once 'api/config.php';
require_once ('functions.php');
require_once 'api/functions.php';

reset_blocked_IP_if_out_of_time();

// Überprüfen, ob das Anmeldeformular gesendet wurde
if ($_SERVER["REQUEST_METHOD"] == "POST") {
	
    // Überprüfen, ob Benutzername und Passwort korrekt sind
    if ($_POST["username"] == $usernameLogin && $_POST["password"] == $passwordLogin) {
		
        // Der Benutzername und das Passwort sind korrekt, speichern Sie den Benutzernamen in der Sitzung
        $_SESSION["username"] = $_POST["username"];
		
        // Leite den Benutzer auf die Hauptseite weiter		
		addLoginInDB(true);
        header("Location: index.php");
        exit();
    } else {
		addLoginInDB(false);
		
        // Der Benutzername oder das Passwort sind falsch, zeigen Sie eine Fehlermeldung an
        $error = "Benutzername oder Passwort falsch (noch " . getRemainingAttempts() . " Versuche)";
    }
}
?>

<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width,height=device-height,initial-scale=1.0"/>
	<link rel="shortcut icon" href="images/favicon.ico" type="image/x-icon">
	<title>Mate-Counter</title>
	<link rel="stylesheet" href="css/style.css">	
	<link rel="stylesheet" href="css/progressBar.css">
</head>
<body>
	<div class="container">
		<div class="content">
			<?php if (isAttemptsEnd()) { ?>
				<p class="no_more_attempts">Das war's <br> Versuche es später nochmal</p>
			<?php } elseif (isset($_SESSION['username'])) { ?>
				<div class="audioControls" id="audioControls">
					<img id="audioIcon" src="images/ton_24px.png" width="20" height="20" />
					<div>
						<input type="range" min="0" max="1" step="0.05" value="1" class="volumeSlider" id="volumeSlider">
					</div>
				</div>				
				<div class="content_progress_counter" id="">
					<div class="rank_image" id="rank_image">
						<img src="images/ranks/mate_90px.bmp" alt="">
						<div class="rank_number" id="rank_number">1</div>
					</div>
					<div class="counter" id="counter"></div>
					<div class="content_progress" id="content_progress">
						<div class="progress">
							<div class="bar" id="bar" style="width: 0%;"></div>
							<div class="label" id="label">0%</div>
						</div>
					</div>
					<div class="rank_name" id="rank_name">Mate oder nicht Mate, das ist hier die Frage</div>
				</div>
				<div class="buttons" id="buttons">	
					<button class="hvr-sweep-to-left increase" id="increase" onclick="wackeln(); playSoundPlus();">+</button>
					<button class="hvr-sweep-to-right decrease" id="decrease" onclick="wackeln(); playSoundMinus();">-</button>			
				</div>				
			<?php } else { ?>	
				<form method="POST" class="login_form">
					<input class="login_bn" type="text" name="username" placeholder="Benutzername"><br>
					<input class="login_pw" type="password" name="password" placeholder="Passwort"><br>
					<button class="login_button hvr-fade" type="submit">Anmelden</button>
					<?php if(isset($error)): ?>
						<p class="login_error"><?php echo $error; ?></p>
					<?php endif; ?>
				</form>
			<?php } ?>			
		</div>
	</div>
	<script language="javascript" type="text/javascript" src="js/mateCounter.js"></script>
	</div>
</body>
</html>