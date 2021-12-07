let selectedPlanDirection = "X" // X , Y ou Z
let selectedPlanNumber = 1 // entre 1 et 8
let selectedFrame = 1 // entre 1 et N frame
let selected2D = [] // tableau en 2D avec des valeurs entre 0 et 7
let shiftpressed = false

let copied2D = []
let copied3D = []


function paste2D() {
    if (!copied2D.length) return;
    switch (selectedPlanDirection) {
        case "X":
            for (let y = 0; y < 8; y++) {
                for (let z = 0; z < 8; z++) {
                    framecontent[selectedFrame - 1][selectedPlanNumber - 1][y][z] = copied2D[y][z]
                }
            }
            break;
        case "Y":
            for (let x = 0; x < 8; x++) {
                for (let z = 0; z < 8; z++) {
                    framecontent[selectedFrame - 1][x][selectedPlanNumber - 1][z] = copied2D[x][z]
                }
            }
            break;
        default:
            for (let x = 0; x < 8; x++) {
                for (let y = 0; y < 8; y++) {
                    framecontent[selectedFrame - 1][x][y][selectedPlanNumber - 1] = copied2D[x][y]
                }
            }
            break;
    }
    refresh2D()
}

function copy2D() {
    document.getElementById("paste2D").disabled = false
    copied2D = []
    switch (selectedPlanDirection) {
        case "X":
            for (let y = 0; y < 8; y++) {
                copied2D.push([])
                for (let z = 0; z < 8; z++) {
                    copied2D[y].push(framecontent[selectedFrame - 1][selectedPlanNumber - 1][y][z])
                }
            }
            break;
        case "Y":
            for (let x = 0; x < 8; x++) {
                copied2D.push([])
                for (let z = 0; z < 8; z++) {
                    copied2D[x].push(framecontent[selectedFrame - 1][x][selectedPlanNumber - 1][z])
                }
            }
            break;
        default:
            for (let x = 0; x < 8; x++) {
                copied2D.push([])
                for (let y = 0; y < 8; y++) {
                    copied2D[x].push(framecontent[selectedFrame - 1][x][y][selectedPlanNumber - 1])
                }
            }
            break;
    }
}


function copy3D() {
    document.getElementById("paste3D").disabled = false
    copied3D = []
    for (let x = 0; x < 8; x++) {
        copied3D.push([])
        for (let y = 0; y < 8; y++) {
            copied3D[x].push([])
            for (let z = 0; z < 8; z++) {
                copied3D[x][y].push(framecontent[selectedFrame - 1][x][y][z])
            }
        }
    }
}

function paste3D() {
    for (let x = 0; x < 8; x++) {
        for (let y = 0; y < 8; y++) {
            for (let z = 0; z < 8; z++) {
                framecontent[selectedFrame - 1][x][y][z] = copied3D[x][y][z]
            }
        }
    }
    refresh3D()
}

function Draw2DMatrix() {
    const matrix = document.getElementById("matrix");
    matrix.innerHTML = "";
    switch (selectedPlanDirection) {
        case "X":
            for (let z = 0; z < 8; z++) {
                for (let y = 0; y < 8; y++) {
                    matrix.innerHTML += "<svg onclick=\'SelectLED(" + (y + z * 8) + ")\'><circle stroke-width=\'3\' stroke=\'#3e4f51\' fill=\'" + framecontent[selectedFrame - 1][selectedPlanNumber - 1][y][z] + "\' cx=\'50%\' cy=\'50%\' r=\'20\'></circle></svg>";
                }
            }
            break;
        case "Y":
            for (let z = 0; z < 8; z++) {
                for (let x = 0; x < 8; x++) {
                    matrix.innerHTML += "<svg onclick=\'SelectLED(" + (x + z * 8) + ")\'><circle stroke-width=\'3\' stroke=\'#3e4f51\' fill=\'" + framecontent[selectedFrame - 1][x][selectedPlanNumber - 1][z] + "\' cx=\'50%\' cy=\'50%\' r=\'20\'></circle></svg>";
                }
            }
            break;
        default:
            for (let y = 0; y < 8; y++) {
                for (let x = 0; x < 8; x++) {
                    matrix.innerHTML += "<svg onclick=\'SelectLED(" + (x + y * 8) + ")\'><circle stroke-width=\'3\' stroke=\'#3e4f51\' fill=\'" + framecontent[selectedFrame - 1][x][y][selectedPlanNumber - 1] + "\' cx=\'50%\' cy=\'50%\' r=\'20\'></circle></svg>";
                }
            }
            break;
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
            cube_set_color(selectedPlanNumber - 1, selected2D[i][0], selected2D[i][1], color.replace('#', '0x'), false)
        } else if (selectedPlanDirection === "Y") {
            framecontent[selectedFrame - 1][selected2D[i][0]][selectedPlanNumber - 1][selected2D[i][1]] = color
            cube_set_color(selected2D[i][0], selectedPlanNumber - 1, selected2D[i][1], color.replace('#', '0x'), false)
        } else {
            framecontent[selectedFrame - 1][selected2D[i][0]][selected2D[i][1]][selectedPlanNumber - 1] = color
            cube_set_color(selected2D[i][0], selected2D[i][1], selectedPlanNumber - 1, color.replace('#', '0x'), false)
        }
    }
    renderer.render(scene, camera);
}

function changeColor(element,color){
    let colorPicker = document.getElementById("pickColor")
    let actualColor = colorPicker.value.replace('#', '').match(/.{1,2}/g)
    console.log(actualColor)
    if(element.className === "colorButton grey"){
        element.className = "colorButton " + color
        switch(color){
            case 'red':
                actualColor[0] = "FF"
                break
            case 'green':
                actualColor[1] = "FF"
                break
            case 'blue':
                actualColor[2] = "FF"
                break
        }
    }else{
        element.className = "colorButton grey"
        switch(color){
            case 'red':
                actualColor[0] = "00"
                break
            case 'green':
                actualColor[1] = "00"
                break
            case 'blue':
                actualColor[2] = "00"
                break
        }
    }
    console.log(actualColor)
    colorPicker.value = "#" + actualColor[0] + actualColor[1] + actualColor[2]
}

function refresh2D() {
    for (let i = 0; i < framecontent[selectedFrame - 1].length; i++) {
        for (let j = 0; j < framecontent[selectedFrame - 1][0].length; j++) {
            switch (selectedPlanDirection) {
                case "X":
                    cube_set_color(selectedPlanNumber - 1, i, j, framecontent[selectedFrame - 1][selectedPlanNumber - 1][i][j].replace('#', '0x'), false)
                    break
                case "Y":
                    cube_set_color(i, selectedPlanNumber - 1, j, framecontent[selectedFrame - 1][i][selectedPlanNumber - 1][j].replace('#', '0x'), false)
                    break
                default:
                    cube_set_color(i, j, selectedPlanNumber - 1, framecontent[selectedFrame - 1][i][j][selectedPlanNumber - 1].replace('#', '0x'), false)
            }
        }
    }
    Draw2DMatrix()
    renderer.render(scene, camera);
}

function refresh3D() {
    const indexframe = selectedFrame - 1

    for (let i = 0; i < 8; i++) {
        for (let j = 0; j < 8; j++) {
            for (let k = 0; k < 8; k++) {
                cube_set_color(i, j, k, framecontent[indexframe][i][j][k].replace('#', '0x'), false)
            }
        }
    }
    Draw2DMatrix()
    renderer.render(scene, camera)
}

function ckeckIndexselected2D(findArray) {
    let index = -1
    for (let i = 0; i < selected2D.length; i++) {
        if (selected2D[i][0] === findArray[0] && selected2D[i][1] === findArray[1]) {
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
    const index = ckeckIndexselected2D([x, y])
    if (index >= 0) {
        selected2D.splice(index, 1)
        LEDelement.setAttribute("stroke", "#3e4f51")
        return
    }
    if (shiftpressed) {
        selected2D.push([x, y])
    } else {
        for (var i = 0; i < 64; i++) {
            LEDelements[i].setAttribute("stroke", "#3e4f51")
        }
        selected2D = [[x, y]]
    }
    LEDelement.setAttribute("stroke", "#3aaa96")
    let colorPicker = document.getElementById("pickColor")
    colorPicker.value = LEDelement.getAttribute("fill")
    let actualColor = LEDelement.getAttribute("fill").replace('#', '').match(/.{1,2}/g)
    console.log(actualColor)
    if(actualColor[0].toUpperCase() === "FF"){
        document.getElementById("redButton").className = "colorButton red"
    }else{
        document.getElementById("redButton").className = "colorButton grey"
    }
    if(actualColor[1].toUpperCase() === "FF"){
        document.getElementById("greenButton").className = "colorButton green"
    }else{
        document.getElementById("greenButton").className = "colorButton grey"
    }
    if(actualColor[2].toUpperCase() === "FF"){
        document.getElementById("blueButton").className = "colorButton blue"
    }else{
        document.getElementById("blueButton").className = "colorButton grey"
    }

}

function SelectPlan() {
    selectedPlanDirection = getRadioSelectedValue("axe")
    const range = document.getElementById("planNumber")
    selectedPlanNumber = parseInt(range.value)
    displaySelectedPlan(selectedPlanDirection, selectedPlanNumber)
    Draw2DMatrix()
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


function addframebefore() {

    const contentnum = document.getElementById("numframebefore")
    const contentnumaf = document.getElementById("numframeafter")
    index = parseInt(contentnum.value) - 1

    if (index >= framecontent.length) {
        index = framecontent.length - 1
    }

    if (index < 0) {
        index = 0
    }

    contentnum.value = index + 1

    addframe(index)
    const framerange = document.getElementById("frameRange")
    const frameinput = document.getElementById("frameInput")
    if (index <= (selectedFrame - 1)) {
        selectedFrame++;
        framerange.value = selectedFrame
        frameinput.value = selectedFrame
    }
    framerange.max = framecontent.length
    frameinput.max = framecontent.length
    contentnumaf.max = framecontent.length
    contentnum.max = framecontent.length

}

function addframeafter() {
    const contentnum = document.getElementById("numframebefore")
    const contentnumaf = document.getElementById("numframeafter")
    index = parseInt(contentnumaf.value)

    if (index > framecontent.length) {
        index = framecontent.length - 1
    }

    if (index <= 0) {
        index = 1
    }

    if (index === framecontent.length) {
        console.log("test")
        contentnumaf.value = index + 1
    } else {
        contentnumaf.value = index
    }

    addframe(index)
    const framerange = document.getElementById("frameRange")
    const frameinput = document.getElementById("frameInput")
    if (index <= (selectedFrame - 1)) {
        selectedFrame++;
        framerange.value = selectedFrame
        frameinput.value = selectedFrame
    }
    framerange.max = framecontent.length
    frameinput.max = framecontent.length
    contentnumaf.max = framecontent.length
    contentnum.max = framecontent.length
}

function gotoframe() {
    const framerange = document.getElementById("frameRange")
    const frameinput = document.getElementById("frameInput")
    if (framerange.value < 1) framerange.value = 1
    if (framerange.value > framecontent.length) framerange.value = framecontent.length
    selectedFrame = framerange.value
    frameinput.value = framerange.value
    refresh3D()
}

function removeframe() {
    const framerange = document.getElementById("frameRange")
    const frameinput = document.getElementById("frameInput")
    const frame = framerange.value
    let confirmremove = confirm('Voulez-vous supprimer la frame numéro ' + frame + ' ?')
    if (confirmremove && framecontent.length > 1) {
        framecontent.splice(frame - 1, 1)
        if (frame > framecontent.length) {
            framerange.value = framecontent.length
            frameinput.value = framecontent.length
            selectedFrame = framecontent.length
        }
        const contentnum = document.getElementById("numframebefore")
        const contentnumaf = document.getElementById("numframeafter")
        framerange.max = framecontent.length
        frameinput.max = framecontent.length
        contentnumaf.max = framecontent.length
        contentnum.max = framecontent.length
        document.getElementById("frameNumber").innerHTML = framecontent.length.toString()
        if (contentnum.value > framecontent.length) {
            contentnum.value = framecontent.length
        }
        if (contentnumaf.value > framecontent.length) {
            contentnumaf.value = framecontent.length
        }
        refresh3D()
    }
    if (framecontent.length <= 1) alert('Erreur: vous devez avoir au moins une frame')
}

function nextframe() {
    const framerange = document.getElementById("frameRange")
    framerange.value++
    gotoframe()
}

function previousframe() {
    const framerange = document.getElementById("frameRange")
    framerange.value--
    gotoframe()
}

function addframe(index) {
    framecontent.splice(index, 0, [])
    for (let i = 0; i < RES; i++) {
        framecontent[index].push([])
        for (let j = 0; j < RES; j++) {
            framecontent[index][i].push([])
            for (let k = 0; k < RES; k++) {
                framecontent[index][i][j].push("#000000")
            }
        }
    }
    document.getElementById("frameNumber").innerHTML = framecontent.length.toString()
}

async function init() {
    addframe(0)
    Draw2DMatrix()
    await initGL()
    document.getElementsByName("axe").forEach(item => {
        item.addEventListener("click", SelectPlan)
    })
    selectedPlanDirection = getRadioSelectedValue("axe")
    document.getElementById("planNumber").value = 1
    document.getElementById("paste2D").disabled = true
    document.getElementById("paste3D").disabled = true
    document.getElementById("frameInput").value = framecontent.length
    document.getElementById("frameRange").value = framecontent.length
}


function reset() {
    if(!confirm('Voulez-vous vraiment tout effacer?')) return;
    const contentnum = document.getElementById("numframebefore")
    const contentnumaf = document.getElementById("numframeafter")
    const framerange = document.getElementById("frameRange")
    const frameinput = document.getElementById("frameInput")
    framecontent = []
    addframe(0)
    selectedFrame=1;
    framerange.value = selectedFrame
    frameinput.value = selectedFrame
    contentnumaf.value = selectedFrame
    contentnum.value = selectedFrame
    framerange.max = framecontent.length
    frameinput.max = framecontent.length
    contentnumaf.max = framecontent.length
    contentnum.max = framecontent.length
    refresh3D()
}

function nextplan() {

}

function previousplan() {

}

function nextaxe() {

}

function previousaxe() {


}

document.addEventListener('keydown', (e) => {
    switch (e.key) {
        case "Shift":
            shiftpressed = true;
            break;
    }
    //console.log(e.key)
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