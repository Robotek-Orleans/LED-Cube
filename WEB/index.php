<!DOCTYPE html>
<html lang="fr">

<?php

$path = "./animations/";

$dataArray = array();

 if(isset($_GET['f'])){
    $file = fopen($path.$_GET['f'], "r") or die("Unable to open file!");

    $frames = fgets($file);
    //print_r($frames);
    $time = fgets($file);

    if ($file) {
        for ($i=0;$i<$frames;$i++) {
            $dataArray[$i] = array();
            for ($j=0;$j<8;$j++) {
                $dataArray[$i][$j] = array();
                for ($k=0;$k<8;$k++) {
                    $dataArray[$i][$j][$k] = array();
                    for ($l=0;$l<8;$l++) {
                        $red    = dechex(intval(fgets($file)));
                        $green  = dechex(intval(fgets($file)));
                        $blue   = dechex(intval(fgets($file)));
                        array_push($dataArray[$i][$j][$k][$l],"#".$red.$green.$blue);
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
            if(isset($_GET['f'])){
                echo "var framecontent = ".json_encode($dataArray).";";
            }else{
                echo "var framecontent = [];";
                echo "addframe(0);";
            }
        ?>
    </script>
</head>

<body onload="init()">

    <div class="header">
        <h1 style="text-align: left;">LED CUBE</h1>
    </div>
    <div class="content">


        <div class="contentviewer">
            <h1 class="title">Gestion du fichier</h1>
            <input class="hugemargin" type="text" placeholder="Nom du fichier">
            <button class="hugemargin mediumtopmargin" onclick="sendAnimation()" disabled>Enregistrer</button>
            <button class="hugemargin red mediumtopmargin" onclick="reset()">Reset</button>
        </div>

        <div class="contentviewer fixedwidth">
            <h1 class="title">Gestion des frames</h1>
            <div class="flexRow">
                <p>Nombre de frames:</p>
                <p id="frameNumber">1</p>
            </div>
            <div class="flexRow">
                <p>Temps d'une frame (en ms)</p>
                <input min="1" value="500" type="number">
            </div>
            <div class="flexRow">
                <button onclick="addframebefore()">Ajouter une frame avant</button>
                <input min="1" max="1" id="numframebefore" value="1" type="number">
            </div>
            <div class="flexRow">
                <button onclick="addframeafter()">Ajouter une frame après</button>
                <input min="1" max="1" id="numframeafter" value="1" type="number">
            </div>
            <p>Aller à la frame</p>
            <div class="flexRow">

                <button onclick="previousframe()">&lt;</button>
                <input id="frameRange" onchange="gotoframe()" oninput="document.getElementById('frameInput').value = this.value" type="range" min="1" max="1" value="1">
                <button onclick="nextframe()">&gt;</button>
                <input class="inputnumfixedwidth" id="frameInput" type="number" min="1" value="1">
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
            <input class="hugemargin" type="color" id="pickColor" disabled>
            <div class="flexRow">
                <button class="mediummargin purple" onclick="copy2D()">Copier la matrice</button>
                <button class="mediummargin purple" id="paste2D" onclick="paste2D()" disabled>Coller la matrice</button>
            </div>
        </div>
    </div>
    <table id="contentnotifs"></table>
</body>


</html>