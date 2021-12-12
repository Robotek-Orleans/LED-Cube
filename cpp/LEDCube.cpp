#include "LEDCube.h"

#include <iostream>

std::ostream &operator<<(std::ostream &os, uint8_t n)
{
    if (n < 10)
        os << " ";
    os << std::to_string(n);
    if (n < 100)
        os << " ";
    return os;
}

uint8_t flipBit(uint8_t n)
{
    n = (n & 0xF0) >> 4 | (n & 0x0F) << 4;
    n = (n & 0xCC) >> 2 | (n & 0x33) << 2;
    n = (n & 0xAA) >> 1 | (n & 0x55) << 1;
    return n;
}

LEDCube::LEDCube(std::string fileName)
{
    tlc = new TLCSin(DATALENGTH);
    // Set the xlat pin to be an output
    bcm2835_gpio_fsel(LAYER1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LAYER2, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LAYER3, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LAYER4, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LAYER5, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LAYER6, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LAYER7, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LAYER8, BCM2835_GPIO_FSEL_OUTP);

    for (uint8_t i = 0; i < 8; i++)
        setLayer(i, false);

    std::string line;

    std::ifstream flux(fileName, std::ios::in);
    if (!flux)
    {
        std::cout << "Erreur lors de la lecture du fichier" << std::endl;
        exit(1);
    }
    std::getline(flux, line);
    numberFrames = std::stoi(line);
    std::getline(flux, line);
    frameTime = std::stoi(line) * MS_CLOCK;

    tlc_pattern = new uint8_t **[numberFrames];
    for (int t = 0; t < numberFrames; ++t)
    {
        tlc_pattern[t] = new uint8_t *[ZLENGTH];
        for (int z = 0; z < ZLENGTH; ++z)
        {
            tlc_pattern[t][z] = new uint8_t[DATALENGTH];
            std::memset(tlc_pattern[t][z], 0, DATALENGTH);
        }
    }

    uint8_t ****transientData;
    transientData = new uint8_t ***[ZLENGTH];

    for (int z = 0; z < ZLENGTH; z++)
    {
        transientData[z] = new uint8_t **[YLENGTH];
        for (int y = 0; y < YLENGTH; y++)
        {
            transientData[z][y] = new uint8_t *[XLENGTH];
            for (int x = 0; x < XLENGTH; x++)
            {
                transientData[z][y][x] = new uint8_t[3];
                std::memset(transientData[z][y][x], 0, 3);
            }
        }
    }

    for (int t = 0; t < numberFrames; t++)
    {
        for (int z = 0; z < ZLENGTH; z++)
        {
            for (int y = 0; y < YLENGTH; y++)
            {
                for (int x = 0; x < XLENGTH; x++)
                {
                    for (int c = 0; c < 3; c++)
                    {
                        std::getline(flux, line);
                        transientData[z][y][x][c] = std::stoi(line);
                    }
                }
            }
        }
        setData(t, transientData);

        #ifdef DEBUG
        std::cout << "Premiere couche matrice d'entree (r-g-b)" << std::endl;
        for (int y = 0; y < 8; ++y)
        {
            std::cout << "Ligne " << y << " : ";
            for (int x = 0; x < 8; ++x)
            {
                std::cout << transientData[0][y][x][0] << "-" << transientData[0][y][x][1] << "-" << transientData[0][y][x][2] << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "PremiÃ¨re couche TLC Compatible (r | g | b)" << std::endl;
        const uint8_t *layer = tlc_pattern[t][0];
        int octet_in_2_lines_one_color = 24;
        for (int y_2 = 0; y_2 < 4; ++y_2)
        {
            std::cout << "Lignes " << 2 * y_2 << "-" << 2 * y_2 + 1 << " : ";
            for (int c = 0; c < 3; ++c)
            {
                int first = octet_in_2_lines_one_color * (c + 3 * y_2);
                for (int i = 0; i < octet_in_2_lines_one_color; i += 3)
                {
                    std::cout << layer[i + first] << "." << layer[i + first + 1] << "." << layer[i + first + 2] << " ";
                }
                std::cout << " |  ";
            }
            std::cout << std::endl;
        }
        #endif
    }

    flux.close();

    for (int z = 0; z < ZLENGTH; z++)
    {
        for (int y = 0; y < YLENGTH; y++)
        {
            for (int x = 0; x < XLENGTH; x++)
            {
                delete[] transientData[z][y][x];
            }
            delete[] transientData[z][y];
        }
        delete[] transientData[z];
    }
    delete[] transientData;
}

void LEDCube::start()
{
    int f = 0;

    while (1)
    {
        clock_t newFrameTime = clock() + frameTime;
        while(clock() < newFrameTime){
            for (int z = 0; z < 8; z++)
            {
                setLayer(z == 0 ? 7 : z - 1, false);
                setLayer(z, true);
                #ifdef DEBUG
                std::cout << "layer set " << i << std::endl;
                #endif
                tlc->send(tlc_pattern[f][(z + 1) % 8]);
            }
        }
        f = (f+1)%numberFrames;
    }
}

void LEDCube::setLayer(uint8_t nLayer, bool bState)
{
    switch (nLayer)
    {
    case 0:
        bcm2835_gpio_write(LAYER1, !bState);
        return;
    case 1:
        bcm2835_gpio_write(LAYER2, !bState);
        return;
    case 2:
        bcm2835_gpio_write(LAYER3, !bState);
        return;
    case 3:
        bcm2835_gpio_write(LAYER4, !bState);
        return;
    case 4:
        bcm2835_gpio_write(LAYER5, !bState);
        return;
    case 5:
        bcm2835_gpio_write(LAYER6, !bState);
        return;
    case 6:
        bcm2835_gpio_write(LAYER7, !bState);
        return;
    case 7:
        bcm2835_gpio_write(LAYER8, !bState);
        return;
    default:
        return;
    }
}

#ifdef DEBUG
void LEDCube::sendLayer(uint8_t nLayer, uint8_t *data)
{
    for (uint8_t i = 0; i < 8; i++) setLayer(i, false);
    setLayer(nLayer, true);
    tlc->send(data);
}
#endif

void LEDCube::stop()
{
    for (uint8_t i = 0; i < 8; i++) setLayer(i, false);
}

void LEDCube::setData(int t, uint8_t ****matrice)
{
    uint8_t *tlc_layer;
    uint8_t **line;
    int z, y, x;
    bool y_bottom;
    int y_group;
    bool x_left;
    int i;
    uint8_t r1, g1, b1, r2, g2, b2;
    int iSplitR, iSplitG, iSplitB;
    uint8_t *tlc_octet;

    for (z = 0; z < ZLENGTH; ++z)
    {
        // Inversion de couleurs
        uint8_t a = matrice[z][3][6][1];
        matrice[z][3][6][1] = matrice[z][3][6][2];
        matrice[z][3][6][2] = a;

        tlc_layer = tlc_pattern[t][z];
        for (y = 0; y < YLENGTH; ++y)
        {
            y_bottom = (y % 2) == 0; // Partie bas ou partie haute
            y_group = y / 2;
            line = matrice[z][y];
            for (x = 0; x < XLENGTH; x += 2)
            {
                // Traitement de 2 led à la fois

                x_left = (x % XLENGTH) < XLENGTH / 2; // Partie gauche ou partie droite

                i = y_group * LED_GROUP_RGB_COLOR + x;

                if (x_left)
                { // Partie gauche
                    if (!y_bottom)
                    {
                        i += XLENGTH;
                    }
                }
                else
                { // Partie droite
                    if (y_bottom)
                    {
                        i += XLENGTH;
                    }
                }

                r1 = (line[x][0]);
                g1 = (line[x][1]);
                b1 = (line[x][2]);
                r2 = (line[x + 1][0]);
                g2 = (line[x + 1][1]);
                b2 = (line[x + 1][2]);

                // Split 2 LED : 6 uint16 en 9 uint8 (3 uint8 pour 3 couleurs, ou encore 6*1.5 uint8)

                iSplitR = DATALENGTH - (i)*INT12_TO_INT8 - 1;

                tlc_octet = &tlc_layer[iSplitR];
                *(tlc_octet) = (r1 & 0b00001111) << 4;
                *(--tlc_octet) = (r1 & 0b11110000) >> 4;
                *(--tlc_octet) = r2;
                *(tlc_octet -= TLC_NEXT_COLOR - 2) = (g1 & 0b00001111) << 4;
                *(--tlc_octet) = (g1 & 0b11110000) >> 4;
                *(--tlc_octet) = g2;
                *(tlc_octet -= TLC_NEXT_COLOR - 2) = (b1 & 0b00001111) << 4;
                *(--tlc_octet) = (b1 & 0b11110000) >> 4;
                *(--tlc_octet) = b2;

                // Exceptions (pb de cablages)
                /*bool overrideRed1 = false;
                bool overrideGreen1 = false;
                bool overrideBlue1 = false;
                bool overrideRed2 = false;
                bool overrideGreen2 = false;
                bool overrideBlue2 = false;
                if (x == 0 && y == 2)
                {
                    // rouge affiche rien...
                    // overrideRed1 = true;
                }
                else if (x == 6 && y == 2)
                {
                    // Vert affiche rien... (faux contact dans les fils)
                    // overrideGreen1 = true;
                }
                else if (x == 2 && y == 4)
                {
                    // Rouge affiche rien... (maix un rouge clignote comme le 6,2 vert)
                    //colors[102] = r; // Valeur pour (6,5)
                    overrideRed1 = true;
                    // Rouge affiche rien...
                    //colors[103] = r; // Valeur pour (7,5)
                    overrideRed2 = true;
                }
                else if (x == 6 && y == 4)
                {
                    // Vert affiche rien... mais un vert clignote en (5,4)
                    overrideGreen1 = true;
                    // Vert affiche rien... mais un vert clignote en (4,4)
                    overrideGreen2 = true;
                }
                else if (x == 4 && y == 5)
                {
                    // Rouge affiche rien...
                    //colors[96] = r; // Valeur pour (0,4)
                    overrideRed1 = true;
                    // Rouge affiche rien...
                    //colors[97] = r; // Valeur pour (1,4)
                    overrideRed2 = true;
                }
                else if (x == 6 && y == 5)
                {
                    // Rouge affiche le rouge en (2,4)
                    overrideRed1 = true;
                    // Rouge affiche le rouge en (3,4)
                    overrideRed2 = true;
                }
                else if (x == 2 && y == 6)
                {
                    // Rouge affiche rien
                    //overrideRed2 = true;
                }

                tlc_octet = &tlc_layer[iSplitR];
                if (overrideRed1)
                    --tlc_octet;
                else
                {
                    *(tlc_octet) = (r1 & 0b00001111) << 4;
                    *(--tlc_octet) = (r1 & 0b11110000) >> 4;
                }
                if (overrideRed2)
                    --tlc_octet;
                else
                    *(--tlc_octet) = r2;
                if (overrideGreen1)
                    tlc_octet -= TLC_NEXT_COLOR - 1;
                else
                {
                    *(tlc_octet -= TLC_NEXT_COLOR - 2) = (g1 & 0b00001111) << 4;
                    *(--tlc_octet) = (g1 & 0b11110000) >> 4;
                }
                if (overrideGreen2)
                    --tlc_octet;
                else
                    *(--tlc_octet) = g2;
                if (overrideBlue1)
                    tlc_octet -= TLC_NEXT_COLOR - 1;
                else
                {
                    *(tlc_octet -= TLC_NEXT_COLOR - 2) = (b1 & 0b00001111) << 4;
                    *(--tlc_octet) = (b1 & 0b11110000) >> 4;
                }
                if (overrideBlue2)
                    --tlc_octet;
                else
                    *(--tlc_octet) = b2;
                    */

                /**
                 * Exemple pour une couleur :
                 * 2 couleur 12 bits : r1=0bAAAAABBBBCCCC et r2=0bDDDDEEEEFFFF
                 * (binaire avec les régions représentées par des symboles)
                 *
                 * octet 1 : 0bAAAABBBB
                 * octet 2 : 0bCCCCDDDD
                 * octet 3 : 0bEEEEFFFFs
                 */
            }
        }
    }
}

LEDCube::~LEDCube()
{
    // Eteind toutes les couches
    stop();
    // Delete des variables
    delete[] tlc;
    for (int t = 0; t < numberFrames; ++t)
    {
        for (int z = 0; z < ZLENGTH; ++z)
        {
            delete[] tlc_pattern[t][z];
        }
        delete[] tlc_pattern[t];
    }
    delete[] tlc_pattern;
}
