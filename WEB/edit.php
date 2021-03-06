
<?php
session_start();


include('functions/function.php');

redirect_user_if_is_not_log_in();

?>
<!DOCTYPE html>
<html lang="fr">

<?php

$path = "./animations/";

$futureFrameContent = "[]";
$dataArray = array();

if (isset($_GET['f'])) {
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
    }
    fclose($file);
    $futureFrameContent = json_encode($dataArray);
} else if (isset($_POST['frames'])) {
    $futureFrameContent = $_POST['frames'];
    $futureFrameContentZYX = 1;
}

?>

<head>
    <link rel="apple-touch-icon" sizes="180x180" href="./favicon/apple-touch-icon.png">
    <link rel="icon" type="image/png" sizes="32x32" href="./favicon/favicon-32x32.png">
    <link rel="icon" type="image/png" sizes="16x16" href="./favicon/favicon-16x16.png">
    <link rel="manifest" href="./favicon/site.webmanifest">

    <title>LEDCube : ??dition</title>
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

<body onload="init()">

    <div class="header">
        <h1 style="text-align: left;">LED CUBE</h1>
    </div>
    <div class="content">


        <div class="contentviewer">
            <h1 class="title">Gestion du fichier</h1>
            <input oninput="clearTimeout(isSavableTimeout); isSavableTimeout = setTimeout(isSavable, 500)" onchange="isSavable()" id="fileName" class="mediummargin hugetopmargin" type="text" placeholder="Nom du fichier" value=<?php echo "\"" . (isset($_GET['f']) ? $_GET['f'] : '') . "\"" ?>>
            <button class="mediummargin purple" id="playAnimation" onclick="sendAnimation(true)" disabled>Jouer sur le LEDCube</button>
            <button class="mediummargin" id="saveButton" onclick="sendAnimation(false)" disabled>Enregistrer</button>
            <button class="red mediummargin" onclick="reset()">Reset</button>
        </div>

        <div class="contentviewer fixedwidth">
            <h1 class="title">Gestion des frames</h1>
            <div class="flexRow">
                <p>Nombre de frames:</p>
                <p id="frameNumber">1</p>
            </div>
            <div class="flexRow">
                <p>Temps d'une frame (en ms)</p>
                <input id="frameTime" onchange="timeChanged()" min="1" type="number" value="<?php echo isset($time) ? $time : "500" ?>">
            </div>
            <div class="flexRow">
                <button onclick="addframebefore()">Ajouter une frame avant</button>
                <input min="1" max="1" id="numframebefore" value="1" type="number">
            </div>
            <div class="flexRow">
                <button onclick="addframeafter()">Ajouter une frame apr??s</button>
                <input min="1" max="1" id="numframeafter" value="1" type="number">
            </div>
            <p>Aller ?? la frame</p>
            <div class="flexRow">

                <button onclick="previousframe()">&lt;</button>
                <input id="frameRange" oninput="gotoframe()" type="range" min="1" max="1" value="1">
                <button onclick="nextframe()">&gt;</button>
                <input class="inputnumfixedwidth" id="frameInput" type="number" oninput="document.getElementById('frameRange').value = Math.max(this.value, 1); gotoframe()" min="1" max="1" value="1">
            </div>
            <div class="flexRow">
                <button class="mediumtopmargin red" onclick="removeframe()">Supprimer la frame</button>
                <button class="mediumtopmargin purple" onclick="copy3D()">Copier</button>
                <button class="mediumtopmargin purple" onclick="paste3D()" id="paste3D" disabled>Coller</button>
            </div>
        </div>
        <div class="contentviewer">
            <h1 class="title">Viewer 3D</h1>

            <div id="container"></div>
        </div>

        <div class="contentviewer fixedwidth">
            <h1 class="title">Choix de la matrice 2D</h1>
            <p class="hugetopmargin">Axe:</p>
            <div class="flexRow">
                <input type="radio" id="X" name="axe" value="X" checked><label for="X">X</label>
                <input type="radio" id="Y" name="axe" value="Y"> <label for="Y">Y</label>
                <input type="radio" id="Z" name="axe" value="Z"><label for="Z">Z</label>
            </div>
            <p calss="hugetopmargin">Plan:</p>
            <input oninput="SelectPlan()" id="planNumber" class="legendedRange" list="tickmarks" type="range" min="1" max="8" step="1" value="1">
            <datalist id="tickmarks">
                <option value="1" label="1"></option>
                <option value="2" label="2"></option>
                <option value="3" label="3"></option>
                <option value="4" label="4"></option>
                <option value="5" label="5"></option>
                <option value="6" label="6"></option>
                <option value="7" label="7"></option>
                <option value="8" label="8"></option>

            </datalist>
            <button class="hugemargin" onclick="clear3DViewPlan()">Effacer le marquage sur le viewer 3D</button>
        </div>

        <div class="contentviewer fixedwidth black">
            <h1 class='title'>Matrice 2D</h1>
            <div id="matrix">

            </div>
        </div>

        <div class="contentviewer fixedwidth">
            <h1 class="title">Choix de la couleur</h1>
            <div class="flexRow hugetopmargin">
                <button id="redButton" class="colorButton grey" onclick="changeColor(this,'red');setColor()">Rouge</button>
                <button id="greenButton" class="colorButton grey" onclick="changeColor(this,'green');setColor()">Vert</button>
                <button id="blueButton" class="colorButton grey" onclick="changeColor(this,'blue');setColor()">Bleu</button>
            </div>
            <input class="hugemargin" type="color" id="pickColor" onchange="save=true;setColor()">
            <div class="flexRow">
                <button class="mediummargin purple" onclick="copy2D()">Copier la matrice</button>
                <button class="mediummargin purple" id="paste2D" onclick="paste2D()" disabled>Coller la matrice</button>
            </div>
        </div>
    </div>
    <table id="contentnotifs"></table>
</body>


</html>