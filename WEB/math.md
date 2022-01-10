# JigMath.js

Par Jérôme Lécuyer
Pour @Jig0ll & @Robotek
https://github.com/Jiogo18/JigMath
Convertit une chaîne de caractère en système d'équations.

## Utilisation

L'entrée est une formule mathématique utilisant les variables x, y, z et t.
La sortie est une couleur en décimal.

La couleur décimale est un nombre entre 0 et 16777215 (#FFFFFF).
Les couleurs r, g, b sont des nombres entre 0 et 255.
Les angles sont en radians.

## Formule

La formule est recalculée pour chaque point (t,z,y,x) avec
 - t : index de la frame `t ∈ [0;tMax[`
 - z : hauteur `z ∈ [0;7]` (z=0 en haut)
 - y : largeur `y ∈ [0;7]` (y=0 à gauche)
 - x : profondeur `x ∈ [0;7]` (x=0 devant)
 - img(x,y,f) : Couleur décimale du pixel en (x,y) de l'image importée d'index f

## Conseils

Ajoutez autant d'espaces que vous souhaitez, le résultat sera le même.
Pour accélerer le traitement priorisez les valeurs constantes : `3*x/2` => `x*(3/2)`.
Si une variable n'est pas définie la valeur retournée est une chaîne de caractère la plus simplifiée.
Ajoutez des fonctions personaliées par le deuxième paramètre : `JigMath('f(x)', [{name: 'f', func: (x) => x+2}])`.
La partie `f(x,y,z,t)=` n'est pas utile et peut être retirée.
Par défaut tMax correspond au nombre d'images, mais vous pouvez le modifier en fonction de votre animation.
La fonction img(x,y,f) autorise les points (x,y) invalides (retourne 0=éteint) mais f doit être compris entre 0 et le nombre d'images importées.
Si vous n'utilisez pas d'images vous n'avez qu'à entrer une formule sans la fonction img()

## Opérateurs disponibles

### Opérations élémentaires : `+`, `-`, `*`, `/`, `%`
### Comparaison : `==`, `!=`, `>`, `>=`, `<`, `<=`, `&&`, `||`
### Manipulation binaire : `<<`, `>>`, `&`, `|`, `^`
### Spéciaux
- Hexadécimal : `#nombre`, `0xnombre`
- Binaire : `0bnombre`

### Fonctions mathématiques

TOUTES les fonctions de `Math` (objet JavaScript) sont disponibles (sans préciser Math).
| Nom                  | Description                                         |
| -------------------- | --------------------------------------------------- |
| max(a, b)            | Maximum entre a et b                                |
| min(a, b)            | Minimum entre a et b                                |
| minmax(min, x, max)  | Un nombre dans l'intervalle [min;max], x ou limite  |
| range(min, x, max)   | Vrai si x est dans l'intervalle [min;max]           |
| abs(x)               | Valeur absolue                                      |
| sqrt(x)              | Racine carrée                                       |
| pow(x, n)            | Puissance                                           |
| exp(x)               | Exponentielle                                       |
| round(a)             | Arrondi à l'entier le plus proche                   |
| floor(a)             | Arrondi à l'entier inférieur                        |
| ceil(a)              | Arrondi à l'entier supérieur                        |
| random()             | Nombre aléatoire entre 0 et 1                       |
| pi()                 | Valeur de PI à 6 décimales                          |
| cos(a)               | Cosinus                                             |
| sin(a)               | Sinus                                               |
| tan(a)               | Tangente                                            |
| acos(a)              | Arc Cosinus                                         |
| asin(a)              | Arc Sinus                                           |
| atan(a)              | Arc Tangente                                        |
| modulo(a,b)          | Modulo                                              |
| angle_complexe(x, y) | Angle de rotation d'un nombre complexe / d'un point |

### Fonctions de construction
| Nom                         | Description                                                        |
| --------------------------- | ------------------------------------------------------------------ |
| triangle(x, x0, y0, pente)) | Fonction triangle avec un sommet en (x0,y0) et un facteur de pente |
| distance(a, b))             | sqrt(a²+b²)                                                        |
| heaviside(t)                | Fonction Heaviside : 0 sur ]-inf;0[ et 1 sur ]0;+inf[              |
| porte(t, t1, t2))           | Fonction porte : identique à range(min,x,max)                      |
| pente_cosale(t)             | Fonction f(t)=t avec f(t<=0)=0                                     |

### Fonctions de couleur
| Nom                  | Description                                                           |
| -------------------- | --------------------------------------------------------------------- |
| rgb(r, g, b))        | Associe les couleurs pour former une couleur décimale                 |
| red(c)               | Quantité de rouge (entre 0 et 255) dans la couleur décimale           |
| green(c)             | Quantité de vert (entre 0 et 255) dans la couleur décimale            |
| blue(c)              | Quantité de bleu (entre 0 et 255) dans la couleur décimale            |
| huerotate(c, angle)) | Rotation de la couleur décimale avec un angle de rotation (en radian) |
| lumiere(c, lumiere)) | Change le taux de luminosité de la couleur (1 = ne change pas)        |


## Exemples

(Si tMax n'est pas précisé, utiliser le nombre d'images importées)

### Couleur unie (cyan)
`#00FFFF`, `tMax = 1`

### Nuances de gris
`rgb(t*255/10,t*255/10,t*255/10)`, `tMax = 10`
`lumiere(#FFFFFF, t/10)`, `tMax = 10`

### Afficher une image ou une série d'images
- Sur l'axe x (en profondeur) : `img(y,z,t)`
- Sur le plan x=0 (devant) : `(x==0) && img(y,z,t)`
- Sur le plan z=7 (au sol) : `(z==7) && img(y,7-x,t)`
- Sur les 2 premiers plans x=0 et x=1 (devant) : `(x<2) && img(y,z,t)`

### Wave (l'image 0 dessine une vague)
`img(y,z+min(8-abs((t-20)-(x-4)),0),0)`, `tMax=40`
`img(y,z+min(triangle(t-x,16,8,1),0),0)`, `tMax=40`

### Image Aléatoire (avec 12 images)
`img(y,z,modulo(t+floor(random()*x*10),12))`

### 2 plans type minecraft (symétrie)
`(range(3, x, 4) && img(y,z,t)) || (range(3, y, 4) && img(x,z,t))`

### 2 plans type minecraft (déphasé), si tMax=12
`(range(3, x, 4) && img(y,z,t)) || (range(3, y, 4) && img(x,z,modulo(t+6,12)))`

### Avance puis recul
`((7-x)==minmax(0,round(triangle(t,50,20,0.5)),7)) && img(y,z,minmax(0,triangle(t,50,40,1),20))`, `tMax=100`

### Robotek rainbow
`(x<2) && huerotate(img(y,z,0), t * 2 * pi() / 60)`, `tMax=60`

### Robotek Tourne (ne rend pas super bien et tourne en continu)
`(x<2) && img(round(3.5+(y-3.5)*cos(t*6.283/30) + (z-3.5)*sin(t*6.283/30)), round(3.5+(z-3.5)*cos(t*6.283/30) - (y-3.5)*sin(t*6.283/30)), 0)`, `tMax=30`

### Radar cyan
`(x<2) && lumiere(#00FFFF, 1-exp(modulo(t*2*pi()/20-angle_complexe(y-3.5, z-3.5), 2*pi()) - pi()/2))`, `tMax=20`

### Cercle_chromatique (face entière)
`(x<2) && huerotate(#FF0000, angle_complexe(y-3.5, z-3.5)-t*2*pi()/20)`, `tMax=20`

### Cercle_chromatique2 (cercle)
`(x<2) && (distance(y-3.5,z-3.5) <= 4) && huerotate(#FF0000, angle_complexe(y-3.5, z-3.5)-t*2*pi()/20)`, `tMax=20`

### Polytech3 (camembert s'ouvre puis se ferme)
`(x<2) &&    range(-2*pi(),    modulo(angle_complexe(y-3.5, z-3.5)+pi(),2*pi())  - (   pente_cosale(t) - pente_cosale(t-20) + pente_cosale(t-40) - pente_cosale(t-60)   )*2*pi()/20   ,  0)    && img(y,z,0)`, `tMax=80`

### fire_decompo_rgb (8 plans : 2 rouges, 2 verts, 2 bleus, 2 rgb)
`rgb(((x<2)||(6<=x))&&red(img(y,z,t)), (range(2, x, 3)||(6<=x))&&green(img(y,z,t)), (range(4, x, 5)||(6<=x))&&blue(img(y,z,t)))`
