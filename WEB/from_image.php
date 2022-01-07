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

	<meta charset='utf-8'>
	<meta http-equiv='X-UA-Compatible' content='IE=edge'>
	<title>LEDCube : image / fonction</title>
	<meta name='viewport' content='width=device-width, initial-scale=1'>
	<link rel='stylesheet' type='text/css' media='screen' href='css/Main.css'>
	<link rel='stylesheet' type='text/css' media='screen' href='css/from_image.css'>
	<script src='js/from_image.js'></script>
	<script src='JigMath/jigmath.js'></script>
</head>

<body>

	<div class="header">
		<h1 style="text-align: left;">LED CUBE</h1>
	</div>
	<div class="content">

		<div class="contentviewer">
			<h1 class="title">Gestion des images</h1>
			<button class="mediummargin" onclick="document.querySelector('#image_input')?.click()">Sélectionner une image</button>
			<input type="file" name="image_input" id="image_input" multiple accept="image/*" hidden>
			<button class="mediummargin" id="send_pattern" title="Choisissez une image ou une formule sans img()" disabled>Exporter vers LED CUBE</button>

			<input type="range" class="mediummargin" min="1" max="1" value="1" id="frame_range" disabled />
			<div id="remplissage" class="flexRow">
				<p>Type de remplissage</p>
				<div class="flexRow">
					<input type="radio" name="fill_type" value="fill_one_layer" id="fill_one_layer" checked>
					<label for="fill_one_layer">Uniquement la première couche</label>
				</div>
				<div class="flexRow">
					<input type="radio" name="fill_type" value="fill_extend" id="fill_extend">
					<label for="fill_extend">Prolonger sur toutes les couches</label>
				</div>
				<div class="flexRow">
					<input type="radio" name="fill_type" value="fill_with_formule" id="fill_with_formule">
					<label for="fill_with_formule">Utiliser une formule</label>
				</div>
			</div>
		</div>

		<div class="contentviewer fixedwidth">
			<h1 class="title">Viewer 2D</h1>
			<div id="front_view" class="flexRow">
				<canvas id="display_frames" width=8 height=8></canvas>
			</div>
		</div>


		<div class="contentviewer" id="config_formule" disabled>
			<h1 class="title">Formule</h1>
			<div class="flexRow">
				<textarea class="mediummargin" name="fill_formule" id="fill_formule" title="f(x,y,z,t)=#FFFFFF
			x : profondeur
			y : de gauche à droite
			z : de haut en bas
			t : temps ou index d'image" oninput="onFillFormuleChanged()">f(x,y,z,t)=(x==0) && img(y,z,t)</textarea>
				<div class="mediummargin">
					<p>Temps maximum ou nombre d'images</p>
					<input type="number" class="inputnumfixedwidth" min="1" value="1" name="fill_tmax" id="fill_tmax" title="tMax">
				</div>
			</div>
			<div>
				<a target="_blank" href="https://github.com/Robotek-Orleans/LED-Cube/blob/main/WEB/math.md">
					<p>Cliquez ici pour accéder à la docummentation</p>
				</a>
			</div>
		</div>
	</div>
	<div class="splash_screen">
		<div class="send_progress" id="send_progress">
			<p class="chargement">15/160</p>
			<div class="barre"><span class="barre_chargee"></span></div>
		</div>
	</div>
</body>

</html>