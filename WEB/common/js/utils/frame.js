import { RGBColor } from './ColorPicker.js';

export const ZLENGTH = 8;
export const YLENGTH = 8;
export const XLENGTH = 8;

export class LEDFrame {
	/**
	 * @type {number[][][]} 3D array with decimal colors
	 * Size : 8x8x8
	 */
	colors = [];

	constructor() {
		this.colors = [];
		for (let z = 0; z < ZLENGTH; z++) {
			this.colors[z] = [];
			for (let y = 0; y < YLENGTH; y++) {
				this.colors[z][y] = [];
				for (let x = 0; x < XLENGTH; x++) {
					this.colors[z][y][x] = 0;
				}
			}
		}
	}

	/**
	 * @param {number} x
	 * @param {number} y
	 * @param {number} z
	 * @param {number} color
	 */
	setColor(x, y, z, color) { this.colors[z][y][x] = color; }

	/**
	 * @param {number} x
	 * @param {number} y
	 * @param {number} z
	 * @returns {number}
	 */
	getColor(x, y, z) { return this.colors[z][y][x]; }

	/**
	 * @param {number} x
	 * @param {number} y
	 * @param {number} z
	 * @param {RGBColor} color
	 */
	setRGBColor(x, y, z, color) { this.colors[z][y][x] = color.toDecimal(); }

	/**
	 * @param {number} x
	 * @param {number} y
	 * @param {number} z
	 */
	getRGBColor(x, y, z) { return RGBColor.fromDecimal(this.colors[z][y][x]); }

	/**
	 * @param {number} color
	 */
	fill(color) { this.colors = this.colors.map(layer => layer.map(row => row.map(() => color))); }

	copy() {
		const frame = new LEDFrame();
		frame.colors = this.colors.map(layer => layer.map(row => row.map(color => color)));
		return frame;
	}

	/**
	 * @param {string} axis
	 * @param {number} index
	 */
	getPlan(axis, index) {
		switch (axis.toLowerCase()) {
			case 'x':
				return this.colors.map(layer => layer.map(row => row[index]));
			case 'y':
				return this.colors.map(layer => layer[index]);
			case 'z':
				return this.colors[index];
			default:
				throw new Error(`Invalid axis: ${axis}`);
		}
	}

	/**
	 * @param {string} axis
	 * @param {number} index
	 */
	getRGBPlan(axis, index) {
		return this.getPlan(axis, index).map(row => row.map(color => RGBColor.fromDecimal(color)));
	}

	/**
	 * @param {string} axis
	 * @param {number} index
	 * @param {number[][]} plan
	 */
	setPlan(axis, index, plan) {
		switch (axis.toLowerCase()) {
			case 'x':
				plan.forEach((row, z) => row.forEach((color, y) => this.setColor(index, y, z, color)));
				break;
			case 'y':
				plan.forEach((row, x) => row.forEach((color, z) => this.setColor(z, index, x, color)));
				break;
			case 'z':
				plan.forEach((row, y) => row.forEach((color, x) => this.setColor(x, y, index, color)));
				break;
			default:
				throw new Error(`Invalid axis: ${axis}`);
		}
	}

	/**
	 * @param {string} axis
	 * @param {number} index
	 * @param {number[][]} plan
	 */
	setRGBPlan(axis, index, plan) {
		this.setPlan(axis, index, plan.map(row => row.map(color => color.toDecimal())));
	}
}

export class LEDAnimation {
	/** @type {LEDFrame[]} */
	frames = [];
	/** @type {number} */
	currentFrameIndex = 0;
	/** @type {number} */
	frameDuration = 50;
	/** @type {string} */
	name = 'New animation';

	constructor() {
		this.frames = [];
		this.addFrame(0);
	}

	/**
	 * @param {number} index
	 */
	addFrame(index = null) {
		const frame = new LEDFrame();
		if (index == null)
			this.frames.push(frame);
		else
			this.frames.splice(index, 0, frame);
		return this;
	}

	nextFrame() {
		this.currentFrameIndex++;
		if (this.currentFrameIndex >= this.frames.length)
			this.currentFrameIndex = 0;
	}

	/**
	 * @param {number} index
	 */
	removeFrame(index) { this.frames.splice(index, 1); }

	get currentFrame() { return this.frames[this.currentFrameIndex]; }

	/**
	 * @param {number[][][][]} frames
	 */
	static fromDecimalArrays(frames) {
		const animation = new LEDAnimation();
		animation.frames = frames.map(array => {
			const frame = new LEDFrame();
			frame.colors = array;
			return frame;
		});
		return animation;
	}

	toDecimalArrays() { return this.frames.map(frame => frame.colors); }

	copy() {
		const animation = new LEDAnimation();
		animation.frames = this.frames.map(frame => frame.copy());
		animation.currentFrameIndex = this.currentFrameIndex;
		animation.frameDuration = this.frameDuration;
		animation.name = this.name;
		return animation;
	}
}