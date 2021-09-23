
#include "PatternThread.h"

void sleepms(int duration)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

int getLocalTimeMS()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

double modulo(double a, double b)
{
	return a - ((int)(a / b)) * b;
}

PatternThread::PatternThread()
{
	ask_to_stop = false;
	colors = new uint16_t[COLORS];
	for (int i = 0; i < COLORS; i++)
		colors[i] = 0;
	is_updated = false;
	default_pattern = "";
}

PatternThread::~PatternThread()
{
	std::cout << "[PatternThread] Closing a thread..." << std::endl;
	ask_to_stop = true;

	if (thread)
		delete thread;
	if (colors)
		delete[] colors;
	std::cout << "[PatternThread] Closed" << std::endl;
}

void PatternThread::start()
{
	ask_to_stop = false;
	if (thread != nullptr && is_running)
		delete thread;
	thread = new std::thread([this]()
							 { this->process(); });
}

void PatternThread::join()
{
	if (thread)
		thread->join();
}

void PatternThread::terminate()
{
	if (thread)
		delete thread;
	thread = nullptr;
	is_running = false;
}

void PatternThread::forcequit()
{
	close();
	sleepms(50);
	if (is_running)
		terminate();
	join();
}

void PatternThread::setDefaultPattern(std::string default_pattern)
{
	this->default_pattern = default_pattern;
}

void PatternThread::process()
{
	is_running = true;

	std::string choix;

	do
	{

		std::cout << std::endl
				  << "Choix du pattern :" << std::endl
				  << " 0  - Auto-sapin (plusieurs patterns)" << std::endl
				  << " 1  - i++ (alterne)" << std::endl
				  << " 2  - test set_led_color (R,G,B a la suite)" << std::endl
				  << " 3  - Guirlande" << std::endl
				  << " 4  - Carre plein" << std::endl
				  << " 5  - Carre bord" << std::endl
				  << " 6  - Set LED Value" << std::endl
				  << " 7  - Ligne Balai" << std::endl
				  << " 8  - Ligne Balai (Alterne)" << std::endl
				  << " 9  - Serpentin" << std::endl
				  << " 10 - Radar" << std::endl
				  << ">> ";

		if (default_pattern != "")
		{
			choix = default_pattern;
			default_pattern = "";
		}
		else
			std::cin >> choix;
		std::cout << "Lancement du Pattern #" << choix << std::endl;

		loop = 0;

		while (!ask_to_stop)
		{
			if (choix == "0" || choix == "auto_sapin")
				pattern_auto_sapin();
			else if (choix == "1" || choix == "iplusplus")
				pattern_iplusplus();
			else if (choix == "2" || choix == "test_set_led_color")
				pattern_test_set_led_color();
			else if (choix == "3" || choix == "guirlande")
				pattern_guirlande();
			else if (choix == "4" || choix == "carre_plein")
				pattern_carre_plein();
			else if (choix == "5" || choix == "carre_bord")
				pattern_carre_bord();
			else if (choix == "6" || choix == "set_led_value")
				pattern_set_led_value();
			else if (choix == "7" || choix == "ligne_balai")
				pattern_ligne_balai();
			else if (choix == "8" || choix == "ligne_balai_alterne")
				pattern_ligne_balai_alterne();
			else if (choix == "9" || choix == "serpentin")
				pattern_serpentin();
			else if (choix == "10" || choix == "radar")
				pattern_radar();
			else
			{
				std::cout << "ERROR: Choice Unknown \"" << choix << "\"" << std::endl;
				break;
			}
			loop++;
		}
	} while (!ask_to_stop);

	is_running = false;
}

#define LED_WIDTH 8			   // One line with 1 color
#define LED_GROUP_ONE_COLOR 16 // 2 lines with 1 color
#define LED_GROUP_TWO_COLOR 32 // 2 lines with 2 colors
#define LED_GROUP_RGB_COLOR 48 // 2 lines with 3 colors

void PatternThread::set_color(uint16_t x, uint16_t y, uint16_t r, uint16_t g, uint16_t b)
{
	bool x_left = (x % LED_WIDTH) < LED_WIDTH / 2; // Partie gauche ou partie droite
	bool y_bottom = (y % 2) == 0;				   // Partie bas ou partie haute
	int y_group = y / 2;

	int i = y_group * LED_GROUP_RGB_COLOR + x;

	if (x_left)
	{ // Partie gauche
		if (!y_bottom)
		{
			i += LED_WIDTH;
		}
	}
	else
	{ // Partie droite
		if (y_bottom)
		{
			i += LED_WIDTH;
		}
	}

	// Exceptions (pb de cablages)
	bool overrideRed = false;
	bool overrideGreen = false;
	bool overrideBlue = false;
	/**
	 * Exceptions :
	 * En (0,4) le rouge allume le (4,5) => le rouge (4,5) est controlé par le rouge (0,4)
	 * En (1,4) le rouge allume le (5,5)
	 * En (4,5) le rouge allume le (partout)
	 * En (5,5) le rouge allume le (partout)
	 * 
	 **/
	// if (y == 4)
	// {
	// 	if (x == 0 || x == 1)
	// 	{
	// 		colors[i + 4] = r;
	// 		colors[i + LED_GROUP_ONE_COLOR] = g;
	// 		colors[i + LED_GROUP_TWO_COLOR] = b;
	// 		overrided = true;
	// 	}
	// }
	// else if (y == 4)
	// {
	// 	if (x == 4 || x == 5)
	// 	{
	// 		colors[i - 4] = r;
	// 		colors[i + LED_GROUP_ONE_COLOR] = g;
	// 		colors[i + LED_GROUP_TWO_COLOR] = b;
	// 		overrided = true;
	// 	}
	// }
	if (x == 0 && y == 2)
	{
		// rouge affiche rien...
		// overrideRed = true;
	}
	else if (x == 6 && y == 2)
	{
		// Vert affiche rien... (faux contact dans les fils)
		// overrideGreen = true;
	}
	else if (x == 0 && y == 4)
	{
		// Rouge affiche le rouge en (4,5)
		overrideRed = true;
	}
	else if (x == 1 && y == 4)
	{
		// Rouge affiche le rouge en (5,5)
		overrideRed = true;
	}
	else if (x == 2 && y == 4)
	{
		// Rouge affiche rien... (maix un rouge clignote comme le 6,2 vert)
		colors[102] = r; // Valeur pour (6,5)
		overrideRed = true;
	}
	else if (x == 3 && y == 4)
	{
		// Rouge affiche rien...
		colors[103] = r; // Valeur pour (7,5)
		overrideRed = true;
	}
	else if (x == 6 && y == 4)
	{
		// Vert affiche rien... mais un vert clignote en (5,4)
	}
	else if (x == 7 && y == 4)
	{
		// Vert affiche rien... mais un vert clignote en (4,4)
		overrideGreen = true;
	}
	else if (x == 4 && y == 5)
	{
		// Rouge affiche rien...
		colors[96] = r; // Valeur pour (0,4)
		overrideRed = true;
	}
	else if (x == 5 && y == 5)
	{
		// Rouge affiche rien...
		colors[97] = r; // Valeur pour (1,4)
		overrideRed = true;
	}
	else if (x == 6 && y == 5)
	{
		// Rouge affiche le rouge en (2,4)
		overrideRed = true;
	}
	else if (x == 7 && y == 5)
	{
		// Rouge affiche le rouge en (3,4)
		overrideRed = true;
	}
	else if (x == 3 && y == 6)
	{
		// Rouge affiche rien
		// overrideRed = true;
	}

	if (!overrideRed)
		colors[i] = r;
	if (!overrideGreen)
		colors[i + LED_GROUP_ONE_COLOR] = g;
	if (!overrideBlue)
		colors[i + LED_GROUP_TWO_COLOR] = b;
}

void PatternThread::fill_color_off()
{
	is_updated = true;
	for (int i = 0; i < COLORS; i++)
		colors[i] = 0;
	is_updated = false;
}

void PatternThread::pattern_auto_sapin()
{
	for (int pattern = 0; pattern < 4 && !ask_to_stop; pattern++)
	{
		int start = getLocalTimeMS();
		fill_color_off();
		loop = 0;
		do
		{
			if (pattern == 0)
				pattern_guirlande();
			else if (pattern == 1)
				pattern_ligne_balai_alterne();
			else if (pattern == 2)
				pattern_serpentin();
			else if (pattern == 3)
				pattern_radar();

			loop++;

		} while (getLocalTimeMS() - start < 10000 && !ask_to_stop);
	}
}

void PatternThread::pattern_iplusplus()
{
	int biton = loop % COLORS;

	std::cout << "i = " << biton << std::endl;

	is_updated = true;
	fill_color_off();
	colors[biton] = HIGH;
	is_updated = false;

	getchar();
}

void PatternThread::pattern_test_set_led_color()
{
	is_updated = true;
	// Test de set_color
	std::cout << "Test set_color" << std::endl;
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			std::cout << "Test set_color (" << x << "," << y << ")" << std::endl;
			set_color(x, y, HIGH, 0, 0);
			is_updated = false;
			getchar();
			set_color(x, y, 0, HIGH, 0);
			is_updated = false;
			getchar();
			set_color(x, y, 0, 0, HIGH);
			is_updated = false;
			getchar();
			set_color(x, y, 0, 0, 0);

			if (ask_to_stop)
			{
				is_updated = false;
				return;
			}
		}
	}
	is_updated = false;
	sleepms(50);
}

void PatternThread::pattern_guirlande()
{
	is_updated = true;
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{

			if (rand() < RAND_MAX * 0.1)
			{ // 10 % de chance d'être allumé
				set_color(x, y,
						  rand() < RAND_MAX / 2 ? HIGH : 0,
						  rand() < RAND_MAX / 2 ? HIGH : 0,
						  rand() < RAND_MAX / 2 ? HIGH : 0);
			}
			else
			{
				set_color(x, y, 0, 0, 0);
			}
		}
	}
	is_updated = false;
	sleepms(200);
}

void PatternThread::pattern_carre_plein()
{
	float distance;
	float x0 = 3.5, y0 = 3.5;

	int phase = loop % 5;

	is_updated = true;
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{

			// Circle
			//distance = std::sqrt((x0 - x)*(x0 - x) + (y0 - y)*(y0 - y));
			// Manhattan distance
			distance = std::max(std::abs(x0 - x), std::abs(y0 - y));

			if (distance < phase)
			{
				set_color(x, y, HIGH, HIGH, HIGH);
			}
			else
			{
				set_color(x, y, 0, 0, 0);
			}
		}
	}
	is_updated = false;
	sleepms(200);
}

void PatternThread::pattern_carre_bord()
{
	float distance;
	float x0 = 3.5, y0 = 3.5;

	int phase = loop % 5;
	int phase_color = (loop / 5) % 8; // Même couleur sur une phase
	uint16_t r = phase_color & 0b100 ? HIGH : 0;
	uint16_t g = phase_color & 0b010 ? HIGH : 0;
	uint16_t b = phase_color & 0b001 ? HIGH : 0;

	is_updated = true;
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			// Circle
			//distance = std::sqrt((x0 - x)*(x0 - x) + (y0 - y)*(y0 - y));
			// Manhattan distance
			distance = std::max(std::abs(x0 - x), std::abs(y0 - y));

			if (distance < phase && phase < distance + 1)
			{
				set_color(x, y, r, g, b);
			}
			else
			{
				set_color(x, y, 0, 0, 0);
			}
		}
	}
	is_updated = false;
	sleepms(200);
}

void PatternThread::pattern_set_led_value()
{
	int x, y;
	int r, g, b;
	std::cout << "Position de la Led (0-7) :" << std::endl;
	std::cout << "x = ";
	std::cin >> x;
	std::cout << "y = ";
	std::cin >> y;

	if (x < 0 || y < 0 || WIDTH <= x || HEIGHT <= y)
	{
		std::cout << "ERROR: Position en dehors du cube" << std::endl;
		return;
	}

	std::cout << "Couleur de la Led (0-HIGH ou -1) :" << std::endl;
	std::cout << "R = ";
	std::cin >> r;
	if (r == -1)
		r = HIGH;
	std::cout << "G = ";
	std::cin >> g;
	if (g == -1)
		g = HIGH;
	std::cout << "B = ";
	std::cin >> b;
	if (b == -1)
		b = HIGH;

	is_updated = true;
	set_color(x, y, r, g, b);
	is_updated = false;
}

void PatternThread::pattern_ligne_balai()
{
	int phase = loop % 16;
	int phase_x = phase < 8 ? phase : 15 - phase;
	is_updated = true;
	fill_color_off();
	set_color(phase_x, 0, HIGH, HIGH, HIGH);
	set_color(phase_x, 1, HIGH, LOW, LOW);
	set_color(phase_x, 2, LOW, HIGH, LOW);
	set_color(phase_x, 3, LOW, LOW, HIGH);
	set_color(phase_x, 4, HIGH, HIGH, LOW);
	set_color(phase_x, 5, HIGH, LOW, HIGH);
	set_color(phase_x, 6, LOW, HIGH, HIGH);
	set_color(phase_x, 7, HIGH, HIGH, HIGH);
	is_updated = false;
	sleepms(100);
}

void PatternThread::pattern_ligne_balai_alterne()
{
	int phase = loop % 14;
	int phase_x = phase < 7 ? phase : 14 - phase;
	int phase_x2 = 7 - phase_x;
	is_updated = true;
	fill_color_off();
	set_color(phase_x, 0, HIGH, HIGH, HIGH);
	set_color(phase_x2, 1, HIGH, LOW, LOW);
	set_color(phase_x, 2, LOW, HIGH, LOW);
	set_color(phase_x2, 3, LOW, LOW, HIGH);
	set_color(phase_x, 4, HIGH, HIGH, LOW);
	set_color(phase_x2, 5, HIGH, LOW, HIGH);
	set_color(phase_x, 6, LOW, HIGH, HIGH);
	set_color(phase_x2, 7, HIGH, HIGH, HIGH);
	// for (int y = 0; y < HEIGHT; y++)
	// {
	// 	if (y % 2 == 0)
	// 		set_color(phase_x, y, HIGH, HIGH, HIGH);
	// 	else
	// 		set_color(7 - phase_x, y, HIGH, HIGH, HIGH);
	// }
	is_updated = false;
	sleepms(100);
}

void PatternThread::pattern_serpentin()
{
	int phase_led = loop % LEDS;

	int carre = sqrt(phase_led) / 2;

	int led_par_bord = (carre + 1) * 2;

	int first_led_du_carre = (led_par_bord - 2) * (led_par_bord - 2);
	int phase_led_du_carre = phase_led - first_led_du_carre;

	int phase_bord_du_carre = phase_led_du_carre / (led_par_bord - 1);
	int phase_led_du_bord = phase_led_du_carre % (led_par_bord - 1);

	int x, y;
	if (phase_bord_du_carre == 0)
	{
		x = 3 - carre;
		y = 3 + carre - phase_led_du_bord;
	}
	else if (phase_bord_du_carre == 1)
	{
		x = 4 - carre + phase_led_du_bord;
		y = 3 - carre;
	}
	else if (phase_bord_du_carre == 2)
	{
		x = 4 + carre;
		y = 4 - carre + phase_led_du_bord;
	}
	else if (phase_bord_du_carre == 3)
	{
		x = 3 + carre - phase_led_du_bord;
		y = 4 + carre;
	}

	int phase_color = (loop / LEDS) % 2;
	if (phase_color == 0)
	{
		set_color(x, y, 0, HIGH, HIGH);
	}
	else if (phase_color == 1)
	{
		set_color(x, y, 0, 0, 0);
	}

	is_updated = false;
	sleepms(74); // 10000 ms / 128 - 4 ms
}

void PatternThread::pattern_radar()
{
	float phase_theta = (loop % 100) / 100.0 * 2 * PI;
	float theta, theta_diff, facteur;
	float distance;
	float x0 = 3.5, y0 = 3.5;

	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			// Circle
			// distance = std::sqrt((x0 - x) * (x0 - x) + (y0 - y) * (y0 - y));
			theta = atan((y - y0) / (x - x0));
			if ((x - x0) < 0)
				theta += PI;
			theta = modulo(theta, 2 * PI);

			theta_diff = abs(modulo(abs(theta - phase_theta), 2 * PI) - PI);

			facteur = 1 - theta_diff / PI;
			facteur *= facteur * facteur * facteur;
			if (facteur < 0)
			{
				facteur = 0;
			}
			else if (facteur > 1.0)
			{
				facteur = 1;
			}

			set_color(x, y, 0, 4095 * facteur, 4095 * facteur);
		}
	}

	is_updated = false;
	sleepms(50);
}