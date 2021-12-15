// Math equation
// by Jiogo18
// for @Jig0ll

const MinMax = (min, x, max) => Math.max(min, Math.min(x, max));
const distance = (a, b) => Math.sqrt(a * a + b * b);
const distancePointOrigine = p => Math.sqrt(p.re * p.re + p.im * p.im);
const distancePoints = (p1, p2) => Math.sqrt(Math.pow(p1.re - p2.re, 2) + Math.pow(p1.im - p2.im, 2));
const pointCart = (re, im) => new Object({ re, im });
const pointExpo = (r, angle) => new Object({ r, angle });
const expToCart = (angle, r) => pointCart(r * Math.cos(angle), r * Math.sin(angle));
const cartToExp = p => pointExpo(distancePointOrigine(p), Math.atan(p.im / p.re) + (p.re < 0 && Math.PI));
const valideOctet = v => MinMax(0, Math.round(v), 255);
const modulo = (a, b) => a - Math.floor(a / b) * b;

class Color {
	decValue;
	get r() {
		return (this.decValue & 0xff0000) >> 16;
	}
	get g() {
		return (this.decValue & 0x00ff00) >> 8;
	}
	get b() {
		return this.decValue & 0x0000ff;
	}
	/**
	 * @param {number} value
	 */
	constructor(value) {
		this.decValue = value;
	}
	static fromRGB(r, g, b) {
		return new Color((valideOctet(r) << 16) | (valideOctet(g) << 8) | valideOctet(b));
	}
	hue_rotation(angle) {
		var cosA = Math.cos(angle);
		var sinA = Math.sin(angle);
		var ratio_partage = (1 / 3) * (1 - cosA);
		var sqrt1_3 = Math.sqrt(1 / 3);
		var m0 = ratio_partage + cosA;
		var m1 = ratio_partage - sqrt1_3 * sinA;
		var m2 = ratio_partage + sqrt1_3 * sinA;
		var rx = this.r * m0 + this.g * m1 + this.b * m2;
		var gx = this.r * m2 + this.g * m0 + this.b * m1;
		var bx = this.r * m1 + this.g * m2 + this.b * m0;
		return Color.fromRGB(rx, gx, bx);
	}
	lumiere(lumiere) {
		return Color.fromRGB(this.r * lumiere, this.g * lumiere, this.b * lumiere);
	}
}

const n = '[\\d\\.]';
const reel = `[\\+\\-]?\\.?\\d+\\.?\\d*(?:e-\\d+)?`;
const op = '\\+\\-\\*\\/';
const prefix_nochar = '(?<=^|[^\\w\\d])';
const suffix_nochar = '(?=[^\\w\\d]|$)';
const MATH = {};
function listCompare(strOperator, funcOperator) {
	// 10d6 >= 3 = (n: 5 6 4 ~~1~~ ~~1~~ 3 6 3 4 6) = 8
	return [
		`(\\d[\\d \\.${op}]*) *${strOperator} *(${reel})`,
		match => {
			return (
				'(n: ' +
				match[1]
					.replace(/ +/, ' ')
					.replace(/^ /, '')
					.replace(/ $/, '')
					.split(' ')
					.map(v => parseFloat(v) || 0)
					.map(v => (funcOperator(v, match[2]) ? v : `~~${v}~~`))
					.join(' ') +
				')'
			);
		},
	];
}
function funcOperation(strFunction, nbArgs, funcOperator) {
	var param = new Array(nbArgs).fill(`(${reel})`);
	return [
		`${prefix_nochar}${strFunction}\\\( *${param.join(' *, *')} *\\\)`,
		match => {
			const args = new Array(nbArgs).fill(0).map((v, i) => parseFloat(match[1 + i]) ?? undefined);
			return funcOperator(...args);
		},
	];
}
function pairOperation(strOperator, funcOperator) {
	strOperator = strOperator.replace(/(.)/g, '\\$1');
	return [
		`(${reel}) *${strOperator} *(${reel})`,
		match => '+' + funcOperator(parseFloat(match[1]) || undefined, parseFloat(match[2]) || undefined),
	];
}

const transfoCalc = [
	[
		// transfo sign
		[/\+ *\+/, () => '+'],
		[/\- *\-/, () => '+'],
		[/\+ *\-/, () => '-'],
		[/\- *\+/, () => '-'],
		[/\* *\+/, () => '*'],
		[/\/ *\+/, () => '/'],
		[/ {2}/, () => ' '],
	],
	[
		[`[${op}]*~~[\\d\\. ${op}]*~~`, () => '', 'before'], //deleted number
		[`#([\\dA-Fa-f]+)`, match => parseInt(match[1], 16)], //hexadecimal
		[`0x([\\dA-Fa-f]+)`, match => parseInt(match[1], 16)], //hexadecimal
		[`0[bB]([01]+)`, match => parseInt(match[1], 2)], //binaire
		[`${prefix_nochar}true${suffix_nochar}`, () => 1],
		[`${prefix_nochar}false${suffix_nochar}`, () => 0],
	],
	[
		[`\\(n: [\\d \\.${op}]*\\)`, match => match[0].match(new RegExp(`${n}+`, 'g'))?.length || 0], //deleted number
	],
	[
		// specialCalc
		[...listCompare('>', (a = 0, b = 0) => a > b), 'after'],
		listCompare('>=', (a = 0, b = 0) => a >= b),
		listCompare('==?', (a = 0, b = 0) => a == b),
		listCompare('!=', (a = 0, b = 0) => a == b),
		listCompare('<=', (a = 0, b = 0) => a <= b),
		listCompare('<', (a = 0, b = 0) => a < b),
	],
	[
		// transfoMult
		pairOperation('/', (a = 0, b = 0) => a / b), // 1 / 2 * 3
		pairOperation('*', (a = 0, b = 0) => a * b),
		pairOperation('%', (a = 0, b = 0) => modulo(a, b)),
	],
	[
		// transfoAddition
		pairOperation('+', (a = 0, b = 0) => a + b),
		pairOperation('-', (a = 0, b = 0) => a - b),
		pairOperation(' ', (a = 0, b = 0) => a + b),
	],
	[
		// transfo fonctions
		funcOperation('max', 2, (a, b) => Math.max(a, b)),
		funcOperation('min', 2, (a, b) => Math.min(a, b)),
		funcOperation('minmax', 3, (min, x, max) => MinMax(min, x, max)),
		funcOperation('range', 3, (min, x, max) => (min <= x && x <= max ? 1 : 0)),
		funcOperation('abs', 1, a => Math.abs(a)),
		funcOperation('sqrt', 1, a => Math.sqrt(a)),
		funcOperation('pow', 2, (a, b) => Math.pow(a, b)),
		funcOperation('exp', 1, x => Math.exp(x)),
		funcOperation('round', 1, a => Math.round(a)),
		funcOperation('floor', 1, a => Math.floor(a)),
		funcOperation('ceil', 1, a => Math.ceil(a)),
		funcOperation('random', 0, () => Math.random()),
		funcOperation('pi', 0, () => 3.141593),
		funcOperation('cos', 1, a => Math.cos(a)),
		funcOperation('sin', 1, a => Math.sin(a)),
		funcOperation('tan', 1, a => Math.tan(a)),
		funcOperation('acos', 1, a => Math.acos(a)),
		funcOperation('asin', 1, a => Math.asin(a)),
		funcOperation('atan', 1, a => Math.atan(a)),
		funcOperation('modulo', 2, (a, b = 1) => modulo(a, b)),
		funcOperation('angle_complexe', 2, (a, b) => modulo(Math.atan(b / a) + (a < 0 ? Math.PI : 0), 2 * Math.PI)),
		funcOperation('triangle', 4, (x, x0, y0, pente) => y0 - pente * Math.abs(x - x0)),
		funcOperation('distance', 2, (x1, y1) => distance(x1, y1)),
		funcOperation('heaviside', 1, t => 0 <= t),
		funcOperation('porte', 3, (t, t1, t2) => t1 <= t && t <= t2),
		funcOperation('pente_cosale', 1, t => (0 <= t ? t : 0)),
		funcOperation('rgb', 3, (r, g, b) => (MinMax(0, r, 255) << 16) | (MinMax(0, g, 255) << 8) | MinMax(0, b, 255)),
		funcOperation('red', 1, c => (c & 0xff0000) >> 16),
		funcOperation('green', 1, c => (c & 0x00ff00) >> 8),
		funcOperation('blue', 1, c => c & 0x0000ff),
		funcOperation('huerotate', 2, (c, angle) => new Color(c).hue_rotation(angle).decValue),
		funcOperation('lumiere', 2, (c, lumiere) => new Color(c).lumiere(lumiere).decValue),
	],
	[
		// transfoAddition
		[`\\( *\\+? *(${reel}) *\\)`, match => match[1]],
		[`\\( *\\- *(${reel}) *\\)`, match => -match[1]],
		//[/\([ \+\-\*\/]*\)/, () => 0],
	],
	[
		// binary manipulation
		pairOperation('<<', (a = 0, b = 0) => a << b),
		pairOperation('>>', (a = 0, b = 0) => a >> b),
		pairOperation('&', (a = 0, b = 0) => a & b),
		pairOperation('|', (a = 0, b = 0) => a | b),
		pairOperation('^', (a = 0, b = 0) => a ^ b),
	],
	[
		// transfoOperator
		pairOperation('||', (a = 0, b = 0) => a || b),
		pairOperation('&&', (a = 0, b = 0) => a && b),
	],
];

/**
 * @param {string} equation Line
 * @param {string[]|undefined} equaStep Empty array
 */
MATH.parseMath = (equation, equaStep) => {
	equation = equation.replace(/^ */, '').replace('&gt;', '>').replace('&lt;', '<');
	//`!r 1++2 * 5` donne `1+2*5`
	equaStep ??= [];
	equaStep.push(equation);
	var previousLine;
	do {
		previousLine = equation;
		for (const transfoGrp of transfoCalc) {
			const equationTransformed = applyTransfo(equation, transfoGrp);
			if (equationTransformed != equation) {
				if (transfoGrp.some(t => t[2] === 'after')) {
					// l'une des transfos demande l'update avec la nouvelle version
					equaStep.push(equationTransformed);
				} else if (transfoGrp.some(t => t[2] === 'before')) {
					// l'une des transfos demande l'update avec la version précédente
					if (equaStep[equaStep.length - 1] !== equation) equaStep.push(equation);
				} else {
					equaStep.push(equationTransformed);
				}
				equation = equationTransformed;
				break;
			}
		}
	} while (equation != previousLine && equation.length < 100000);

	if (equation == '') equation = 0;
	equaStep.push(equation);
	return equation;
};

/**
 * @param {string} equation
 * @param {[string,Function][]} rules
 */
function getFirstMatch(equation, rules) {
	return rules
		.map(rule => {
			return { rule, match: equation.match(new RegExp(rule[0], 'ig')) };
		})
		.filter(result => result.match)
		.sort((a, b) => Math.sign(a.match.index, b.match.index))?.[0];
}

/**
 * @param {string} equation
 * @param {[string,Function][]} rules
 */
function applyTransfo(equation, rules) {
	var i = 0;
	var result;
	while ((result = getFirstMatch(equation, rules))) {
		const rule = result.rule;
		const regex = new RegExp(rule[0], 'i');
		const replacement = rule[1](equation.match(regex));
		equation = equation.replace(result.match[0], replacement);

		if (i++ > 100) throw new Error('équation trop grande ou impossible à résoudre');
	}
	return equation;
}
