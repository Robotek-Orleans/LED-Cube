import { XLENGTH, YLENGTH, ZLENGTH, LEDFrame } from '../utils/frame.js';
import { ledCubeTools } from '../main.js';

/**
 * Functions to paint on the ledcube with a single frame
 * This util is used by games such as snake
 */

export var events = {
	/** Called when the viewer is created */
	onStart: () => { },
	/** Called when the viewer is connected to the ledcube */
	onReady: () => { },
};

/** @type {LEDFrame} */
export var uniqueFrame = undefined;

export class Vector3D {
	x;
	y;
	z;
	/**
	 * @param {number} x
	 * @param {number} y
	 * @param {number} z
	 */
	constructor(x, y, z) {
		this.x = x;
		this.y = y;
		this.z = z;
	}

	clone() { return new Vector3D(this.x, this.y, this.z); }

	/**
	 * @param {Vector3D} vector
	 */
	add(vector) {
		this.x += vector.x;
		this.y += vector.y;
		this.z += vector.z;
		return this;
	}

	equals(vec) { return this.x == vec.x && this.y == vec.y && this.z == vec.z; }
};

export class Pos3D extends Vector3D {
	constructor(x, y, z) { super(x, y, z); }

	clone() { return new Pos3D(this.x, this.y, this.z); }

	/**
	 * @param {Vector3D} vector
	 */
	add(vector) {
		super.add(vector);
		if (this.x < 0) this.x = 0;
		if (this.y < 0) this.y = 0;
		if (this.z < 0) this.z = 0;
		if (this.x >= XLENGTH) this.x = XLENGTH - 1;
		if (this.y >= YLENGTH) this.y = YLENGTH - 1;
		if (this.z >= ZLENGTH) this.z = ZLENGTH - 1;
		return this;
	}
};

ledCubeTools.addEventListener(ledCubeTools.EVENTS.PAGE_LOADED, async () => {
	uniqueFrame = ledCubeTools.cubeViewer.animation.frames[0];
	ledCubeTools.addEventListener(ledCubeTools.EVENTS.LOGGED, onWebSocketConnected);
	ledCubeTools.cubeViewer.showAxis();
	events.onStart();
	console.log('uniqueFramePainter loaded');
});

async function onWebSocketConnected() {
	const answer = await ledCubeTools.ledcubeWS.syncViewer(ledCubeTools.cubeViewer, { syncAnimation: false });
	if (answer) {
		if (answer.currentName != '') {
			await ledCubeTools.ledcubeWS.sendLedcube.set_animation_frame_count(1);
		}
		uniqueFrame = ledCubeTools.cubeViewer.animation.frames[0];
	}
	else {
		ledCubeTools.ledcubeWS.sendLedcube.set_animation_frame_count(1);
	}
	events.onReady();
	console.log('uniqueFramePainter ready');
}

/**
 * @param {Pos3D} pos
 * @param {number} color
 * @param {boolean} refresh
 */
export function paintLed(pos, color, refresh = true) {
	uniqueFrame.setColor(pos.x, pos.y, pos.z, color);
	if (refresh) ledCubeTools.cubeViewer.refresh();
	if (ledCubeTools.ledcubeWS.isConnected()) {
		ledCubeTools.ledcubeWS.sendLedcube.setLed({ x: pos.x, y: pos.y, z: pos.z, color: color });
	}
}

export function paintAxis() {
	paintLed(new Pos3D(0, 0, 0), 0xFFFFFF, false)
	paintLed(new Pos3D(1, 0, 0), 0xFF0000, false)
	paintLed(new Pos3D(0, 1, 0), 0x00FF00, false)
	paintLed(new Pos3D(0, 0, 1), 0x0000FF, true)
}

/**
 * @param {string} key
 */
export function getVectorFromKey(key) {
	switch (key) {
		case 'ArrowUp':
		case 'z':
			return new Pos3D(0, 1, 0);
		case 'ArrowDown':
		case 's':
			return new Pos3D(0, -1, 0);
		case 'ArrowRight':
		case 'd':
			return new Pos3D(1, 0, 0);
		case 'ArrowLeft':
		case 'q':
			return new Pos3D(-1, 0, 0);
		case 'Shift':
		case 'a':
			return new Pos3D(0, 0, -1);
		case ' ':
		case 'e':
			return new Pos3D(0, 0, 1);
		default:
			return new Pos3D(0, 0, 0);
	}
}

export default {
	ledCubeTools,
	uniqueFrame,
	events,
	Vector3D,
	Pos3D,
	paintLed,
	paintAxis,
	getVectorFromKey,
};
