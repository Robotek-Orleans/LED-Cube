/**
 * @type {Map<File,Image8x8>}
 */
var images = new Map();

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

/**
 * @return {number[][]}
 */
function generateLayer() {
	const layer = new Array(8);
	for (var y = 0; y < 8; y++) {
		layer[y] = new Array(8);
		layer[y].fill(0);
	}
	return layer;
}

/**
 * @return {number[][][]}
 */
function generateFrame() {
	const frame = new Array(8);
	for (let z = 0; z < 8; z++) {
		frame[z] = generateLayer();
	}
	return frame;
}

class Image8x8 {
	/**
	 * @type {number[][]}
	 */
	pixels;

	/**
	 * @param {number[][]}
	 */
	constructor(pixels) {
		this.pixels = pixels ?? generateLayer();
	}

	/**
	 * @param {number} x
	 * @param {number} y
	 * @param {number} color RGB
	 */
	setPixel(x, y, color) {
		this.pixels[y][x] = color;
	}

	/**
	 * @param {number} x
	 * @param {number} y
	 * @return {number} color RGB
	 */
	getPixel(x, y) {
		if (typeof x !== 'number' || typeof y !== 'number' || x < 0 || 8 <= x || y < 0 || 8 <= y) {
			throw new Error(`Invalid position of Image8x8 (${x},${y})`);
		}
		return this.pixels[y][x];
	}

	/**
	 * @param {ImageData} imageData
	 */
	static decodeImageData(imageData) {
		const scale_x = 8 / imageData.width;
		const scale_y = 8 / imageData.height;

		var data = new Array(8);
		for (let y8 = 0; y8 < 8; y8++) {
			data[y8] = new Array(8);
			var yMin = Math.floor(y8 / scale_y);
			var yMax = Math.ceil((y8 + 1) / scale_y);
			for (let x8 = 0; x8 < 8; x8++) {
				data[y8][x8] = new Array(4).fill(0);
				var xMin = Math.floor(x8 / scale_x);
				var xMax = Math.ceil((x8 + 1) / scale_x);
				for (let y = yMin; y < yMax; y++) {
					for (let x = xMin; x < xMax; x++) {
						const i = (y * imageData.width + x) * 4;
						const alpha = imageData.data[i + 3] / 255;
						data[y8][x8][0] += imageData.data[i + 2] * alpha; // R
						data[y8][x8][1] += imageData.data[i + 1] * alpha; // G
						data[y8][x8][2] += imageData.data[i] * alpha; // B
						data[y8][x8][3] += alpha;
					}
				}
			}
		}
		var img = new Image8x8();
		for (let y8 = 0; y8 < 8; y8++) {
			for (let x8 = 0; x8 < 8; x8++) {
				const rgb = bytesToUint32(
					data[y8][x8].map(c => Math.min(Math.round(c / data[y8][x8][3]), 256)),
					0,
					3
				);
				img.setPixel(x8, y8, rgb);
			}
		}
		return img;
	}

	/**
	 * @param {Uint8Array} byteArrayImage
	 * @return {Promise<Image8x8>} Matrice 2D de l'image
	 */
	static async readImage(byteArrayImage) {
		var imageData = await BlobToImageDataConvertor(new Blob([byteArrayImage.buffer]));
		return Image8x8.decodeImageData(imageData);
	}

	rotate90() {
		const rot = new Image8x8();
		for (var y = 0; y < 8; y++) {
			for (var x = 0; x < 8; x++) {
				rot.setPixel(x, y, this.getPixel(y, x));
			}
		}
		return rot;
	}

	clone() {
		const c = new Image8x8();
		for (var y = 0; y < 8; y++) {
			c.pixels[y] = [...this.pixels[y]];
		}
		return c;
	}
}

/**
 * @param {Blob} imageFileBlob
 * @return {Promise<ImageData>}
 */
function BlobToImageDataConvertor(imageFileBlob) {
	const canvas = document.createElement('canvas');
	const ctx = canvas.getContext('2d');
	const imageEl = new Image();

	const imageUrl = window.URL.createObjectURL(imageFileBlob);
	return new Promise((res, rej) => {
		imageEl.onload = e => {
			const w = e.target.width;
			const h = e.target.height;
			canvas.width = w;
			canvas.height = h;
			ctx.drawImage(e.target, 0, 0, w, h);
			const img = ctx.getImageData(0, 0, w, h);
			res(img);
		};
		imageEl.onabort = rej;
		imageEl.onerror = rej;
		imageEl.src = imageUrl;
	});
}

async function sendMatrice() {
	var images8x8 = Array.from(images.values());

	console.log('Images :', images8x8);

	images8x8.filter(image => image !== undefined);
	if (images8x8.length !== images.size) {
		alert("Une image n'a pas été chargée");
	}

	const fill_type = Array.from(document.getElementsByName('fill_type')).filter(r => r.checked)[0].value;
	var frames;
	if (fill_type === 'fill_one_layer' || fill_type === 'fill_extend') {
		const fill_one_layer = fill_type === 'fill_one_layer';
		frames = images8x8.map(image => {
			var frame = generateFrame();
			var data = image.rotate90().pixels;
			if (fill_one_layer) {
				frame[0] = data;
			} else {
				frame = frame.map(() => data);
			}
			return frame;
		});
	} else if (fill_type == 'fill_with_formule') {
		const fill_formule = document.querySelector('#fill_formule').value;
		try {
			frames = generateFramesWithFormule(fill_formule, images8x8);
		} catch (error) {
			alert(error);
			console.error(error);
			return;
		}
	} else {
		alert(`Le type de remplissage '${fill_type}' n'est pas valide`);
	}
	if (frames.length === 0) {
		alert('Aucune image à envoyer');
		return;
	}

	console.log('Frames : ', frames);

	if (document.location.search === '?debug_dont_submit') return;

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

function loadImages() {
	/**
	 * @type {HTMLInputElement}
	 */
	const image_input = document.querySelector('#image_input');
	const files = image_input.files;
	if (files.length == 0) return;

	const reader = new FileReader();
	images = new Map();
	/**
	 * @type {HTMLCanvasElement}
	 */
	const canvas = document.querySelector('canvas#display_frames');
	const ctx = canvas.getContext('2d');
	const canvasSize = 8;
	canvas.width = canvasSize;
	canvas.height = canvasSize * files.length;

	reader.addEventListener('load', async () => {
		const arrayBuffer = reader.result;
		var i = images.size;
		var byteArray = new Uint8Array(arrayBuffer);
		const img = await Image8x8.readImage(byteArray);
		images.set(files[i], img);

		var canvasImg = ctx.createImageData(8, 8);
		for (let y8 = 0; y8 < 8; y8++) {
			for (let x8 = 0; x8 < 8; x8++) {
				const iPixel = (y8 * 8 + x8) * 4;
				canvasImg.data[iPixel + 3] = 255;
				canvasImg.data[iPixel + 2] = img.pixels[y8][x8] & 0x0000ff;
				canvasImg.data[iPixel + 1] = (img.pixels[y8][x8] & 0x00ff00) >> 8;
				canvasImg.data[iPixel + 0] = (img.pixels[y8][x8] & 0xff0000) >> 16;
			}
		}
		ctx.putImageData(canvasImg, 0, canvasSize * i);

		if (i + 1 < files.length) reader.readAsArrayBuffer(files[i + 1]);
		else {
			const frame_range_input = document.querySelector('#frame_range');
			frame_range_input.setAttribute('max', images.size);
			if (images.size == 1) {
				frame_range_input.setAttribute('hidden', '');
			} else {
				frame_range_input.removeAttribute('hidden');
			}
			onImagesChanged();
		}
	});

	reader.readAsArrayBuffer(this.files[0]);
}

function updateButtonSubmit() {
	var canSubmit = false;
	const has_image = images.size > 0;
	const fill_type = Array.from(document.getElementsByName('fill_type')).filter(r => r.checked)[0].value;
	const fill_formule = document.querySelector('#fill_formule');
	const fill_formule_require_img = fill_formule.value.includes('img');
	const input_fill_time = document.querySelector('#fill_time');

	if (has_image) {
		canSubmit = true;
	} else {
		if (fill_type == 'fill_with_formule' && !fill_formule_require_img) {
			canSubmit = true;
		}
	}
	const submit = document.querySelector('#send_pattern');
	if (canSubmit) {
		submit.removeAttribute('disabled');
	} else {
		submit.setAttribute('disabled', '');
	}

	if (fill_type === 'fill_with_formule') {
		fill_formule.removeAttribute('disabled');
		input_fill_time.parentElement.removeAttribute('hidden');
	} else {
		fill_formule.setAttribute('disabled', '');
		input_fill_time.parentElement.setAttribute('hidden', '');
	}
}

function onImagesChanged() {
	updateButtonSubmit();
	if (!preLoadImages) {
		const input_fill_time = document.querySelector('#fill_time');
		input_fill_time.value = Math.max(images.size, 1);
	}
	preLoadImages = false;
}

function onFillControlChanged() {
	updateButtonSubmit();
}

function onFillFormuleChanged() {
	updateButtonSubmit();
}

/**
 * @param {string} formule
 * @param {Image8x8[]} imgs
 */
function generateFramesWithFormule(formule, imgs) {
	const input_fill_time = document.querySelector('#fill_time');
	const tMax = parseInt(input_fill_time.value);
	/** @type {number[][][][]} */
	var frames = new Array(tMax);
	const formuleS = formule.match(/=(.+)$/)[1];
	for (let t = 0; t < tMax; t++) {
		frames[t] = generateFrame();
		var formuleS_T = formuleS.replace(/t/gi, t);
		for (let z = 0; z < 8; z++) {
			var formuleS_TZ = formuleS_T.replace(/z/gi, z);
			for (let y = 0; y < 8; y++) {
				var formuleS_TZY = formuleS_TZ.replace(/y/gi, y);
				for (let x = 0; x < 8; x++) {
					var formuleS_TZYX = formuleS_TZY.replace(/x/gi, x);
					var equaColor = MATH.parseMath(formuleS_TZYX);
					var matchesImg = Array.from(equaColor.matchAll(/img\(.*\)/gi));
					if (t === 6) console.log(equaColor, matchesImg);
					matchesImg.forEach(match => {
						const imgFunc = match[0];
						var args = Array.from(imgFunc.matchAll(/(?<=[\(,])[^,]+(?=[,\)])/gi));
						var imgArgs = args.map(arg => MATH.parseMath(arg[0]));
						const tImg = parseInt(imgArgs[2]) || 0;
						const yImg = parseInt(imgArgs[1]) || 0;
						const xImg = parseInt(imgArgs[0]) || 0;
						var colorImg;
						if (tImg < 0 || imgs.length <= tImg) {
							throw new Error(
								`Vous avez dépassé le nombre d'images disponibles (${imgs.length}) avec la formule (t=${tImg}).\n` +
									`Essayez avec '0' dans l'expression 'img(y,z,0)'.`
							);
						}
						if (0 <= xImg && xImg < 8 && 0 <= yImg && yImg < 8) {
							colorImg = imgs[tImg].getPixel(xImg, yImg);
						} else {
							colorImg = 0;
						}
						equaColor = equaColor.replace(imgFunc, colorImg);
					});

					if (t === 6) console.log(`t=6 [${x}][${y}][${z}] '${formuleS_TZYX}' => '${equaColor}'`);

					var color = MATH.parseMath(equaColor);
					frames[t][x][y][z] = color;
				}
			}
		}
	}
	return frames;
}

var preLoadImages = false;
window.addEventListener('load', () => {
	/**
	 * @type {HTMLInputElement}
	 */
	const image_input = document.querySelector('#image_input');
	image_input.addEventListener('input', loadImages);
	if (image_input.files.length > 0) {
		preLoadImages = true;
		loadImages.call(image_input);
	}

	const div_frames = document.querySelector('#div_frames');
	/**
	 * @type {HTMLInputElement}
	 */
	const frame_range_input = document.querySelector('#frame_range');
	frame_range_input.addEventListener('input', () => {
		var value = frame_range_input.valueAsNumber - 1;
		var height = div_frames.clientHeight;
		div_frames.scroll(0, height * value);
	});

	document.getElementById('fill_formule').setAttribute('disabled', '');
});
