import { ledCubeTools } from '../../common/js/main.js';
import { LEDAnimation } from '../../common/js/utils/frame.js';

var animationsReceived = false;
var documentLoaded = false;
export var animations = [];
var nodeQuery = '#animations';

ledCubeTools.ledcubeWS.addEventListener(ledCubeTools.ledcubeWS.EVENTS.LOGGED, async () => {
	var answer = await ledCubeTools.ledcubeWS.sendLedcube.animation.list();
	if (answer.success) {
		animations = answer.animations;
		animationsReceived = true;
		if (documentLoaded) animationsAndDocumentLoaded();
	}
	else {
		console.warn('Error while getting animations', answer);
	}
});

document.addEventListener('DOMContentLoaded', () => {
	documentLoaded = true;
	if (animationsReceived) animationsAndDocumentLoaded();
});

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

function animationsAndDocumentLoaded() {
	var node = document.querySelector(nodeQuery);
	// Remove all children
	while (node.firstChild) {
		node.removeChild(node.firstChild);
	}

	const animationsNoExt = animations.map(getWithoutExtension);
	const animationsNoExtDouble = animationsNoExt.filter((animation, index) => animationsNoExt.indexOf(animation) !== index);
	// If there is animations with the same name but different extensions, we have to keep the extension for the 2 animations
	animations.sort((a, b) => getWithoutExtension(a).localeCompare(getWithoutExtension(b)) || a.localeCompare(b));

	for (let animationName of animations) {
		// Test is without extension only if it's the only animation with this name
		const animationNameNoExt = getWithoutExtension(animationName);
		const canBeWithoutExtension = animationsNoExtDouble.indexOf(animationNameNoExt) === -1;

		var animationNode = document.createElement('div');
		animationNode.classList.add('animContent');

		var animationNameNode = document.createElement('div');
		animationNameNode.classList.add('contentTitleAnim');
		var animationNameTextNode = document.createElement('p');
		animationNameTextNode.classList.add('titleAnim');
		animationNameTextNode.innerText = canBeWithoutExtension ? animationNameNoExt : animationName;
		animationNameNode.appendChild(animationNameTextNode);
		animationNode.onclick = (ev) => { playAnimationLocal(animationName); ev.stopPropagation(); };
		animationNode.appendChild(animationNameNode);

		var animationEditLinkNode = document.createElement('a');
		animationEditLinkNode.classList.add('button');
		animationEditLinkNode.href = webFolder + 'animation/edit/?animation=' + animationName;
		var animationEditButtonNode = document.createElement('button');
		animationEditButtonNode.classList.add('editButton');
		animationEditLinkNode.appendChild(animationEditButtonNode);
		animationNode.appendChild(animationEditLinkNode);

		var animationPlayNode = document.createElement('button');
		animationPlayNode.classList.add('playButton');
		animationPlayNode.onclick = (ev) => { playAnimation(animationName); ev.stopPropagation(); };
		animationNode.appendChild(animationPlayNode);

		var animationDeleteNode = document.createElement('button');
		animationDeleteNode.classList.add('deleteButton');
		animationDeleteNode.onclick = (ev) => { deleteAnim(animationName); ev.stopPropagation(); };
		animationNode.appendChild(animationDeleteNode);

		animationNode.setAttribute('data-animation-name', animationName);
		animationNode.title = animationName;

		node.appendChild(animationNode);
	}
}

export function playAnimation(animationName) {
	ledCubeTools.ledcubeWS.sendLedCubeWithNotification.animation.play(animationName);
}

export async function playAnimationLocal(animationName) {
	var answer = await ledCubeTools.ledcubeWS.sendLedcube.animation.get(animationName);
	if (!answer.success) {
		console.warn('Error while getting animation', answer);
		return;
	}
	/** @type {LEDAnimation} */
	var animation = answer.animation;
	document.getElementById("modal").style.display = "block";
	ledCubeTools.cubeViewer.animation = animation;
	ledCubeTools.cubeViewer.startAnim();
}

export function stopAnimationLocal() {
	document.getElementById("modal").style.display = "none";
	ledCubeTools.cubeViewer.stopAnim();
}

export async function deleteAnim(animation) {
	if (confirm(`Are you sure you want to delete this animation ?\n${animation}`)) {
		const answer = await ledCubeTools.ledcubeWS.sendLedCubeWithNotification.animation.delete(animation);
		if (answer.success) {
			animations = animations.filter(anim => anim !== animation);
			var node = document.querySelector(nodeQuery);
			var titleAnim = node.querySelector(`.animContent[title="${animation}"]`);
			titleAnim.remove();
		}
	}
}

export default {
	ledCubeTools,
	animations,
	playAnimation,
	playAnimationLocal,
	stopAnimationLocal,
	deleteAnim,
}
