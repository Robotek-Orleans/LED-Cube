<!DOCTYPE html>
<html lang="fr">

<?php

$path = "./animations/";

$futureFrameContent = "[]";
$dataArray = array();

$_GET['f'] = "Robotek_anim";
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
}
fclose($file);
$futureFrameContent = convertArray($dataArray);


function convertArray($array)
{
    $result = "[";
    foreach ($array as $i) {
        $result = $result . " [";
        foreach ($i as $j) {
            $result = $result . " [";
            foreach ($j as $k) {
                $result = $result . " [";
                foreach ($k as $l) {
                    $result = $result . " \"" . $l . "\" ,";
                }
                $result = $result . " ],";
            }
            $result = $result . " ],";
        }
        $result = $result . " ],";
    }
    return $result . " ]";
}

?>

<head>
    <title>LED cube IHM</title>
    <meta http-equiv="content-type" content="text/html;charset=utf-8" />
    <meta name="viewport" content="width=device-width, user-scalable=no, minimum-scale=1.0, maximum-scale=1.0">
    <link rel="stylesheet" href="css/Main.css">

    <script src="js/three.min.js"></script>
    <script src="js/Detector.js"></script>
    <script src="js/OrbitControls.js"></script>
    <script src="js/Viewerscript.js"></script>
    <script src="js/Main.js"></script>
    <script src="js/Request.js"></script>
    <script>
        var save = <?php echo isset($_POST['frames']) ? "true" : "false" ?>;
        var selectedPlanDirection = "X"; // X , Y ou Z
        var selectedPlanNumber = 1; // entre 1 et 8
        var selectedFrame = 1; // entre 1 et N frame
        var selected2D = []; // tableau en 2D avec des valeurs entre 0 et 7
        var shiftpressed = false;
        var copied2D = [];
        var copied3D = [];
        var framecontent = <?php echo $futureFrameContent ?>;
        for (var t = 0; t < framecontent.length; t++)
            for (var z = 0; z < framecontent[t].length; z++)
                for (var y = 0; y < framecontent[t][z].length; y++)
                    for (var x = 0; x < framecontent[t][z][y].length; x++) {
                        if (typeof(framecontent[t][z][y][x]) === "number") {
                            var value = framecontent[t][z][y][x].toString(16);
                            while (value.length < 6) value = "0" + value;
                            framecontent[t][z][y][x] = "#" + value;
                        }
                    }
        var isSavableTimeout;
    </script>
</head>

<body onload="initAnim()">

    <div class="header">
        <h1 style="text-align: left;">LED CUBE</h1>
    </div>
    <div class="content mainContentAnim">


        <div class="contentviewer">

            <?php
            $gfg_folderpath = "animations/";
            // CHECKING WHETHER PATH IS A DIRECTORY OR NOT
            if (is_dir($gfg_folderpath)) {
                // GETTING INTO DIRECTORY
                $files = opendir($gfg_folderpath); {
                    // CHECKING FOR SMOOTH OPENING OF DIRECTORY
                    if ($files) {
                        //READING NAMES OF EACH ELEMENT INSIDE THE DIRECTORY
                        //$i = 0;
                        while (($gfg_subfolder = readdir($files)) !== FALSE) {
                            // CHECKING FOR FILENAME ERRORS
                            if ($gfg_subfolder != '.' && $gfg_subfolder != '..') {
                                //$i = $i + 1;
                                if ($gfg_subfolder != ".htaccess") {
                                    echo "<div class=\"animContent\">";
                                    echo "<div onClick='showAnimation(\"" . $gfg_subfolder . "\")' class=\"contentTitleAnim\"><p class=\"titleAnim\">" . $gfg_subfolder . "</p>";
                                    echo "</div>";
                                    echo "<button  onclick=\"editAnim('" . $gfg_subfolder . "')\">Edit</button><button onclick=\"playAnimationParam('" . $gfg_subfolder . "')\" >Play</button>";
                                    echo "</div>";
                                }
                            }
                        }
                    }
                }
            }
            ?>

        </div>

    </div>

    <div id="modal" class="contentviewer modal-content">
        <button onclick="closeModal()" class="close-modal">X</button>
        <div style="top: 0px;height: 100%;" id="container"></div>
    </div>
    <table id="contentnotifs"></table>

    <script>
        var selectedAnimation = "";

        // Get the modal
        var modal = document.getElementById("modal");

        // Get the button that opens the modal
        var content = document.getElementsByClassName("content")[0];

        // Get the <span> element that closes the modal
        var span = document.getElementsByClassName("close-modal")[0];

        // When the user clicks the button, open the modal 
        function showAnimation(animName) {
            modal.style.display = "block";

            // var classContent = btn.getAttibute('class');

            //var x = this.getAttribute("class");

            //var loc = "index.php?f=" + x;

            //document.getElementById("edit").href = "index.php?f=" + animName;
            //document.getElementById("play").href = "execute.php?f=" + animName;

            //console.log(loc);


        }

        // When the user clicks anywhere outside of the modal, close it
        function closeModal() {
            modal.style.display = "none";
        }


        function editAnim(animation) {
            window.location.href = "/index.php?f=" + animation
        }
    </script>
</body>


</html>