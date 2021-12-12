<!DOCTYPE html>
<html lang="fr">

<head>
	<meta charset='utf-8'>
	<meta http-equiv='X-UA-Compatible' content='IE=edge'>
	<title>Image To Pattern</title>
	<meta name='viewport' content='width=device-width, initial-scale=1'>
	<link rel='stylesheet' type='text/css' media='screen' href='css/from_image.css'>
	<link rel='stylesheet' type='text/css' media='screen' href='css/Main.css'>
	<script src='js/from_image.js'></script>
	<script src='js/math.js'></script>
</head>

<body>

	<div>
		<label for="image_input">Choisir une image</label>
		<input type="file" name="image_input" id="image_input" multiple accept="image/*">
	</div>

	<div id="display">
		<div id="div_frames">
			<canvas id="display_frames"></canvas>
		</div>
		<input type="range" min="1" max="1" value="1" id="frame_range" hidden />
	</div>


	<div id="control">
		<p>Type de remplissage</p>
		<div class="flexRow">
			<input type="radio" oninput="onFillControlChanged()" name="fill_type" value="fill_one_layer" id="fill_one_layer" checked>
			<label for="fill_one_layer">Uniquement la première couche</label>
		</div>
		<div class="flexRow">
			<input type="radio" oninput="onFillControlChanged()" name="fill_type" value="fill_extend" id="fill_extend">
			<label for="fill_extend">Prolonger sur toutes les couches</label>
		</div>
		<div class="flexRow">
			<input type="radio" oninput="onFillControlChanged()" name="fill_type" value="fill_with_formule" id="fill_with_formule">
			<label for="fill_with_formule">Utiliser une formule</label>
			<input type="text" name="fill_formule" id="fill_formule" title="f(x,y,z,t)=#FFFFFF
			x : profondeur
			y : de gauche à droite
			z : de haut en bas
			t : temps ou index d'image" oninput="onFillFormuleChanged()" value="f(x,y,z,t)=(x==0) && img(y,z,t)">
		</div>
		<div class="flexRow hides fakeRadioInput" id="fill_time_control" hidden>
			<label for="fill_time">Temps maximum</label>
			<input type="number" class="inputnumfixedwidth" min="1" value="1" name="fill_time" id="fill_time" title="temps Maximum ou nombre d'images" placeholder="tMax">
		</div>

	</div>

	<button id="send_pattern" disabled title="Choisissez une image ou une formule sans img()">Envoyer au ledcube</button>

</body>

</html>