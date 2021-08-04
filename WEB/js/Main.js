let selectedPlanDirection = "X" // X , Y ou Z
let selectedPlanNumber = 1 // entre 1 et 8
let selectedFrame = 1 // entre 1 et N frame
let selected2D = [] // tableau en 2D avec des valeurs entre 0 et 7
let shiftpressed = false

let framecontent = []

function Draw2DMatrix() {
    const matrix = document.getElementById("matrix");
    matrix.innerHTML = "";
    if (selectedPlanDirection === "X") {
        for (let z = 0; z < 8; z++) {
            for (let y = 0; y < 8; y++) {
                matrix.innerHTML += "<svg onclick=\'SelectLED(" + (y + z * 8) + ")\'><circle stroke-width=\'3\' stroke=\'#3e4f51\' fill=\'" + framecontent[selectedFrame - 1][selectedPlanNumber - 1][y][z] + "\' cx=\'50%\' cy=\'50%\' r=\'20\'></circle></svg>";
            }
        }
    } else if (selectedPlanDirection === "Y") {
        for (let z = 0; z < 8; z++) {
            for (let x = 0; x < 8; x++) {
                matrix.innerHTML += "<svg onclick=\'SelectLED(" + (x + z * 8) + ")\'><circle stroke-width=\'3\' stroke=\'#3e4f51\' fill=\'" + framecontent[selectedFrame - 1][x][selectedPlanNumber - 1][z] + "\' cx=\'50%\' cy=\'50%\' r=\'20\'></circle></svg>";
            }
        }
    } else {
        for (let y = 0; y < 8; y++) {
            for (let x = 0; x < 8; x++) {
                matrix.innerHTML += "<svg onclick=\'SelectLED(" + (x + y * 8) + ")\'><circle stroke-width=\'3\' stroke=\'#3e4f51\' fill=\'" + framecontent[selectedFrame - 1][x][y][selectedPlanNumber - 1] + "\' cx=\'50%\' cy=\'50%\' r=\'20\'></circle></svg>";
            }
        }
    }
    selected2D = []
    document.getElementById("pickColor").value = "#000000"

}

function setColor() {
    const color = document.getElementById("pickColor").value
    const LEDelements = document.getElementById("matrix").getElementsByTagName("circle")
    for (let i = 0; i < selected2D.length; i++) {
        const LEDelement = LEDelements[selected2D[i][1] * 8 + selected2D[i][0]]
        LEDelement.setAttribute("fill", color)
        if (selectedPlanDirection === "X") {
            framecontent[selectedFrame - 1][selectedPlanNumber - 1][selected2D[i][0]][selected2D[i][1]] = color
            cube_set_color(selectedPlanNumber - 1, selected2D[i][0], selected2D[i][1], color.replace('#', '0x'))
        } else if (selectedPlanDirection === "Y") {
            framecontent[selectedFrame - 1][selected2D[i][0]][selectedPlanNumber - 1][selected2D[i][1]] = color
            cube_set_color(selected2D[i][0], selectedPlanNumber - 1, selected2D[i][1], color.replace('#', '0x'))
        } else {
            framecontent[selectedFrame - 1][selected2D[i][0]][selected2D[i][1]][selectedPlanNumber - 1] = color
            cube_set_color(selected2D[i][0], selected2D[i][1], selectedPlanNumber - 1, color.replace('#', '0x'))
        }
    }
}

function ckeckIndexselected2D(findArray){
    let index = -1
    for(let i =0; i< selected2D.length; i++){
        if(selected2D[i][0] === findArray[0] && selected2D[i][1] === findArray[1]){
            index = i
            break
        }
    }
    return index
}

function SelectLED(LED) {
    const LEDelements = document.getElementById("matrix").getElementsByTagName("circle")
    const LEDelement = LEDelements[LED]
    const x = LED % 8
    const y = (LED - x) / 8
    const index = ckeckIndexselected2D([x,y])
    if(index >= 0){
        selected2D.splice(index,1)
        LEDelement.setAttribute("stroke", "#3e4f51")
        return
    }
    if (shiftpressed) {
        selected2D.push([x,y])
    } else {
        for (var i = 0; i < 64; i++) {
            LEDelements[i].setAttribute("stroke", "#3e4f51")
        }
        selected2D = [ [x,y] ]
    }
    LEDelement.setAttribute("stroke", "#3aaa96")

    document.getElementById("pickColor").value = LEDelement.getAttribute("fill");
}

function SelectPlan() {
    selectedPlanDirection = getRadioSelectedValue("axe")
    const range = document.getElementById("planNumber")
    selectedPlanNumber = parseInt(range.value)
    displaySelectedPlan(selectedPlanDirection, selectedPlanNumber)
    Draw2DMatrix()
}

function clear3DViewPlan() {
    scene.remove(wireFrame)
}

function getRadioSelectedValue(radioName) {
    var radios = document.getElementsByName(radioName);
    for (var i = 0; i < radios.length; i++) {
        if (radios[i].checked) {
            return radios[i].value
        }
    }
    return null
}

function nextframe() {

}

function previousframe() {

}

function slidertoframe() {

}

function addframebefore() {
    //index
    index = parseInt(document.getElementById("numframebefore").value) - 1
    addframe(index)
    if (index <= (selectedFrame - 1)) {
        selectedFrame++;
    }

}

function addframeafter() {
    //index +1
}

function gotoframe() {

}

function loadframe() {

}

function removeframe() {

}

function addframe(index) {
    framecontent.splice(index, 0, [])
    for (let i = 0; i < RES; i++) {
        framecontent[index].push([])
        for (let j = 0; j < RES; j++) {
            framecontent[index][i].push([])
            for (let k = 0; k < RES; k++) {
                framecontent[index][i][j].push(0)
            }
        }
    }
    document.getElementById("frameNumber").innerHTML = framecontent.length.toString()
}

function init() {
    addframe(0)
    Draw2DMatrix()
    initGL()
    animate()
    document.getElementsByName("axe").forEach(item => {
        item.addEventListener("click", SelectPlan)
    })
    selectedPlanDirection = getRadioSelectedValue("axe")
    let select = document.getElementById("planNumber")
    selectedPlanNumber = parseInt(select.options[select.selectedIndex].value)
}


function nextplan(){

}

function previousplan(){

}

function nextaxe(){

}

function previousaxe(){

    
}


document.addEventListener('keydown', (e) => {
    switch(e.key){
        case "Shift":
            shiftpressed = true;
            break;
    }
    console.log(e.key)
});

document.addEventListener('keyup', (e) => {
    if (e.key === "Shift") {
        shiftpressed = false
    }
});

/*

z: plan précédent
s: plan suivant
q: axe précédent
d: axe suivant
a: frame précédente
e: frame suivante


*/