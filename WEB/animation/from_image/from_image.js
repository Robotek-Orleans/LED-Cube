import { LEDAnimation } from '../../common/js/utils/frame.js';
import { ledcubeWS, requests } from '../../common/js/ws/main.js';

/**
 * @type {Map<File,Image8x8>}
 */
var images = new Map();
/**
 * @type {Image8x8[]}
 */
var images8x8 = [];

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
						data[y8][x8][2] += imageData.data[i] * alpha;	  // B
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
					3);
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

async function sendMatrice(options) {
	images8x8 = Array.from(images.values());

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
		try {
			frames = generateFramesWithFormule();
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

	submitPattern(frames, options);
}

/**
 * @param {number[][][][]} frames
 */
function submitPattern(frames, options) {
	const animation = LEDAnimation.fromDecimalArrays(frames);
	const key = ledcubeWS.storeAnimationLocal(animation);
	ledcubeWS.editAnimationLocal(key, options);
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

		if (i + 1 < files.length)
			reader.readAsArrayBuffer(files[i + 1]);
		else {
			const frame_range_input = document.querySelector('#frame_range');
			frame_range_input.setAttribute('max', images.size);
			if (images.size == 1) {
				frame_range_input.setAttribute('disabled', '');
			} else {
				frame_range_input.removeAttribute('disabled');
			}
			onImagesChanged();
		}
	});

	reader.readAsArrayBuffer(this.files[0]);
}

var formuleReady = false;
var formuleNeedImage = false;

function updateButtonSubmit() {
	var canSubmit;
	const imagesLoaded = images.size > 0;
	const fillType = Array.from(document.getElementsByName('fill_type')).filter(r => r.checked)[0].value;

	if (fillType === 'fill_with_formule') {
		allowFunctionInput(true);
		if (formuleReady) {
			if (formuleNeedImage)
				canSubmit = imagesLoaded;
			else
				canSubmit = true;
		} else {
			canSubmit = false;
		}

	} else {
		allowFunctionInput(false);
		canSubmit = imagesLoaded;
	}

	const submit = document.querySelector('#send_pattern');
	if (canSubmit) {
		submit.removeAttribute('disabled');
	} else {
		submit.setAttribute('disabled', '');
	}
}

function onImagesChanged() {
	updateButtonSubmit();
	if (!preLoadImages) {
		const fill_tmax = document.querySelector('#fill_tmax');
		fill_tmax.value = Math.max(images.size, 1);
	}
	preLoadImages = false;
}

function onFillControlChanged() {
	updateButtonSubmit();
}

function allowFunctionInput(enabled) {
	const config_formule = document.querySelector('#config_formule');
	const fill_tmax = document.querySelector('#fill_tmax');

	if (enabled) {
		config_formule.removeAttribute('disabled');
		formule_input.removeAttribute('disabled');
		fill_tmax.removeAttribute('disabled');
	} else {
		config_formule.setAttribute('disabled', '');
		formule_input.setAttribute('disabled', '');
		fill_tmax.setAttribute('disabled', '');
	}
}

/**
 * @type {HTMLTextAreaElement}
 */
var formule_input;
/**
 * @type {HTMLDivElement}
 */
var fill_formule;

function onFormuleChanged() {
	const formule = getFormule();
	var systeme;
	var error;
	formuleNeedImage = false;
	fill_formule.innerHTML = '';

	try {
		systeme = new JigMath.System(formule);
		formuleReady = true;
	} catch (err) {
		formuleReady = false;
		if (err.constructor === JigMath.EquaError) {
			systeme = err.system;
			error = err;
		} else {
			console.warn('Formule error', err, err.system);
			fill_formule.appendChild(createSpanForItem(err.message || err));
			updateButtonSubmit();
			return;
		}
	}

	if (formulePrefix) {
		const spanPrefix = createSpanForItem(formulePrefix);
		spanPrefix.removeAttribute('JigMath');
		fill_formule.appendChild(spanPrefix);
	}
	fill_formule.appendChild(createSpanForItem(systeme));
	if (formule.endsWith('\n')) fill_formule.appendChild(createSpanForItem(' '));
	if (error?.item) {
		var errorItem;
		if (error.data?.blobEnd) {
			const blobEndIndex = error.data.blobEnd.iS;
			const childs = Array.from(error.item?.span?.children || []);
			errorItem = childs[blobEndIndex];
		}
		if (!errorItem) {
			errorItem = error.item?.span;
		}
		errorItem?.classList.add('equa_error');
	}

	onFormuleCursorMoved();
	updateButtonSubmit();
}

/**
 * @param {Item} item
 */
function createSpanForItem(item) {
	const span = document.createElement('span');
	if (typeof item === 'string') {
		fillSpanWithText(span, item);
		if (item.match(/^\s*$/)) {
			span.setAttribute('JigMath', 'spaces');
		} else {
			span.setAttribute('JigMath', 'string');
		}
		if (item === ')' || item === '(') {
			span.classList.add('equa_error');
		}
	} else {
		item.span = span;
		span.item = item;
		span.setAttribute('JigMath', item.constructor.name);
		const subItems = item.getSubItems();
		if (subItems) {
			for (const subItem of subItems) {
				span.appendChild(createSpanForItem(subItem));
			}
		} else {
			fillSpanWithText(span, item.getOriginal());
		}
		applyEquaDecoration(item, span);
	}
	return span;
}

function fillSpanWithText(span, text) {
	if (text.includes('\n')) {
		var spanCount = 0;
		for (const s of text.split('\n')) {
			if (spanCount) span.appendChild(document.createElement('br'));
			const subSpan = document.createElement('span');
			subSpan.innerText = s;
			span.appendChild(subSpan);
			spanCount++;
		}
	} else {
		span.innerText = text;
	}
}

/**
 * Colors the equation
 * @param {Item} item
 * @param {HTMLSpanElement} span
 */
function applyEquaDecoration(item, span) {
	if (item.constructor === JigMath.EquaVariable) {
		if (!['x', 'y', 'z', 't'].includes(item.name)) {
			span.classList.add('equa_warning');
			formuleReady = false;
		}
	} else if (item.constructor === JigMath.EquaFunction) {
		if (!item.function) {
			span.classList.add('equa_warning');
			formuleReady = false;
		}
		if (item.function === getPixelImgs)
			formuleNeedImage = true;
	}
}

function onFormuleCursorMoved() {
	const cursorIndexStart = formule_input.selectionStart;
	const cursorIndexEnd = formule_input.selectionEnd;

	if (cursorIndexStart !== cursorIndexEnd) {
		highlightBlob(null);
		return;
	}

	const cursorIndex = cursorIndexStart;
	const selectedItemBefore = getEquaItemAtOriginalIndex(fill_formule, cursorIndex - 1);
	const selectedItemAfter = getEquaItemAtOriginalIndex(fill_formule, cursorIndex);

	var blobHightlight;
	if (selectedItemBefore) {
		blobHightlight = getFirstBlobParent(selectedItemBefore);
	}
	if (selectedItemAfter && (!blobHightlight || selectedItemAfter.getAttribute('JigMath') === 'EquaBlobLimit')) {
		blobHightlight = getFirstBlobParent(selectedItemAfter);
	}

	highlightBlob(blobHightlight);
}

/**
 * @param {HTMLSpanElement} span
 * @param {number} index
 * @param {number} originalOffset
 * @return {HTMLElement}
 */
function getEquaItemAtOriginalIndex(span, index, originalOffset = 0) {
	/**
	 * @type {HTMLSpanElement[]}
	 */
	const subItems = span.children && Array.from(span.children);
	if (!subItems) return span;

	for (const subItem of subItems) {
		if (subItem.constructor !== HTMLSpanElement)
			continue;
		const original = subItem.innerText;
		const originalLength = original.length;
		if (originalOffset + originalLength > index)
			return getEquaItemAtOriginalIndex(subItem, index, originalOffset);
		originalOffset += originalLength;
	}
	return span;
}

/**
 * @param {HTMLSpanElement} span
 */
function getFirstBlobParent(span) {
	while (span && span.getAttribute('JigMath') !== 'EquaBlob') {
		span = span.parentElement;
	}
	return span;
}

/**
 * @type {HTMLSpanElement}
 */
var previousBlobHighlighed;
/**
 * @param {HTMLSpanElement} blob
 */
function highlightBlob(blob) {
	if (blob === previousBlobHighlighed) return;
	if (previousBlobHighlighed) {
		previousBlobHighlighed.removeAttribute('selected');
	}
	if (blob) {
		blob.setAttribute('selected', '');
		previousBlobHighlighed = blob;
	} else {
		previousBlobHighlighed = undefined;
	}
}

function getTMax(imgCount) {
	const fill_tmax = document.querySelector('#fill_tmax');
	var tMax = parseInt(fill_tmax.value);
	if (isNaN(tMax)) return imgCount;
	return tMax;
}

function getPixelImgs(xImg, yImg, tImg) {
	if (tImg < 0 || images8x8.length <= tImg) {
		throw new JigMath.EquaError(
			`Vous avez dépassé le nombre d'images disponibles (${images8x8.length}) avec la formule (t=${tImg}).\n` +
			`Essayez avec '0' dans l'expression 'img(y,z,0)'.\n` +
			`Position: img(${xImg}, ${yImg}, ${tImg})`,
			this, { xImg, yImg, tImg });
	}
	if (xImg < 0 || 8 <= xImg || yImg < 0 || 8 <= yImg) return 0;
	return images8x8[tImg].getPixel(xImg, 7 - yImg);
}

function fillFrameWithFormule(systeme, t) {
	var frame = generateFrame();
	systeme.setVariable('t', t);
	for (let z = 0; z < 8; z++) {
		systeme.setVariable('z', z);
		for (let y = 0; y < 8; y++) {
			systeme.setVariable('y', y);
			for (let x = 0; x < 8; x++) {
				systeme.setVariable('x', x);

				var color = systeme.getValue();
				if (isNaN(color) || typeof color !== 'number') {
					console.log('Equation non valide', { x, y, z, t, color, systeme });
					throw new Error(`L'équation n'est pas valide : ${color}`);
				}
				frame[z][y][x] = color;
			}
		}
	}
	return frame;
}

var formulePrefix = '';
function getFormule() {
	/**
	 * @type {string}
	 */
	var formule = formule_input.value;
	formulePrefix = formule.match(/^f\([\w,]+\)\s*=\s*/)?.[0] || '';
	if (formulePrefix)
		formule = formule.replace(formulePrefix, '');
	return formule;
}

function generateFramesWithFormule() {
	const tMax = getTMax(images8x8.length);

	/**
	 * @type {number[][][][]}
	 */
	var frames = new Array(tMax);

	const formule = getFormule();

	var startEqua = Date.now();
	const systeme = JigMath.getSystem(formule);
	var dureeEqua = Date.now() - startEqua;
	console.log(`Système généré en ${dureeEqua} ms`, systeme);

	const start = Date.now();
	var deltaLog = 0;
	for (let t = 0; t < tMax; t++) {
		frames[t] = fillFrameWithFormule(systeme, t);
		if (Date.now() - deltaLog > 500) {
			console.info(`Génération de la frame ${t}/${tMax}`);
			deltaLog = Date.now();
		}
	}
	console.info(`Frames générées en ${Date.now() - start} ms`);
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

	const front_view = document.querySelector('#front_view');
	/**
	 * @type {HTMLInputElement}
	 */
	const frame_range_input = document.querySelector('#frame_range');
	frame_range_input.addEventListener('input', () => {
		var value = frame_range_input.valueAsNumber - 1;
		var height = front_view.clientHeight;
		front_view.scroll(0, height * value);
	});

	const submit = document.querySelector('#send_pattern');

	submit.addEventListener('mousedown', e => {
		if (e.button === 1) e.preventDefault();
	});
	submit.addEventListener('mouseup', e => {
		if (e.button === 1) sendMatrice({ submit_blank: true });
	});
	submit.addEventListener('click', e => {
		if (e.ctrlKey)
			sendMatrice({ submit_blank: true });
		else
			sendMatrice();
	});

	document.getElementsByName('fill_type').forEach(element => element.addEventListener('input', onFillControlChanged));

	// Add a custom functions (you can have as many arguments as you want)
	// example: JigMath.addCustomFunction('custom_function', (a, b) => (a + 1) * (b - 2));
	JigMath.addCustomFunction('img', getPixelImgs);
	JigMath.setLogLevel(1);

	// Colored TextArea https://stackoverflow.com/a/56087599/12908345
	formule_input = document.getElementById('formule_input');
	fill_formule = document.querySelector("#formule_container #fill_formule");
	formule_input.addEventListener("scroll", () => fill_formule.scrollTop = formule_input.scrollTop);
	formule_input.addEventListener("input", onFormuleChanged);
	formule_input.addEventListener("mousedown", () => setTimeout(onFormuleCursorMoved, 1));
	formule_input.addEventListener("keydown", () => setTimeout(onFormuleCursorMoved, 1));

	formule_input.value = "f(x,y,z,t) = (y==0) && img(x,z,t)";
	onFormuleChanged();

	updateButtonSubmit();
});

export default {
	ledcubeWS,
	requests,

	bytesToUint32,
	generateLayer,
	generateFrame,
	Image8x8,
	BlobToImageDataConvertor,
	submitPattern,
	loadImages,
	updateButtonSubmit,
	onImagesChanged,
	onFillControlChanged,
	allowFunctionInput,
	onFormuleChanged,
	createSpanForItem,
	fillSpanWithText,
	applyEquaDecoration,
	onFormuleCursorMoved,
	getEquaItemAtOriginalIndex,
	getFirstBlobParent,
	highlightBlob,
	getTMax,
	getPixelImgs,
	fillFrameWithFormule,
	getFormule,
	generateFramesWithFormule,
};