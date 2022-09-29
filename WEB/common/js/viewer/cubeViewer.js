import { XLENGTH, YLENGTH, ZLENGTH, LEDAnimation } from '../utils/frame.js';
import { EventEmitter } from '../utils/EventEmitter.js';

/**
 * Scripts nécessaires : three.min.js, OrbitControls.js, Detector.js, Viewerscript.js
 */

export class CubeViewer extends EventEmitter {
	/** @type {HTMLElement} */
	container;

	/** @type {LEDAnimation} */
	animation = [];

	EVENTS = {
		FRAMES_CHANGED: 'frameschanged',
	};

	constructor() {
		super();
		this.container = undefined;
		this.animation = new LEDAnimation();
		this.registerEvent(this.EVENTS.FRAMES_CHANGED);
	}

	#emitFramesChanged() { this.emitEvent(this.EVENTS.FRAMES_CHANGED); }

	/** @param {HTMLElement} container */
	async init(container) {
		this.container = container;
		if (this.animation.frames.length == 0) this.animation.addFrame();
		this.#emitFramesChanged();

		await initGL();
		this.refresh();

		const timerRefresh3D = (time) => {
			if (renderer.info.memory.textures == 2) {
				return;
			}
			this.refresh();
			setTimeout(timerRefresh3D, time, time * 2);
		};
		setTimeout(timerRefresh3D, 50, 50);
	}

	reset() {
		if (!confirm('Voulez-vous vraiment tout effacer ?')) return;
		this.animation = new LEDAnimation();
		this.refresh();
	}

	refresh() {
		const currentFrame = this.animation.currentFrame;
		for (let z = 0; z < ZLENGTH; z++) {
			for (let y = 0; y < YLENGTH; y++) {
				for (let x = 0; x < XLENGTH; x++) {
					cube_set_color(x, y, z, currentFrame.colors[z][y][x], false);
				}
			}
		}
		renderer.render(scene, camera);
	}

	animInterval = null;
	/**
	 * @param {{frameIndex: number}} options
	 */
	startAnim(options = {}) {
		this.stopAnim();
		onWindowResize();
		const frameCount = this.animation.frames.length;
		this.animation.currentFrameIndex = options.frameIndex ? Math.floor(options.frameIndex - Math.floor(options.frameIndex / frameCount) * frameCount) : 0;
		if (frameCount > 1) {
			this.animInterval = setInterval(() => {
				if (!this.animation) return this.stopAnim();
				this.animation.nextFrame();
				this.refresh();
			}, this.animation.frameDuration);
		}
		else {
			this.refresh();
		}
	}
	stopAnim() {
		clearInterval(this.animInterval);
		this.animInterval = null;
	}

	////////////////////////
	// RGB Axis at the corner
	////////////////////////

	showAxis() {
		displayAxis();
	}

	hideAxis() {
		removeAxis();
	}

	toggleAxis() {
		if (!wireAxis?.length) {
			this.showAxis();
		}
		else {
			this.hideAxis();
		}
	}
}
