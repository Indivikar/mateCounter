<?php
$dir = './mp3/minus/';
$mp3Files = array();
if (is_dir($dir)){
  if ($dh = opendir($dir)){
    while (($file = readdir($dh)) !== false){
      if (strpos($file, '.mp3') !== false) {
        $mp3Files[] = $file;
      }
    }
    closedir($dh);
  }
}

echo json_encode($mp3Files);
?>