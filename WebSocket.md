# Connexion

Connexion par WebSocket afin de communiquer via une page web

| Noms de variable  | Obtenu par                          | Description                                                     |
| ----------------- | ----------------------------------- | --------------------------------------------------------------- |
| `mdpHash`         | `hash(mdp)`                         | Mot de passe hashé pour plus de sécurité                        |
| `clientChallenge` | `random()`                          | Challenge aléatoire du client pour stocker un cookie-key unique |
| `serverChallenge` | `random()`                          | Challenge aléatoire du serveur pour transférer la clé           |
| `cookieKey`       | `hash(mdp + clientChallenge)`       | Mdp stocké par le navigateur                                    |
| `key`             | `hash(cookieKey + serveurChallenge` | Clé transférée au serveur                                       |
| `sessionId`       | `~random par le serveur`            | Enregistré par le client et le serveur                          |
| `adresse`         |                                     | Adresse du client                                               |
Note : `serverChallenge` n'est pas enregistré car il est unique pour chaque connexion. De plus, un sel aléatoire est utilisé pour une `key`.
`clientChallenge` est un sel (salt) aléatoire pour stocker un cookie-key unique. Le sel est communiqué au serveur pour obtenir le même cookieKey.

## Première connexion :
- Le client créé un `clientChallenge` (salt) et un `cookieKey` (`hash(mdpHash, clientChallenge)`) puis les enregistre dans le navigateur
- Le serveur envoie un `serverChallenge`
- Le client envoie `key` (`hash(cookieKey + serveurChallenge)`, salt aléatoire) et `clientChallenge`
- Le serveur vérifie la clé
- Le serveur enregistre le client (adresse) et envoi un `sessionId`
- Le client enregistre `sessionId` dans le navigateur

## Deuxième connexion :
- Le serveur envoie un `serverChallenge`
- Le client envoie un nouveau `key` (`hash(cookieKey + serveurChallenge)`), `clientChallenge` et `sessionId`
- Le serveur vérifie l'id de session, l'adresse et le hash


# Contenu envoyé par le serveur (Json)

## Server Challenge

```json
{
	"type": "auth",
	"content": "challenge",
	"challenge": "..."
}
```

## Connecté / Deconnecté
Événements de connexion (une fois authentifié)
```json
{
	"type": "management",
	"msg": "connected", // or disconnected
}
```

# Contenu envoyé par le client (Json)

## À la connexion
À envoyer dès que le WebSocket est considéré comme connecté.
```json
{
	"type": "auth",
	"date": "CURRENT DATE TIME IN MS",
	"key": "..."
}
```

## Limiter le nombre de frames à 1
```json
{
	"type": "ledcube",
	"date": "CURRENT DATE TIME IN MS",
	"action": "set-animation-frames-count",
	"count": 1, // nombre de frames (optionnel, 1 par défaut)
	"fps": 20, // nombre de frames par seconde (optionnel, 100 par défaut)
}
```
count : nombre de frames dans l'animation

## Envoyer une image
```json
{
	"type": "ledcube",
	"date": "CURRENT DATE TIME IN MS",
	"action": "set-layer",
	"layer": 0, // couche par rapport à la direction (0 par défaut)
	"layer-dir": "X", // ou Y, ou Z (Z par défaut)
	"colors": [
		// tableau de 8*8 rempli de nombres en int32 (en rgb, 8 bits par couleur)
		[ 0, 0, 0, 0, 0, 0, 0, 0 ],
		[ 0, 0, 0, 0, 0, 0, 0, 0 ],
		[ 0, 0, 0, 0, 0, 0, 0, 0 ],
		[ 0, 0, 0, 0, 0, 0, 0, 0 ],
		[ 0, 0, 0, 0, 0, 0, 0, 0 ],
		[ 0, 0, 0, 0, 0, 0, 0, 0 ],
		[ 0, 0, 0, 0, 0, 0, 0, 0 ],
		[ 0, 0, 0, 0, 0, 0, 0, 0 ],
	],
}
```

## Lancer l'animation d'un fichier
```json
{
	"type": "ledcube",
	"date": "CURRENT DATE TIME IN MS",
	"action": "animation.play",
	"fileName": "...", // nom de l'animation
}
```

## Changer une led

```json
{
	"type": "ledcube",
	"date": "CURRENT DATE TIME IN MS",
	"action": "setLed",
	"x": 0, // position en X (0 par défaut)
	"y": 0, // position en Y (0 par défaut)
	"z": 0, // position en Z (0 par défaut)
	"color": 0, // int32 pour une couleur en rgb (8 bits par couleur)
}
```

# Couleur

Un int32 pour une couleur en rgb (8 bits par couleur)

## Exemples
Rouge : 0xFF0000 = 16711680 = (255, 0, 0)
Vert : 0x00FF00 = 65280 = (0, 255, 0)
Bleu : 0x0000FF = 255 = (0, 0, 255)