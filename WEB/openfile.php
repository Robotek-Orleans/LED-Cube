
<?php

if(isset($_GET['delete'])){
    $path = "./animations/";
    unlink($path.$_GET['delete']);
    header("Location: /openfile.php");
    die();
}

?>


<!DOCTYPE html>
<html lang="fr">

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
        var framecontent = [];
        var frametime = 500;
        var animation;
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
                    
    </script>
</head>

<body onload="initAnim()">

    <div class="header">
        <h1 style="text-align: left;">LED CUBE</h1>
        <button class="stopButton red" onclick="playAnimationParam('')"></button>
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
                        while (($gfg_subfolder = readdir($files)) !== FALSE) {
                            // CHECKING FOR FILENAME ERRORS
                            if ($gfg_subfolder != '.' && $gfg_subfolder != '..') {
                                if ($gfg_subfolder != ".htaccess") {
                                    echo "<div class=\"animContent\">";
                                    echo "<div onClick='getAnimation(\"" . $gfg_subfolder . "\")' class=\"contentTitleAnim\"><p class=\"titleAnim\">" . $gfg_subfolder . "</p>";
                                    echo "</div>";
                                    echo "<button class=\"editButton\" onclick=\"editAnim('" . $gfg_subfolder . "')\"></button>";
                                    echo "<button class=\"playButton purple\" onclick=\"playAnimationParam('" . $gfg_subfolder . "')\" ></button>";
                                    echo "<button class=\"deleteButton red\" onclick=\"deleteAnim('" . $gfg_subfolder . "')\"></button>";
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


        
    </script>
</body>


</html>