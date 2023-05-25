// Ziel-URL der Counter-API
//const apiUrl = 'http://mate.tbf-blog.com/api/index.php/counter';
const apiUrl = 'http://api.mate.tbf-blog.com/counter';
const jwtToken = 'eyJhbGciOiAiSFMyNTYiLCJ0eXAiOiJKV1QifQ.eyJ1c2VybmFtZSI6Ik1hdHRpIiwicGFzc3dvcmQiOiJHcnVzc1Zvbk1haWtVbmRJbmdlIn0.XNbWBwjFfTI_FtmF5DMyJIj2IMv-fkRN4CO2W5LpqrE';

const dirPathMP3Plus = './mp3/plus'; // Passe den Pfad zum gewünschten Ordner an
const dirPathMP3Minus = 'mp3/minus'; // Passe den Pfad zum gewünschten Ordner an

const mp3FilesPlus = [];
const mp3FilesMinus = []; 

// DOM-Elemente
const counterEl = document.getElementById('counter');
const rankNumberEl = document.getElementById('rank_number');
const rankNameEl = document.getElementById('rank_name');
const increaseBtn = document.getElementById('increase');
const decreaseBtn = document.getElementById('decrease');
const audioIcon = document.getElementById("audioIcon");
const audio = new Audio();

//var audio = document.getElementById("myAudio");
var volumeSlider = document.getElementById("volumeSlider");
var audioControls = document.getElementById("audioControls");

// Überprüfen, ob eine vorherige Lautstärkeeinstellung im localStorage gespeichert ist
if (localStorage.getItem("audioVolume") !== null) {
  var savedVolume = parseFloat(localStorage.getItem("audioVolume"));
  audio.volume = savedVolume;
  volumeSlider.value = savedVolume;
} 

audioControls.addEventListener("mouseenter", function() {
  audioControls.classList.add("active");
});

audioControls.addEventListener("mouseleave", function() {
  audioControls.classList.remove("active");
});

volumeSlider.addEventListener("input", function() {
  //audio.volume = parseFloat(this.value);
  localStorage.setItem("audioVolume", this.value); // Speichert die Lautstärkeeinstellung im localStorage
});


function wackeln() {
	var div = document.getElementById("counter");
	div.classList.add("wackeln");
  
	setTimeout(function() {
		div.classList.remove("wackeln");
	}, 500); // Timeout sollte die Dauer der Animation entsprechen
}

function getMp3FilesPlus() {
  return fetch('getMp3FilesPlus.php')
    .then(response => response.json())
    .then(mp3Files => mp3Files.map(file => `mp3/plus/${file}`));
}

function getMp3FilesMinus() {
  return fetch('getMp3FilesMinus.php')
    .then(response => response.json())
    .then(mp3Files => mp3Files.map(file => `mp3/minus/${file}`));
}

/* async function playSoundPlus() {
	var playButton = document.getElementById("increase");
	const sounds = await getMp3FilesPlus();
	const randomIndex = Math.floor(Math.random() * sounds.length);
	const audio = new Audio(sounds[randomIndex]);
	audio.addEventListener("ended", function() {
		playButton.disabled = false; // Aktiviert den Play-Button, wenn das Audio beendet ist
	});
	audio.play();
	playButton.disabled = true;
} */

async function playSoundPlus() {
	var playButtonIncrease = document.getElementById("increase");
	var playButtonDecrease = document.getElementById("decrease");
	const sounds = await getMp3FilesPlus();
	const randomIndex = Math.floor(Math.random() * sounds.length);
	audio.src = sounds[randomIndex];
	audio.addEventListener("ended", function() {
		playButtonIncrease.disabled = false; // Aktiviert den Play-Button, wenn das Audio beendet ist
		playButtonDecrease.disabled = false; // Aktiviert den Play-Button, wenn das Audio beendet ist
		
	});
	audio.play();
	playButtonIncrease.disabled = true;
	playButtonDecrease.disabled = true;
}

async function playSoundMinus() {
	var playButtonIncrease = document.getElementById("increase");
	var playButtonDecrease = document.getElementById("decrease");	
	const sounds = await getMp3FilesMinus();
	const randomIndex = Math.floor(Math.random() * sounds.length);
	//const audio = new Audio(sounds[randomIndex]);
	audio.src = sounds[randomIndex];
  	audio.addEventListener("ended", function() {
		playButtonIncrease.disabled = false; // Aktiviert den Play-Button, wenn das Audio beendet ist
		playButtonDecrease.disabled = false; // Aktiviert den Play-Button, wenn das Audio beendet ist
	});
	audio.play();
	playButtonIncrease.disabled = true;
	playButtonDecrease.disabled = true;
}

/* function playSound() {
	var sounds = [
		"mp3/Mexican Hat Dance_2.mp3",
		"mp3/badumtss.swf.mp3",
		"mp3/george-micael-wham-careless-whisper-1.mp3",
		"mp3/nein-doch-ohh.mp3",
		"mp3/okeyyy-lets-go.mp3"
		
	];
	var randomIndex = Math.floor(Math.random() * sounds.length);
	var audio = new Audio(sounds[randomIndex]);
	audio.play();
} */

function stopSound() {
	audio.pause();
}

// Funktion, die den Counter aus der API abruft und im DOM aktualisiert
async function updateCounter() {
/* 	sendCommand("getNumberOfAllMate").then(function(response) {
		counterEl.textContent = response;
	}).catch(function(error) {
		console.log("Fehler: " + error);
	}); */
	

	sendCommand("getData").then(function(response) {
		// Verarbeite die Daten aus der Response
		//console.log("response: ");
		//console.log(response);

		let jsonData = JSON.parse(response);

		let countBottles = jsonData["count"];
		let rankNumber = jsonData["rankNumber"];
		let rangUpNummerMin = jsonData["rangMin"];
		let rangUpNummerMax = jsonData["rangMax"];
		let rankName = jsonData["rankName"];
		let rankProgressInPercent = jsonData["rankProgress"];
		let play_rankup_audio = jsonData["playRankUpAudio"];
		let isRankUpAudioPlayed = jsonData["isRankUpPlayed"];
		let audioVolume = jsonData["audioVolume"];
		let last_timestamp = jsonData["last_timestamp"];
		let now_timestamp = jsonData["now_timestamp"];
		
/* 		console.log("countBottles: " + countBottles);
		console.log("rankNumber: " + rankNumber);
		console.log("rangUpNummerMin: " + rangUpNummerMin);
		console.log("rangUpNummerMax: " + rangUpNummerMax);
		console.log("rankName: " + rankName);
		console.log("rankProgressInPercent: " + rankProgressInPercent);
		console.log("play_rankup_audio: " + play_rankup_audio);
		console.log("isRankUpAudioPlayed: " + isRankUpAudioPlayed);
		console.log("audioVolume: " + audioVolume);
		console.log("last_timestamp: " + last_timestamp);
		console.log("now_timestamp: " + now_timestamp); */

		counterEl.textContent = countBottles;
		rankNumberEl.textContent = rankNumber;
		rankNameEl.textContent = rankName;
		updateProgress(rankProgressInPercent);
	  
      // Führe weitere Aktionen aus
      // ...
    }).catch(function(error) {
		console.log("Fehler: " + error);
    });	

}

// Funktion, die den Counter in der API erhöht und im DOM aktualisiert


async function increaseCounter() {
	
	sendCommand("increase").then(function(response) {
		//counterEl.textContent = response;
		updateCounter();
	}).catch(function(error) {
		console.log("Fehler: " + error);
	});
	
	// über den REST-API Link, geht aber nur ohne Header, weil sonst der CORS-Mode blockiert wird
	// also kann auch kein jwt-Token mit geschickt werden
/* 	const response = await fetch(apiUrl + '/increase', { method: 'POST' });
	const responseData = await response.json();
	counterEl.textContent = responseData.count; */

	// https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API/Using_Fetch
	
	// Diser Code geht leider nur im mode: "no-cors", aber in disem mode wird kein Header mitgeschickt
/* 	const data = {"username": "user", "password": "pw"};	
	const response = await fetch(apiUrl + '/increase', {
		method: "POST", // *GET, POST, PUT, DELETE, etc.
		mode: "cors", // no-cors, *cors, same-origin
		cache: "no-cache", // *default, no-cache, reload, force-cache, only-if-cached
		credentials: "same-origin", // include, *same-origin, omit
		headers: {
		  "Content-Type": "application/json",
		  'Authorization': 'Bearer eyJhbGciOiAiSFMyNTYiLCJ0eXAiOiJKV1QifQ.eyJ1c2VybmFtZSI6Ik1hdHRpIiwicGFzc3dvcmQiOiJHcnVzc1Zvbk1haWtVbmRJbmdlIn0.XNbWBwjFfTI_FtmF5DMyJIj2IMv-fkRN4CO2W5LpqrE'
		  // 'Content-Type': 'application/x-www-form-urlencoded',
		},
		redirect: "follow", // manual, *follow, error
		referrerPolicy: "no-referrer", // no-referrer, *no-referrer-when-downgrade, origin, origin-when-cross-origin, same-origin, strict-origin, strict-origin-when-cross-origin, unsafe-url
		body: JSON.stringify(data), // body data type must match "Content-Type" header
	}).then(response => {
	if (!response.ok) {
		throw new Error(response.status);
	}
		return response.json();
	})
	.then(data => {
	  console.log(data);
	})
	.catch(error => {
	  console.error('There was a problem with the fetch operation:', error);
	}); */
} 

// Funktion, die den Counter in der API verringert und im DOM aktualisiert
async function decreaseCounter() {
	sendCommand("decrease").then(function(response) {
		//counterEl.textContent = response;
		updateCounter();
	}).catch(function(error) {
		console.log("Fehler: " + error);
	});
	
	// über den REST-API Link, geht aber nur ohne Header, weil sonst der CORS-Mode blockiert wird
	// also kann auch kein jwt-Token mit geschickt werden
/* 	const response = await fetch(apiUrl + '/decrease', { method: 'POST' });
	const data = await response.json();
	counterEl.textContent = data.count; */
}

function updateProgress(progress) {
    var bar = document.getElementById("bar");
    var label = document.getElementById("label");
    bar.style.width = progress + "%";
    label.innerHTML = progress + "%";

    // Dynamisch ändern der Textfarbe basierend auf der Fortschrittsbreite
    var textColor = "#093385";
    if (progress >= 95) {
        textColor = "#093385";
    } else if (progress >= 90) {
        textColor = "#093385";
    } else if (progress >= 80) {
        textColor = "#093385";
    }
    label.style.color = textColor;
}

function sendCommand(command) {
  return new Promise(function(resolve, reject) {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", "api/functions.php");
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function() {
      if (xhr.readyState === 4) {
        if (xhr.status === 200) {
          var response = xhr.responseText;
          // Hier können Sie mit der Antwort arbeiten
          //console.log(response);
          resolve(response); // Das Ergebnis an die Promise übergeben
        } else {
          // Hier können Sie Fehlerbehandlung durchführen
          console.log("Fehler: " + xhr.statusText);
          reject(xhr.statusText); // Fehler an die Promise übergeben
        }
      }
    };
    xhr.send("action=" + command);
  });
}

// Bei Klick auf den Increase-Button: Counter in API erhöhen und im DOM aktualisieren
increaseBtn.addEventListener('click', async () => {
	await increaseCounter();
});

// Bei Klick auf den Decrease-Button: Counter in API verringern und im DOM aktualisieren
decreaseBtn.addEventListener('click', async () => {
	await decreaseCounter();
});

volumeSlider.addEventListener("input", function() {
  audio.volume = volumeSlider.value;
});

let istGeklickt = false;
audioIcon.addEventListener("click", function() {
 
    // prüfen, ob das Bild bereits geklickt wurde
	if (istGeklickt) {
		// wenn ja, das Bild wieder auf das ursprüngliche Bild ändern
		audioIcon.src = "images/ton_24px.png";
		var savedVolume = parseFloat(localStorage.getItem("audioVolume"));
		audio.volume = savedVolume;
		istGeklickt = false;
	} else {
		// wenn nicht, das Bild auf das neue Bild ändern
		audioIcon.src = "images/kein-ton_24px.png";
		audio.volume = 0.0;
		istGeklickt = true;
	}
});

// Counter beim Laden der Seite aus der API abrufen und im DOM aktualisieren
updateCounter();

// Rufe die updateCounter-Funktion alle 10 Sekunden automatisch auf
setInterval(updateCounter, 5000);
//setInterval(increaseCounter, 5000);