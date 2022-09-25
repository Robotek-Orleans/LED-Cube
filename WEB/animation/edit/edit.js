import { ledCubeTools } from '../../common/js/main.js';
import { RGBColor } from '../../common/js/utils/ColorPicker.js';
import { selectMatrix } from './selectMatrix.js';

var dirty = false; // Dirty => save button is enabled
var selectedPlanDirection = "X"; // X , Y ou Z
var selectedPlanNumber = 1; // entre 1 et 8
var selectedFrame = 1; // entre 1 et N frame

/** @type {number[][]} */
var copied2D = [];
/** @type {number[][][]} */
var copied3D = [];
var loaded = false;

export function getSelectedFrame() {
	return ledCubeTools.cubeViewer.animation.frames[selectedFrame - 1];
}

export function getFrameCount() {
	return ledCubeTools.cubeViewer.animation.frames.length;
}

export function paste2D() {
	if (!copied2D.length) return;
	dirty = true;
	getSelectedFrame().setPlan(selectedPlanDirection, selectedPlanNumber - 1, copied2D);
	refresh3D()
}

export function makeDirty() {
	dirty = true
}

export function copy2D() {
	document.getElementById("paste2D").disabled = false
	copied2D = getSelectedFrame().getPlan(selectedPlanDirection, selectedPlanNumber - 1)
}


export function copy3D() {
	document.getElementById("paste3D").disabled = false
	copied3D = getSelectedFrame().copy().colors;
}

export function paste3D() {
	if (!copied3D.length) return;
	dirty = true;
	getSelectedFrame().colors = copied3D;
	refresh3D();
}

export function Draw2DMatrix() {
	const matrix = document.getElementById("matrix");
	matrix.innerHTML = "";
	for (let i = 0; i < 8; i++) {
		for (let j = 0; j < 8; j++) {
			matrix.innerHTML += "<svg onmousedown=\'module.selectLED(" + (j + i * 8) + ", event)\'><circle stroke-width=\'3\' stroke=\'#3e4f51\' fill=\'#000000\' cx=\'50%\' cy=\'50%\' r=\'20\'></circle></svg>";
		}
	}
	selectMatrix.removeAll();
	ledCubeTools.colorPicker.setColor(new RGBColor(0, 0, 0));
	refresh2D();
}

export function onColorPickedUp() {
	const color = ledCubeTools.colorPicker.color;
	const colorDec = color.toDecimal();
	var ledChanged = 0;
	var plan = getSelectedFrame().getPlan(selectedPlanDirection, selectedPlanNumber - 1);

	for (const pos of selectMatrix.selected2D) {
		if (plan[7 - pos.y][pos.x] !== colorDec) {
			plan[7 - pos.y][pos.x] = colorDec;
			ledChanged++;
		}
	}

	if (ledChanged > 0) {
		dirty = true;
		getSelectedFrame().setPlan(selectedPlanDirection, selectedPlanNumber - 1, plan);
		refresh3D();
	}
}

export function refresh2D() {
	const matrix = document.getElementById("matrix");
	const circles = Array.from(matrix.getElementsByTagName("circle"));
	const plan = getSelectedFrame().getRGBPlan(selectedPlanDirection, selectedPlanNumber - 1);
	for (let i = 0; i < 8; i++) {
		for (let j = 0; j < 8; j++) {
			const LEDelement = circles[j + (7 - i) * 8];
			if (!LEDelement) continue;
			let color = plan[i][j];
			LEDelement?.setAttribute("fill", color.toHex());
		}
	}
}

export function refresh3D() {
	ledCubeTools.cubeViewer.animation.currentFrameIndex = selectedFrame - 1;
	ledCubeTools.cubeViewer.refresh();
	refresh2D();
}

/**
 * @param {number} LED
 * @param {MouseEvent} event
 */
export function selectLED(LED, event = {}) {
	selectMatrix.updateLedCircles();

	const x = LED % 8
	const y = Math.floor(LED / 8)
	const clickedPos = { x, y };
	const LEDelement = selectMatrix.getLedElement(clickedPos);
	const isSelected = selectMatrix.findSelectedLedIndex(clickedPos) !== -1;

	if (isSelected && !event.shiftKey) {
		// Just unselect the LED
		selectMatrix.removeLed(clickedPos);
		return
	}

	if (!event.ctrlKey) {
		selectMatrix.removeAll();
	}

	if (event.shiftKey) {
		selectMatrix.addSquare(clickedPos, selectMatrix.lastClicked);
	} else {
		selectMatrix.addLed(clickedPos, true);
	}

	if (!event.ctrlKey && !event.shiftKey) {
		selectMatrix.lastClicked = clickedPos;
	}

	ledCubeTools.colorPicker.setColor(RGBColor.fromHex(LEDelement.getAttribute("fill")), false)
}


export function addFrameBefore() {
	const contentnum = document.getElementById("numframebefore")
	let index = Math.max(0, Math.min(parseInt(contentnum.value) - 1), getFrameCount() - 1)

	contentnum.value = index + 1

	addFrame(index)

}

export function addFrameAfter() {
	const contentnumaf = document.getElementById("numframeafter")
	let index = Math.max(1, Math.min(parseInt(contentnumaf.value), getFrameCount()))

	if (index === getFrameCount()) {
		contentnumaf.value = index + 1
	} else {
		contentnumaf.value = index
	}

	addFrame(index)
}

export function addFrame(index) {
	dirty = true;
	ledCubeTools.cubeViewer.animation.addFrame(index);

	const framerange = document.getElementById("frameRange")
	const frameinput = document.getElementById("frameInput")
	if (index <= (selectedFrame - 1)) {
		selectedFrame++;
		framerange.value = selectedFrame
		frameinput.value = selectedFrame
	}
	onframeschanged();
}

export function removeFrame() {
	if (getFrameCount() <= 1) {
		alert('Erreur: vous devez avoir au moins une frame')
		return
	}
	dirty = true;
	const framerange = document.getElementById("frameRange")
	const frameinput = document.getElementById("frameInput")
	const frame = framerange.value
	let confirmremove = confirm('Voulez-vous supprimer la frame numéro ' + frame + ' ?')
	if (confirmremove && getFrameCount() > 1) {
		ledCubeTools.cubeViewer.animation.removeFrame(frame - 1);
		if (frame > getFrameCount()) {
			framerange.value = getFrameCount()
			frameinput.value = getFrameCount()
			selectedFrame = getFrameCount()
		}
		onframeschanged();
	}
}

export function goToFrame() {
	const framerange = document.getElementById("frameRange")
	const frameinput = document.getElementById("frameInput")
	if (framerange.value < 1) framerange.value = 1
	if (framerange.value > getFrameCount()) framerange.value = getFrameCount()
	selectedFrame = framerange.value
	frameinput.value = framerange.value
	refresh3D()
}

export function nextFrame(loop = false) {
	const frameRange = document.getElementById("frameRange")
	if (loop && frameRange.value >= getFrameCount())
		frameRange.value = 1
	else
		frameRange.value++
	goToFrame()
}

export function previousFrame(loop = false) {
	const frameRange = document.getElementById("frameRange")
	if (loop && frameRange.value <= 1)
		frameRange.value = getFrameCount()
	else
		frameRange.value--
	goToFrame()
}

export function onframeschanged() {
	const FRAME_COUNT = getFrameCount();
	const framenumber = document.getElementById("frameNumber")
	if (framenumber)
		framenumber.innerHTML = FRAME_COUNT.toString()

	const framerange = document.getElementById("frameRange")
	const frameinput = document.getElementById("frameInput")
	const contentnum = document.getElementById("numframebefore")
	const contentnumaf = document.getElementById("numframeafter")
	framerange.max = frameinput.max = contentnumaf.max = contentnum.max = FRAME_COUNT
	if (contentnum.value > FRAME_COUNT) contentnum.value = FRAME_COUNT
	if (contentnumaf.value > FRAME_COUNT) contentnumaf.value = FRAME_COUNT
	refresh3D()
}

var initDone = false;
export async function init() {
	if (initDone) return;
	Draw2DMatrix()
	document.getElementsByName("axe").forEach(item => item.addEventListener("click", selectPlan))

	if (getFrameCount() == 0)
		addFrame(0);
	else
		onframeschanged();

	selectedPlanDirection = getRadioSelectedValue("axe")
	document.getElementById("planNumber").value = 1
	if (!copied2D.length) document.getElementById("paste2D").disabled = true
	if (!copied3D.length) document.getElementById("paste3D").disabled = true
	document.getElementById("frameInput").value = 1
	document.getElementById("frameRange").value = 1
	updateSaveButton()

	ledCubeTools.addEventListener(ledCubeTools.EVENTS.PICKUP_COLOR, onColorPickedUp);
	initDone = true;
}


var loggedInit = false;
export async function onLogged() {
	if (!initDone) init();
	if (!loggedInit) {
		// Get the animation if specified in the URL
		const urlParams = new URLSearchParams(window.location.search);
		const animationName = urlParams.get('animation');
		const animationLocalKey = urlParams.get('local_key');

		if (animationName) {
			const response = await ledCubeTools.ledcubeWS.sendLedcube.animation.get(animationName);
			if (response.success) {
				console.log("Animation chargée", response);
				ledCubeTools.cubeViewer.animation = response.animation;
				document.querySelector('#frameTime').value = response.animation.frameDuration;
				document.getElementById("fileName").value = getWithoutExtension(animationName); // Remove the extension
				updateSaveButton();
				onframeschanged();
			}
			else {
				console.error("Impossible de charger l'animation demandée", response);
			}
		}
		else if (animationLocalKey) {
			const animation = ledCubeTools.ledcubeWS.getAnimationLocal(animationLocalKey);
			if (animation) {
				ledCubeTools.cubeViewer.animation = animation;
				document.getElementById("fileName").value = animation.name;
				updateSaveButton();
				onframeschanged();
			}
			else {
				console.warn("Impossible de charger l'animation locale demandée.", animationLocalKey);
			}
		}
	}
	loggedInit = true;
}

export function updateSaveButton() {
	const fileName = document.getElementById("fileName")
	const saveButton = document.getElementById("saveButton");
	const exportButton = document.getElementById("exportButton");

	const fileNameIsValid = fileName.value.length > 0 && new RegExp("^[a-zA-Z0-9À-ÿ_\.\-\/ ]+$", 'i').test(fileName.value);

	if (fileNameIsValid) {
		saveButton.disabled = false;
		exportButton.disabled = false;
	} else {
		saveButton.disabled = true;
		exportButton.disabled = true;
	}
}


export function selectPlan() {
	selectedPlanDirection = getRadioSelectedValue("axe")
	const range = document.getElementById("planNumber")
	selectedPlanNumber = parseInt(range.value)
	displaySelectedPlan(selectedPlanDirection, selectedPlanNumber - 1)
	document.querySelector('#toggleViewPlan').innerText = 'Masquer le marquage du plan';
	refresh2D()
}

export function getRadioSelectedValue(radioName) {
	var radios = Array.from(document.getElementsByName(radioName));
	return radios.find(r => r.checked)?.value;
}

export function setRadioSelectedValue(radioName, value) {
	var radios = Array.from(document.getElementsByName(radioName));
	radios.forEach(r => r.checked = r.value === value);
}


export function nextplan() {
	let element = document.getElementById("planNumber");
	let val = parseInt(element.value) + 1;
	if (val > 8) val = 8;
	element.value = val;
	selectPlan();
}

export function previousplan() {
	let element = document.getElementById("planNumber");
	let val = parseInt(element.value) - 1;
	if (val < 1) val = 1;
	element.value = val;
	selectPlan();
}

export function switchaxe() {
	const axises = ["X", "Y", "Z"];
	let index = (axises.indexOf(selectedPlanDirection) + 1) % 3;
	setRadioSelectedValue("axe", axises[index]);
	selectPlan();
}

/**
 * @param {MouseEvent} event
 */
export function toggle3DViewPlan(event) {
	if (wireFrame) {
		clear3DViewPlan();
		wireFrame = false;
		event.target.innerText = 'Afficher le marquage du plan';
	} else {
		selectPlan();
		event.target.innerText = 'Masquer le marquage du plan';
	}
}

export function sendAnimation() {
	ledCubeTools.ledcubeWS.sendLedCubeWithNotification.animation.play_no_save(ledCubeTools.animation);
}

/**
 * @param {string} filename
 */
function getWithoutExtension(filename) {
	const extensions = ['.json.txt', '.json', '.bin', '.txt', '.ledcube', '.ledcube_animation'];
	for (const extension of extensions) {
		if (filename.endsWith(extension)) {
			return filename.slice(0, -extension.length);
		}
	}
	return filename;
}

async function saveAnimationAndConfirmIfNeeded(fileName) {
	let answer = await ledCubeTools.ledcubeWS.sendLedCubeWithNotification.animation.save({ fileName, animation: ledCubeTools.animation });
	if (!answer?.success) {
		if (answer?.error === 'File already exists') {
			if (confirm('Le fichier ' + fileName + ' existe déjà, voulez-vous le remplacer ?')) {
				document.querySelector('#contentnotifs > tbody > tr:first-child').remove(); // Remove the error notification
				let answer = await ledCubeTools.ledcubeWS.sendLedCubeWithNotification.animation.save({ fileName, animation: ledCubeTools.animation, overwrite: true });
				if (!answer?.success) {
					alert('Impossible de sauvegarder l\'animation : ' + answer?.error);
				}
			}
		}
	}
}

export async function saveAnimation() {
	/** @type {string} */
	const fileName = document.getElementById("fileName").value;
	ledCubeTools.animation.name = fileName;
	const fileNameWithExtension = fileName.includes('.json.txt') ? fileName : fileName + '.json.txt';
	saveAnimationAndConfirmIfNeeded(fileNameWithExtension);
}

export async function saveAnimationWithoutExtension() {
	const fileName = document.getElementById("fileName").value;
	ledCubeTools.animation.name = fileName;
	saveAnimationAndConfirmIfNeeded(fileName);
}

export async function exportAnimation() {
	// Save dialog to save the content of the animation
	const fileName = document.getElementById("fileName").value;
	ledCubeTools.animation.name = fileName;
	const fileData = ledCubeTools.ledcubeWS.encodeAnimation(ledCubeTools.animation);

	const blob = new Blob([fileData], { type: 'application/octet-stream' });
	const url = URL.createObjectURL(blob);
	const a = document.createElement('a');
	a.href = url;
	a.download = fileName + '.json.txt';
	a.click();
}

export function changeFrameTime() {
	const previousDuration = ledCubeTools.animation.frameDuration;
	const newDuration = parseInt(document.getElementById("frameTime").value);
	ledCubeTools.animation.frameDuration = newDuration;
	if (previousDuration != newDuration) {
		// Start the animation again (if it was playing)
		const frameIndex = ledCubeTools.animation.currentFrameIndex;
		if (ledCubeTools.cubeViewer.animInterval) {
			ledCubeTools.cubeViewer.startAnim({ frameIndex });
		}
	}
}

export function playAnimationLocal() {
	const playButton = document.getElementById("playAnimationLocal");
	if (playButton.innerText === "Jouer en local") {
		ledCubeTools.cubeViewer.startAnim();
		playButton.innerText = "Arrêter l'animation en local";
	}
	else {
		ledCubeTools.cubeViewer.stopAnim();
		playButton.innerText = "Jouer en local";
	}
}

export function stopAnimationLocal() {
	ledCubeTools.cubeViewer.stopAnim();
}

document.addEventListener('keydown', (e) => {
	if (e.target instanceof HTMLInputElement || e.target instanceof HTMLButtonElement) return;
	switch (e.key) {
		case "ArrowRight":
			if (e.altKey) return;
			e.preventDefault();
			nextFrame(true);
			if (e.shiftKey) nextFrame(true);
			break;
		case "ArrowLeft":
			if (e.altKey) return;
			e.preventDefault();
			previousFrame(true);
			if (e.shiftKey) previousFrame(true);
			break;
		case "Tab":
			e.preventDefault();
			switchaxe();
			break;
		case "ArrowDown":
			e.preventDefault();
			previousplan();
			break;
		case "ArrowUp":
			e.preventDefault();
			nextplan();
			break;
	}
});

/*
ArrowDown: plan précédent
ArrowUp: plan suivant
Tab: change l'axe du plan
ArrowLeft: frame précédente
ArrowRight: frame suivante
*/

export default {
	getSelectedFrame,
	getFrameCount,
	paste2D,
	makeDirty,
	selectPlan,
	copy2D,
	copy3D,
	paste3D,
	Draw2DMatrix,
	onColorPickedUp,
	refresh2D,
	refresh3D,
	selectMatrix,
	selectLED,
	getRadioSelectedValue,
	setRadioSelectedValue,
	addFrameBefore,
	addFrameAfter,
	goToFrame,
	removeFrame,
	nextFrame,
	previousFrame,
	onframeschanged,
	updateSaveButton,
	nextplan,
	previousplan,
	switchaxe,
	toggle3DViewPlan,
	sendAnimation,
	saveAnimation,
	saveAnimationWithoutExtension,
	exportAnimation,
	playAnimationLocal,
	changeFrameTime,

	ledCubeTools,
	get copied2D() { return copied2D; },
	get copied3D() { return copied3D; },
};

window.addEventListener('load', () => init());
ledCubeTools.addEventListener(ledCubeTools.EVENTS.LOGGED, () => onLogged());
