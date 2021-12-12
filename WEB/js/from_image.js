/**
 * @type {Map<File,Uint8Array>}
 */
var images8x8 = new Map();

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

function generateFrame() {
	const frame = new Array(8);
	frame.fill(generateLayer(8, 8), 0, 8);
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
		if (pixels != undefined) {
			this.pixels = pixels;
		} else {
			this.pixels = new Array(8);
			for (var y = 0; y < 8; y++) {
				this.pixels[y] = new Array(8);
				this.pixels[y].fill(0, 0, 8);
			}
		}
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
		return this.pixels[y][x];
	}

	/**
	 * @param {ImageData} imageData
	 */
	static decodeImageData(imageData) {
		const scale_x = 8 / imageData.width;
		const scale_y = 8 / imageData.height;
		const countPPPX = Math.ceil(1 / scale_x);
		const countPPPY = Math.ceil(1 / scale_y);
		const countPPP = countPPPX * countPPPY;

		var data = new Array(8);
		for (let y8 = 0; y8 < 8; y8++) {
			data[y8] = new Array(8);
			var yMin = Math.floor(y8 / scale_y);
			var yMax = Math.ceil((y8 + 1) / scale_y);
			for (let x8 = 0; x8 < 8; x8++) {
				data[y8][x8] = new Array(3).fill(0);
				var xMin = Math.floor(x8 / scale_x);
				var xMax = Math.ceil((x8 + 1) / scale_x);
				for (let y = yMin; y < yMax; y++) {
					for (let x = xMin; x < xMax; x++) {
						const i = (y * imageData.width + x) * 4;
						const alpha = imageData.data[i + 3] / 255;
						data[y8][x8][0] += imageData.data[i + 2] * alpha; // R
						data[y8][x8][1] += imageData.data[i + 1] * alpha; // G
						data[y8][x8][2] += imageData.data[i] * alpha; // B
					}
				}
			}
		}
		var img = new Image8x8();
		for (let y8 = 0; y8 < 8; y8++) {
			for (let x8 = 0; x8 < 8; x8++) {
				const rgb = bytesToUint32(
					data[y8][x8].map(c => Math.round(c / countPPP)),
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
	const images = Array.from(images8x8.values());

	console.log('Images :', images);

	if (document.location.search === '?debug_dont_submit') return;

	var count_no_image = 0;
	const frames = images.map(image => {
		const frame = generateFrame();
		if (image !== undefined) {
			frame[0] = image.rotate90().pixels;
		} else {
			count_no_image++;
			console.warn("Une image n'a pas été chargée");
		}
		return frame;
	});
	if (count_no_image === frames.length) {
		alert('Aucune image à envoyer');
		return;
	}
	if (count_no_image > 0) {
		alert("Une image n'a pas été chargée");
	}

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
	images8x8 = new Map();
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
		var i = images8x8.size;
		var byteArray = new Uint8Array(arrayBuffer);
		const img = await Image8x8.readImage(byteArray);
		images8x8.set(files[i], img);

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

		if (i == 0) {
			document.querySelector('#send_matrice').removeAttribute('disabled');
		}
		if (i + 1 < files.length) reader.readAsArrayBuffer(files[i + 1]);
		else {
			const frame_range_input = document.querySelector('#frame_range');
			frame_range_input.setAttribute('max', images8x8.size);
			if (images8x8.size == 1) {
				frame_range_input.setAttribute('hidden', '');
			} else {
				frame_range_input.removeAttribute('hidden');
			}
		}
	});

	reader.readAsArrayBuffer(this.files[0]);
}

window.addEventListener('load', () => {
	const image_input = document.querySelector('#image_input');
	image_input.addEventListener('input', loadImages);

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
});
