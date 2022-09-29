import { EventEmitter } from './EventEmitter.js';

export class RGBColor {
	/** @type {number} */
	red;
	/** @type {number} */
	green;
	/** @type {number} */
	blue;

	/**
	 * @param {number} red
	 * @param {number} green
	 * @param {number} blue
	 */
	constructor(red, green, blue) {
		this.red = red;
		this.green = green;
		this.blue = blue;
	}

	/** @param {number} decimal */
	static fromDecimal(decimal) { return new RGBColor(decimal >> 16, (decimal >> 8) & 0xFF, decimal & 0xFF); }

	toDecimal() { return (this.red << 16) + (this.green << 8) + this.blue; }

	/** @param {string} hex */
	static fromHex(hex) {
		if (!hex) return new RGBColor(0, 0, 0);
		if (hex.startsWith('#')) hex = hex.substring(1);
		return RGBColor.fromDecimal(parseInt(hex, 16));
	}

	toHex() {
		var hex = this.toDecimal().toString(16);
		while (hex.length < 6) hex = '0' + hex;
		return '#' + hex;
	}

	clone() {
		return new RGBColor(this.red, this.green, this.blue);
	}
}

export class ColorPicker extends EventEmitter {
	/** @type {RGBColor} */
	color;
	/** @type {HTMLElement} */
	container;
	/** @type {HTMLInputElement} */
	picker;

	EVENTS = {
		PICKUP_COLOR: 'pickup',
	}

	/** @param {HTMLElement} container */
	constructor() {
		super();
		this.color = undefined;
		this.container = undefined;
		this.picker = undefined;
		this.registerEvent(this.EVENTS.PICKUP_COLOR, false);
	}

	/** @param {HTMLElement} container */
	init(container) {
		this.container = container;
		this.picker = container.querySelector('#pickColor');
		this.picker?.addEventListener('change', () => this.setColor(RGBColor.fromHex(this.picker.value)));
		container.querySelector('#redButton')?.addEventListener('click', (ev) => this.onColorClicked(ev.target, 'red'));
		container.querySelector('#greenButton')?.addEventListener('click', (ev) => this.onColorClicked(ev.target, 'green'));
		container.querySelector('#blueButton')?.addEventListener('click', (ev) => this.onColorClicked(ev.target, 'blue'));
		if (this.color == undefined) {
			this.color = RGBColor.fromHex(container.dataset.color);
		}
		else {
			setColor(this.color);
		}
	}

	#emitPickEvent() { this.emitEvent(this.EVENTS.PICKUP_COLOR, this.color.clone()); }

	/**
	 * @param {RGBColor} color
	 * @param {boolean} emitEvent
	 */
	setColor(color, emitEvent = true) {
		if (typeof color === 'string') {
			color = RGBColor.fromHex(color);
		} else if (typeof color === 'number') {
			color = RGBColor.fromDecimal(color);
		} else if (!(color instanceof RGBColor)) {
			throw new Error('Invalid color type, should be RGBColor');
		}
		this.color = color;
		this.updateButtonPicker();
		if (emitEvent) this.#emitPickEvent();
	}

	getDecimal() { return this.color.toDecimal(); }

	/**
	 * @param {HTMLElement} element
	 * @param {string} colorName
	 */
	setButtonActivated(button, color) {
		if (color === 0 || color === 'grey') {
			button.classList.add('grey');
			button.classList.remove('red', 'green', 'blue');
		} else {
			button.classList.remove('grey');
			button.classList.add(color);
		}
	}

	/**
	 * @param {HTMLElement} button
	 * @param {string} colorName
	 */
	onColorClicked(button, colorName) {
		if (button.classList.contains('grey')) {
			this.setButtonActivated(button, colorName);
			switch (colorName) {
				case 'red':
					this.color.red = 255;
					break;
				case 'green':
					this.color.green = 255;
					break;
				case 'blue':
					this.color.blue = 255;
					break;
			}
		} else {
			this.setButtonActivated(button, 'grey');
			switch (colorName) {
				case 'red':
					this.color.red = 0;
					break;
				case 'green':
					this.color.green = 0;
					break;
				case 'blue':
					this.color.blue = 0;
					break;
			}
		}
		this.setColor(this.color);
	}

	updateButtonPicker() {
		this.setButtonActivated(this.container.querySelector('#redButton'), this.color.red > 127 ? 'red' : 'grey');
		this.setButtonActivated(this.container.querySelector('#greenButton'), this.color.green > 127 ? 'green' : 'grey');
		this.setButtonActivated(this.container.querySelector('#blueButton'), this.color.blue > 127 ? 'blue' : 'grey');
		this.picker.value = this.color.toHex();
	}
}