/**
 * Sélectionner des LEDs dans la matrice
 */
export const selectMatrix = {
	/**
	 * Tableau de points 2D (x, y)
	 * @type {{x:number,y:number}[]}
	 */
	selected2D: [],

	/**
	 * Dernière LED cliquée
	 * La LED n'est pas forcément sélectionnée
	 */
	lastClicked: { x: 0, y: 0 },

	/**
	 * @type {SVGCircleElement[]}
	 */
	circles: [],

	/**
	 * Actualiser les éléments SVG de la matrice
	 */
	updateLedCircles() {
		this.circles = Array.from(document.getElementById("matrix").getElementsByTagName("circle"));
	},

	/**
	 * @param {{x:number,y:number}} pos
	 */
	getLedElement(pos) {
		return this.circles[pos.y * 8 + pos.x];
	},

	/**
	 * Couleurs de contour des LEDs sélectionnées
	 * 
	 * [ pas sélectionnée, sélectionnée, sélectionnée et dernière cliquée ]
	 */
	activeColors: ['#3e4f51', '#328979', '#3da1a0'],
	/**
	 * @param {{x:number,y:number}} pos
	 * @param {number} active
	 */
	setLedActive(pos, active) {
		this.getLedElement(pos)?.setAttribute("stroke", this.activeColors[active] || this.activeColors[0]);
	},

	/**
	 * @param {{x:number,y:number}} pos1
	 * @param {{x:number,y:number}} pos2
	 */
	posAreEquals(pos1, pos2) {
		return pos1.x === pos2.x && pos1.y === pos2.y;
	},

	/**
	 * @param {{x:number,y:number}} pos
	 */
	findSelectedLedIndex(pos) {
		return this.selected2D.findIndex(p2 => this.posAreEquals(p2, pos));
	},

	/**
	 * @param {{x:number,y:number}} pos
	 * @param {boolean} lastClicked
	 */
	addLed(pos, lastClicked = false) {
		if (this.findSelectedLedIndex(pos) === -1) {
			this.selected2D.push(pos);
		}
		if (lastClicked || this.posAreEquals(this.lastClicked, pos)) {
			if (this.findSelectedLedIndex(this.lastClicked) !== -1)
				this.setLedActive(this.lastClicked, 1);
			this.lastClicked = pos;
			this.setLedActive(pos, 2);
		}
		else {
			this.setLedActive(pos, 1);
		}
	},

	/**
	 * @param {{x:number,y:number}} pos
	 */
	removeLed(pos) {
		const index = this.findSelectedLedIndex(pos);
		if (index !== -1) {
			this.selected2D.splice(index, 1);
			this.setLedActive(pos, 0);
		}
	},

	removeAll() {
		for (const pos of this.selected2D) {
			this.setLedActive(pos, 0);
		}
		this.selected2D = [];
	},

	/**
	 * @param {{x:number,y:number}} pos1
	 * @param {{x:number,y:number}} pos2
	 */
	addSquare(pos1, pos2) {
		const xMin = Math.min(pos1.x, pos2.x);
		const xMax = Math.max(pos1.x, pos2.x);
		const yMin = Math.min(pos1.y, pos2.y);
		const yMax = Math.max(pos1.y, pos2.y);
		for (let x = xMin; x <= xMax; x++) {
			for (let y = yMin; y <= yMax; y++) {
				this.addLed({ x, y });
			}
		}
	},
};
