
<?php
session_start();


include('functions/function.php');

redirect_user_if_is_not_log_in();


if(isset($_GET['delete'])){
    $_GET['delete'] = preg_replace('/[^a-zA-Z0-9]/', "_", $_GET['delete']);
    $path = "./animations/";
    unlink($path.$_GET['delete']);
    header("Location: openfile.php");
    die();
}

?>


<!DOCTYPE html>
<html lang="fr">

<head>
    <link rel="apple-touch-icon" sizes="180x180" href="./favicon/apple-touch-icon.png">
    <link rel="icon" type="image/png" sizes="32x32" href="./favicon/favicon-32x32.png">
    <link rel="icon" type="image/png" sizes="16x16" href="./favicon/favicon-16x16.png">
    <link rel="manifest" href="./favicon/site.webmanifest">

    <title>LEDCube : ouvrir</title>
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
        <button class="stopButton red" onclick="stopAnnimation()"></button>
        
    </div>
    <div class="content mainContentAnim">


        <div class="contentviewer">

            <?php
            $folder_path = "animations/";
            if (is_dir($folder_path)) {
                $files = opendir($folder_path); 
                if ($files) {

                    $files_names=array();
                    while (($gfg_subfolder = readdir($files)) !== FALSE) {
                        if ($gfg_subfolder != '.' && $gfg_subfolder != '..') {
                            if ($gfg_subfolder != ".htaccess") {
                                array_push($files_names,$gfg_subfolder);
                            }
                        }
                    }

                    sort($files_names, SORT_NATURAL | SORT_FLAG_CASE);

                    for ($i=0;$i<count($files_names);$i++) {
                        $gfg_subfolder = $files_names[$i];
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
            ?>

        </div>

    </div>

    <div id="modal" class="contentviewer modal-content">
        <button onclick="closeModal()" class="close-modal">X</button>
        <div style="top: 0px;height: 100%;" id="container"></div>
    </div>
    <table id="contentnotifs"></table>
</body>


</html>