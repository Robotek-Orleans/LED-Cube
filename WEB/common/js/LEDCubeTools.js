import { ledcubeWS } from './ws/main.js';
import { CubeViewer } from './viewer/cubeViewer.js';
import { ColorPicker, RGBColor } from './utils/ColorPicker.js';
import { EventEmitter } from './utils/EventEmitter.js';
import { LedCubeWS } from './ws/LedCubeWS.js';

/**
 * This is the main class to control the client side of the ledcube :
 * - ledcubeWS : the websocket connection to the server
 * - cubeViewer : the 3D viewer of the cube
 * - colorPicker : the color picker in the page
 * Generally, you can find me by typing `module.ledCubeTools` in the console.
 * If there is no need to use the 3D viewer, you can use ./ws/main.js instead.
 * 
 * Requires : bcrypt.js, pako.js, base64.js, three.min.js, OrbitControls.js, Detector.js, Viewerscript.js
 */
export class LEDCubeTools extends EventEmitter {
	/** @type {LedCubeWS} */
	ledcubeWS = ledcubeWS;
	/** @type {CubeViewer} */
	cubeViewer;
	/** @type {ColorPicker} */
	colorPicker;
	loaded = false;

	EVENTS = {
		LOGGED: 'logged',
		DISCONNECTED: 'disconnected',
		PAGE_LOADED: 'onLoaded',
		FRAMES_CHANGED: 'frameschanged',
		PICKUP_COLOR: 'pickup_color'
	}

	constructor() {
		super();
		this.cubeViewer = new CubeViewer();
		this.colorPicker = new ColorPicker();

		this.registerEvent(this.EVENTS.LOGGED);
		this.registerEvent(this.EVENTS.DISCONNECTED);
		this.registerEvent(this.EVENTS.PAGE_LOADED, true);
		this.registerEvent(this.EVENTS.FRAMES_CHANGED);
		this.registerEvent(this.EVENTS.PICKUP_COLOR);

		this.ledcubeWS.addEventListener(this.ledcubeWS.EVENTS.LOGGED, () => this.emitEvent(this.EVENTS.LOGGED));
		this.ledcubeWS.addEventListener(this.ledcubeWS.EVENTS.DISCONNECTED, () => this.emitEvent(this.EVENTS.DISCONNECTED));
		this.cubeViewer.addEventListener(this.cubeViewer.EVENTS.FRAMES_CHANGED, () => this.emitEvent(this.EVENTS.FRAMES_CHANGED));
		this.colorPicker.addEventListener(this.colorPicker.EVENTS.PICKUP_COLOR, /** @param {RGBColor} color */(color) => this.emitEvent(this.EVENTS.PICKUP_COLOR, color));

		window.addEventListener('load', () => {
			const cubeContainer = document.querySelector('#cube_container');
			if (!cubeContainer) {
				console.error(`Cube container not found`);
			} else {
				this.cubeViewer.init(cubeContainer);
			}

			const colorPickerContainer = document.getElementById("color_picker");
			if (colorPickerContainer) {
				this.colorPicker.init(colorPickerContainer);
			}

			this.emitEvent(this.EVENTS.PAGE_LOADED);
		});
	}

	get animation() {
		return this.cubeViewer.animation;
	}

	get sendLedCubeWS() {
		return this.ledcubeWS.sendLedcube;
	}

	stopAnimation = this.ledcubeWS.sendLedCubeWithNotification.animation.stop;
}