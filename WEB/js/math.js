// Math equation
// by Jiogo18
// for @Jig0ll

var MinMax = (min, x, max) => Math.max(min, Math.min(x, max));

const n = '[\\d\\.]';
const reel = `[\\+\\-]?\\.?\\d+\\.?\\d*`;
const op = '\\+\\-\\*\\/';
const MATH = {};
function listCompare(strOperator, funcOperator) {
	// 10d6 >= 3 = (n: 5 6 4 ~~1~~ ~~1~~ 3 6 3 4 6) = 8
	return [
		`(\\d[\\d \\.${op}]*) *${strOperator} *(${n}+)`,
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
		`${strFunction}\\\( *${param.join(' *, *')} *\\\)`,
		match => {
			const args = new Array(nbArgs).fill(0).map((v, i) => parseFloat(match[1 + i]) || undefined);
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
		[/ +/, () => ' '],
	],
	[
		[`[${op}]*~~[\\d\\. ${op}]*~~`, () => '', 'before'], //deleted number
	],
	[
		[`\\(n: [\\d \\.${op}]*\\)`, match => match[0].match(new RegExp(`${n}+`, 'g'))?.length || 0], //deleted number
	],
	[
		// specialCalc
		[...listCompare('>', (a = 0, b = 0) => a > b), 'after'],
		listCompare('>=', (a = 0, b = 0) => a >= b),
		listCompare('==?', (a = 0, b = 0) => a == b),
		listCompare('<=', (a = 0, b = 0) => a <= b),
		listCompare('<', (a = 0, b = 0) => a < b),
	],
	[
		// transfoOperator
		pairOperation('||', (a = 0, b = 0) => a || b),
		pairOperation('&&', (a = 0, b = 0) => a && b),
	],
	[
		// transfoMult
		pairOperation('/', (a = 0, b = 0) => a / b), // 1 / 2 * 3
		pairOperation('*', (a = 0, b = 0) => a * b),
	],
	[
		// transfoAddition
		pairOperation('+', (a = 0, b = 0) => a + b),
		pairOperation('-', (a = 0, b = 0) => a - b),
		pairOperation(' ', (a = 0, b = 0) => a + b),
	],
	[
		// transfo fonctions
		funcOperation('max', 2, (a = 0, b = 0) => Math.max(a, b)),
		funcOperation('min', 2, (a = 0, b = 0) => Math.min(a, b)),
		funcOperation('abs', 1, (a = 0) => Math.abs(a)),
		funcOperation('sqrt', 1, (a = 0) => Math.sqrt(a)),
		funcOperation('round', 1, (a = 0) => Math.round(a)),
		funcOperation('floor', 1, (a = 0) => Math.floor(a)),
		funcOperation('ceil', 1, (a = 0) => Math.ceil(a)),
		funcOperation('rgb', 3, (r = 0, g = 0, b = 0) => (MinMax(0, r, 255) << 16) | (MinMax(0, g, 255) << 8) | MinMax(0, b, 255)),
	],
	[
		// transfoAddition
		[`\\( *\\+? *(${reel}) *\\)`, match => match[1]],
		[`\\( *\\- *(${reel}) *\\)`, match => -match[1]],
		//[/\([ \+\-\*\/]*\)/, () => 0],
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
			return { rule, match: equation.match(new RegExp(rule[0], 'g')) };
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
		const regex = new RegExp(rule[0]);
		const replacement = rule[1](equation.match(regex));
		equation = equation.replace(result.match[0], replacement);

		if (i++ > 100) throw new Error('équation trop grande ou impossible à résoudre');
	}
	return equation;
}
