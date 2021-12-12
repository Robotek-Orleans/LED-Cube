<!DOCTYPE html>
<html lang="fr">

<?php

$path = "./animations/";

$dataArray = array();

if (isset($_GET['f'])) {
    $file = fopen($path . $_GET['f'], "r") or die("Unable to open file!");
    $frames = intval(fgets($file));
    $time = intval(fgets($file));

    if ($file) {
        for ($i = 0; $i < $frames; $i++) {
            $dataArray[$i] = array();
            for ($j = 0; $j < 8; $j++) {
                $dataArray[$i][$j] = array();
                for ($k = 0; $k < 8; $k++) {
                    $dataArray[$i][$j][$k] = array();
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
                        $dataArray[$i][$j][$k][$l] = "#" . $red . $green . $blue;
                    }
                }
            }
            // process the line read.
        }
    } else {
        // error opening the file.
    }
    fclose($file);
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
        var selectedPlanDirection = "X"; // X , Y ou Z
        var selectedPlanNumber = 1; // entre 1 et 8
        var selectedFrame = 1; // entre 1 et N frame
        var selected2D = []; // tableau en 2D avec des valeurs entre 0 et 7
        var shiftpressed = false;
        var copied2D = [];
        var copied3D = [];
        <?php
        if (isset($_GET['f'])) {
            echo "var framecontent = " . json_encode($dataArray) . ";";
        } else {
            echo "var framecontent = [];";
            echo "addframe(0);";
        }
        ?>
    </script>
    <style>

    </style>
</head>

<body onload="init()">

    <div class="header">
        <h1 style="text-align: left;">LED CUBE</h1>
    </div>
    <div class="content">


        <div class="contentviewer morewith">
            <h1 class="title">Gestion du fichier</h1>





            <?php
            $gfg_folderpath = "animations/";
            // CHECKING WHETHER PATH IS A DIRECTORY OR NOT
            if (is_dir($gfg_folderpath)) {
                // GETTING INTO DIRECTORY
                $files = opendir($gfg_folderpath); {
                    // CHECKING FOR SMOOTH OPENING OF DIRECTORY
                    if ($files) {
                        //READING NAMES OF EACH ELEMENT INSIDE THE DIRECTORY
                        $i = 0;
                        while (($gfg_subfolder = readdir($files)) !== FALSE) {
                            // CHECKING FOR FILENAME ERRORS
                            if ($gfg_subfolder != '.' && $gfg_subfolder != '..') {
                                $i = $i + 1;
                                if ($gfg_subfolder != ".htaccess") {
                                    echo "<br>  
                    <button id='myBtn' onClick='getFileName()' class='$gfg_subfolder'>" . $gfg_subfolder . "</button>
                    <hr class='morewith'>
                    
                    --<br>
                    <!--input onchange='isSavable()' id='fileName' class='hugemargin' type='text' placeholder='Nom du fichier' value='$gfg_subfolder'-->
          

                    ";
                                }


                                $dirpath = "animations/" . $gfg_subfolder . "/";
                                // GETTING INSIDE EACH SUBFOLDERS
                                if (is_dir($dirpath)) {
                                    $file = opendir($dirpath); {
                                        if ($file) {
                                            //READING NAMES OF EACH FILE INSIDE SUBFOLDERS
                                            while (($gfg_filename = readdir($file)) !== FALSE) {
                                                if ($gfg_filename != '.' && $gfg_filename != '..') {
                                                    echo $gfg_filename . "<br>";
                                                }
                                            }
                                        }
                                    }
                                }
                                echo "<br>";
                            }
                        }
                    }
                }
            }
            ?>

        </div>


    </div>


    <!-- The Modal -->
    <div id="myModal" class="modal contentviewer">

        <!-- Modal content -->
        <div class="modal-content">
            <span class="close">&times;</span>



            <div class="contentviewer">

                <div id="container"></div>
            </div>
            <div>
                <a href="#" class="link">Vusialiser</button>
                    <a id="edit" href="" class="link">Editer</a>
            </div>


        </div>

    </div>




    </div>
    </div>
    <table id="contentnotifs"></table>

    <script>
        // Get the modal
        var modal = document.getElementById("myModal");

        // Get the button that opens the modal
        var btn = document.getElementById("myBtn");

        // Get the <span> element that closes the modal
        var span = document.getElementsByClassName("close")[0];

        // When the user clicks the button, open the modal 
        btn.onclick = function() {
            modal.style.display = "block";

            // var classContent = btn.getAttibute('class');

            var x = this.getAttribute("class");

            var loc = "index.php?f=" + x;


            document.getElementById("edit").href = loc;

            console.log(loc);


        }

        // When the user clicks on <span> (x), close the modal
        span.onclick = function() {
            modal.style.display = "none";
        }

        // When the user clicks anywhere outside of the modal, close it
        window.onclick = function(event) {
            if (event.target == modal) {
                modal.style.display = "none";
            }
        }
    </script>
</body>


</html>