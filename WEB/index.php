<?php
    session_start();

    include('functions/function.php');

    redirect_user_if_is_not_log_in();
?>

<!DOCTYPE html>
<html lang="fr">

<head>
    
    <link rel="apple-touch-icon" sizes="180x180" href="./favicon/apple-touch-icon.png">
    <link rel="icon" type="image/png" sizes="32x32" href="./favicon/favicon-32x32.png">
    <link rel="icon" type="image/png" sizes="16x16" href="./favicon/favicon-16x16.png">
    <link rel="manifest" href="./favicon/site.webmanifest">

    <title>LEDCube</title>
    <meta http-equiv="content-type" content="text/html;charset=utf-8" />
    <meta name="viewport" content="width=device-width, user-scalable=no, minimum-scale=1.0, maximum-scale=1.0">
    <link rel="stylesheet" href="css/Main.css">
    <script src="js/Request.js"></script>
    <style>
        div.buttons {
            display: flex;
            height:max-content;
            align-content:center;
            align-items: center;
            align-self: center;
            vertical-align: middle;
            flex-flow: wrap;
            max-width: 500px;
            margin-top: 50px;
        }

        button {
            display: flex;
            align-items: center;
            justify-content: center;
            flex-grow: 2;
            align-content: center;
            align-items: center;
            width: 100%;
        }
    </style>
</head>


<body>

    <div class="header" style="align-items: baseline;">
        <h1 style="text-align: left;">LED CUBE</h1>
    </div>

    <div class = buttons>
        <button onclick="window.location.href = 'openfile.php'" class=" purple">Ouvrir une animation existante</button>
        <button onclick="window.location.href = 'edit.php'" class=" purple">Créer une animation</button>
        <button onclick="window.location.href = 'from_image.php'" class=" purple">Créer une animation à partir d'une image</button>
        <button onclick="playAnimationAleat()" class=" purple">Animations aléatoires</button>
        <button onclick="stopAnnimation()" class=" red">Arrêter l'animation</button>
    </div>
    <table id="contentnotifs"></table>
</body>


</html>