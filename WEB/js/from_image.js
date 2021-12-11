var byteArrayImage;

/**
 * bytesToUint32 converts the given bytes from the "start" to "count" to an unsigned 32 bit integer.
 * @param {Uint8Array} byteArray
 * @param {number} start
 * @param {number} count
 */
function bytesToUint32(byteArray, start, count) {
	let n = 0;
	for (let i = 0; i < count; ++i) {
		n += byteArray[start + i] << (8 * i);
	}
	return n;
}

function generateLayer(w, h) {
	const layer = new Array(h);
	for (var y = 0; y < h; y++) {
		layer[y] = new Array(w);
		layer[y].fill(0);
	}
	return layer;
}

/**
 * @param {Uint8Array} byteArray
 */
function decodeBMP(byteArray) {
	const data_head = byteArray.subarray(0, 54); // offset est à 54

	// 2 octets pour le nombre magique
	// const magic = bytesToUint32(data_head, 0, 2);

	// 4 octets pour la taille en octets
	// const size = bytesToUint32(data_head, 2, 4);

	// 4 octets pour l'adresse de départ du contenu (54)
	// const offset = bytesToUint32(data_head, 10, 4);

	// 4 pour la largeur et 4 pour la hauteur
	const w = bytesToUint32(data_head, 18, 4);
	const h = bytesToUint32(data_head, 22, 4);

	// couleur : 255 255 255 => 3 bytes par pixel (24 bits)
	const raw_data_size = w * h * 3;
	const raw_data = byteArray.subarray(54, 54 + raw_data_size);
	const data = generateLayer(w, h);
	for (var y = 0; y < h; y++) {
		for (var x = 0; x < w; x++) {
			const i = (y * w + x) * 3;
			data[y][x] = bytesToUint32(raw_data, i, 3); // RGB
		}
	}
	return data;
}

/**
 * @param {File} file
 * @return {Promise<number[][]>} Matrice 2D de l'image
 */
function readImage(file) {
	if (file.name.endsWith('.bmp')) {
		console.log("Traitement de l'image BMP");
		return decodeBMP(byteArrayImage);
	} else {
		console.error('Image non valide, veuillez choisir une image bmp');
		alert("L'image n'est pas valide");
	}
}

function convertMatriceXZYtoXYZ(matrice) {
	const data = new Array(8);
	for (var x = 0; x < 8; x++) {
		data[x] = new Array(8);
		for (var y = 0; y < 8; y++) {
			data[x][y] = new Array(8);
			for (var z = 0; z < 8; z++) {
				data[x][y][z] = matrice[x][7 - z][y];
			}
		}
	}
	return data;
}

function sendMatrice() {
	const image_input = document.querySelector('#image_input');

	/**
	 * @type {FileList}
	 */
	const files = image_input.files;
	const image = readImage(files[0]);

	console.log('Image :', image);

	const frame = new Array(8);
	frame.fill(image, 0, 1);
	frame.fill(generateLayer(8, 8), 1, 8);
	const frames = new Array(convertMatriceXZYtoXYZ(frame));
	submitPattern(frames);
}

/**
 * @param {number[][][][]} frames
 */
function submitPattern(frames) {
	var f = document.createElement('form');
	f.action = './';
	f.method = 'POST';

	var inputF = document.createElement('input');
	inputF.type = 'hidden';
	inputF.name = 'frames';
	inputF.value = JSON.stringify(frames);
	f.appendChild(inputF);

	document.body.appendChild(f);
	f.submit();
}

window.addEventListener('load', () => {
	const image_input = document.querySelector('#image_input');
	const display = document.querySelector('#display_image');
	image_input.addEventListener('change', function () {
		const reader = new FileReader();
		reader.addEventListener('load', () => {
			const arrayBuffer = reader.result;
			byteArrayImage = new Uint8Array(arrayBuffer);

			display.src = URL.createObjectURL(new Blob([byteArrayImage.buffer], { type: 'image' }));
			document.querySelector('#send_matrice').removeAttribute('disabled');

			//sendMatrice();
		});
		reader.readAsArrayBuffer(this.files[0]);
	});
});
