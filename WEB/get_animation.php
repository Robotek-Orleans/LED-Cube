<?php
session_start();


include('functions/function.php');

redirect_user_if_is_not_log_in();


$path = "./animations/";

$futureFrameContent = "[]";
$dataArray = array();

if (isset($_GET['f'])) {
    http_response_code(200);
    $_GET['f'] = preg_replace('/[^a-zA-Z0-9]/', "_", $_GET['f']);
    $file = fopen($path . $_GET['f'], "r") or die("Unable to open file!");
    $frames = intval(fgets($file));
    $time = intval(fgets($file));

    for ($i = 0; $i < $frames; $i++) {
        $dataArray[$i] = array();
        for ($j = 0; $j < 8; $j++) {
            $dataArray[$i][$j] = array();
            for ($k = 0; $k < 8; $k++) {
                $dataArray[$i][$j][$k] = array();
                for ($l = 0; $l < 8; $l++) {
                    $dataArray[$i][$j][$k][$l] = "#000000";
                }
            }
        }
        // process the line read.
    }

    if ($file) {
        for ($i = 0; $i < $frames; $i++) {
            for ($j = 0; $j < 8; $j++) {
                for ($k = 0; $k < 8; $k++) {
                    for ($l = 0; $l < 8; $l++) {
                        $red    = dechex(intval(fgets($file)));
                        if (strlen($red) < 2) {
                            $red = '0' . $red;
                        }
                        $green  = dechex(intval(fgets($file)));
                        if (strlen($green) < 2) {
                            $green = '0' . $green;
                        }
                        $blue   = dechex(intval(fgets($file)));
                        if (strlen($blue) < 2) {
                            $blue = '0' . $blue;
                        }
                        $dataArray[$i][7 - $l][$k][7 - $j] = "#" . $red . $green . $blue;
                    }
                }
            }
            // process the line read.
        }
    } else {
        // error opening the file.
        http_response_code(400);
        die('Error opening file');
    }
    fclose($file);
    $sended['data'] = $dataArray;
    $sended['time'] = $time;

    echo json_encode($sended);
}else{
    http_response_code(400);
    echo "Bad request";
}



?>
