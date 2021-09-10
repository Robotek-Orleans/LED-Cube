
#include "PatternThread.h"

void sleepms(int duration)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

int getLocalTimeMS()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
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

void PatternThread::setDefaultPattern(std::string default_pattern)
{
	this->default_pattern = default_pattern;
}

void PatternThread::process()
{
	reverse = false;

	std::string choix;

	do
	{

		std::cout << std::endl
				  << "Choix du pattern :" << std::endl
				  << " 1 - i++ (alterne)" << std::endl
				  << " 2 - test set_led_color (R,G,B a la suite)" << std::endl
				  << " 3 - Guirlande" << std::endl
				  << " 4 - Carre plein" << std::endl
				  << " 5 - Carre bord" << std::endl
				  << " 6 - Set LED Value" << std::endl
				  << " 7 - Ligne Balai" << std::endl
				  << " 8 - Ligne Balai (Alterne)" << std::endl
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
			else
			{
				std::cout << "ERROR: Choice Unknown \"" << choix << "\"" << std::endl;
				break;
			}
			loop++;
		}
	} while (!ask_to_stop);
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
	bool overrided = false;
	/**
	 * Exceptions :
	 * En (0,4) le rouge allume le (4,5) => le rouge (4,5) est controlé par le rouge (0,4)
	 * En (1,4) le rouge allume le (5,5)
	 * En (4,5) le rouge allume le (partout)
	 * En (5,5) le rouge allume le (partout)
	 * 
	 **/
	if (y == 4)
	{
		if (x == 0 || x == 1)
		{
			colors[i + 4] = r;
			colors[i + LED_GROUP_ONE_COLOR] = g;
			colors[i + LED_GROUP_TWO_COLOR] = b;
			overrided = true;
		}
	}
	else if (y == 4)
	{
		if (x == 4 || x == 5)
		{
			colors[i - 4] = r;
			colors[i + LED_GROUP_ONE_COLOR] = g;
			colors[i + LED_GROUP_TWO_COLOR] = b;
			overrided = true;
		}
	}

	if (!overrided)
	{
		colors[i] = r;
		colors[i + LED_GROUP_ONE_COLOR] = g;
		colors[i + LED_GROUP_TWO_COLOR] = b;
	}
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
	for (int pattern = 0; pattern < 2 && !ask_to_stop; pattern++)
	{
		int start = getLocalTimeMS();
		loop = 0;
		do
		{
			if (pattern == 0)
				pattern_guirlande();
			else if (pattern == 1)
				pattern_ligne_balai_alterne();

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