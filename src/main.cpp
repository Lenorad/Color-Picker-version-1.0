#include <SFML\Graphics.hpp>
#include <iomanip>
#include <sstream>
#include <math.h>
#include <string>
#define PI 3.1415926

static int resolution = 2;
int RGBToHue(sf::Color rgb);
sf::Color HueToRGB(int Hue, float Saturation = 1.0f, float Value = 1.0f);

struct Triangle
{
	sf::Vector2f pos;
	sf::Color color;
	sf::Vertex* vertices{ new sf::Vertex[3]{} };
	Triangle(sf::Vector2f Ipo, sf::Color Icolor)
	{
		pos = Ipo;
		color = Icolor;

		vertices[0] = sf::Vertex(sf::Vertex({ pos.x     , pos.y }, color));
		vertices[1] = sf::Vertex(sf::Vertex({ pos.x + 10, pos.y + 10 }, color));
		vertices[2] = sf::Vertex(sf::Vertex({ pos.x     , pos.y + 20 }, color));
	}
	void update(sf::Vector2f Ipo, sf::Color Icolor)
	{
		pos = Ipo;
		color = Icolor;

		vertices[0] = sf::Vertex(sf::Vertex({ pos.x     , pos.y }, color));
		vertices[1] = sf::Vertex(sf::Vertex({ pos.x + 10, pos.y + 10 }, color));
		vertices[2] = sf::Vertex(sf::Vertex({ pos.x     , pos.y + 20 }, color));
	}

};

struct Circle
{
	sf::Vector2f pos;
	sf::Color Pcolor;
	sf::Color Scolor;
	sf::CircleShape circle;

	int radius = 15;
	int Hue = 0;
	float Saturation = 1;
	float Value = 1;

	float preSat = 0.0f;
	float preVal = 0.0f;

	Circle(sf::Vector2f Ipo, sf::Color Icolor, int rad)
	{
		pos = Ipo;
		Pcolor = Icolor;
		Scolor = Icolor;
		radius = rad;
	}

	void setCircleProperties(size_t pc = 40, sf::Color outlineColor = sf::Color(0, 0, 0), float outlineThickness = 2)
	{
		circle.setRadius(radius);
		circle.setPointCount(pc);
		circle.setOutlineColor(outlineColor);
		circle.setOutlineThickness(outlineThickness);
		circle.setFillColor(Pcolor);
		circle.setPosition(pos.x - radius, pos.y - radius);

	}
	void changePosition(sf::Vector2f po)
	{
		Saturation = (po.x - 14) / (255 * resolution);
		Value = 1.0 - (po.y - 14) / (255 * resolution);
	}
	void update()
	{
		int offsetX = 0;
		int offsetY = 0;
		int vi = 255 - (Value * 255);
		sf::Color visible_ind = sf::Color(vi, vi, vi);
		offsetX = (255 * resolution) - (255 * resolution) * Saturation;
		offsetY = (255 * resolution) - (255 * resolution) * Value;
		Hue = RGBToHue(Pcolor);
		Scolor = HueToRGB(Hue, Saturation, Value);
		circle.setPosition(pos.x-radius-offsetX, pos.y-radius+offsetY);
		circle.setFillColor(Scolor);
		circle.setOutlineColor(visible_ind);
	}
};

struct ColorCodes
{
	const char* name;
	std::string value;
};

sf::Vertex* Get_ColorHue();
std::string addSpaces(std::string str);
sf::Vertex* Get_ColorTable(sf::Color PrimaryColor);
void Update_ColorTable(sf::Color PrimaryColor, sf::Vertex* colortable);
void Update_text(sf::Text& txt, ColorCodes* ccd, const int& hue, const float& saturation, const float& value);
sf::Color Update(sf::Vector2f tripo, sf::Color Pcolor, sf::Vertex* CTable, Triangle& triangle, Circle& circle);

int main()
{
	// Window
	sf::RenderWindow window(sf::VideoMode(700, 750), "Color Picker", sf::Style::Titlebar | sf::Style::Close);

	// Default Colors
	sf::Color PrimaryColor = sf::Color(255, 0, 0);
	sf::Color SelectedColor = PrimaryColor;
	int Hue = RGBToHue(SelectedColor);
	float Sat = 1;
	float Val = 1;
	
	// ColorTable
	sf::Vector2f sp(15, 15);
	sf::Vertex* colortable;
	colortable = Get_ColorTable(PrimaryColor);

	// ColorHue
	sf::Vector2f sq(580, 15);
	sf::Vertex* colorhue;
	colorhue = Get_ColorHue();

	// Font
	sf::Font font;
	if (!font.loadFromFile("CaviarDreams.ttf"))
		return -1;

	// Color Values (individual)
	sf::Text Text;
	std::ostringstream osstr;
	Text.setPosition(15, 550);
	Text.setFont(font);
	Text.setCharacterSize(30);
	struct ColorCodes codes[] =
	{
		{"Red", addSpaces(std::to_string(SelectedColor.r))  },
		{"Hue", addSpaces(std::to_string(Hue))},
		{"Green", addSpaces(std::to_string(SelectedColor.g))},
		{"Saturation", addSpaces(std::to_string(int(Sat * 100)))},
		{"Blue", addSpaces(std::to_string(SelectedColor.b))},
		{"Value", addSpaces(std::to_string(int(Val * 100)))}
	};
	Update_text(Text, codes, Hue, Sat, Val);


	// Triangle ColorHue Indicator
	sf::Vector2f triPo(570, 5);
	sf::Color triColor = PrimaryColor;
	sf::Vertex* triVertex;
	struct Triangle triangle = { triPo, triColor };
	triVertex = triangle.vertices;
	
	// Circle ColorTable Indicator
	sf::Vector2f cirPo(524, 14);
	sf::Vector2f initCirclePo;
	sf::Color cirColor = PrimaryColor;
	int radius = 12;
	size_t point_count = 40;
	struct Circle circle = { cirPo, cirColor, radius};
	circle.setCircleProperties(point_count);

	// Event
	sf::Clock clock;
	bool Pressed = false;
	bool clickInTab = false;
	bool clickInHue = false;
	bool control = false;
	bool shift = false;
	sf::Vector2i preMouseMoved(0,0);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.control)
					control = true;
				else if (event.key.shift)
					shift = true;
			}	
			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Key::LControl)
					control = false;
				else if (event.key.code == sf::Keyboard::Key::LShift)
					shift = false;
			}
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				double elapsed;
				int scroll_increment;
				float delta = event.mouseWheelScroll.delta;
				if (clock.getElapsedTime().asSeconds() > 1)
				{
					clock.restart();
					elapsed = 1;
				}
				else
					elapsed = clock.restart().asSeconds();

				if (shift)
				{
					scroll_increment = pow(2.0 / (elapsed * resolution), 0.4);
					circle.Saturation += -delta * resolution * float(scroll_increment) / 100;
					if (circle.Saturation > 1)
						circle.Saturation = 1;
					else if (circle.Saturation < 0)
						circle.Saturation = 0;
					circle.update();
				}
				else if (control)
				{
					scroll_increment = pow(2.0 / (elapsed * resolution), 0.4);
					circle.Value += delta * resolution * float(scroll_increment) / 100;
					if (circle.Value > 1)
						circle.Value = 1;
					else if (circle.Value < 0)
						circle.Value = 0;
					circle.update();
				}
				else if (!shift && !control)
				{
					scroll_increment = pow(2.0 / (elapsed * resolution), 0.7);
					triPo.y += -delta * resolution * scroll_increment;
					if (triPo.y < 5)
						triPo.y = 723;
					else if (triPo.y > 723)
						triPo.y = 5;

					Hue = int(triPo.y - 5) / resolution;
					sf::Color color = HueToRGB(Hue);
					PrimaryColor = Update(triPo, color, colortable, triangle, circle);
				}
				Update_text(Text, codes, Hue, circle.Saturation, circle.Value);

			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				Pressed = true;
				if (event.mouseButton.button == sf::Mouse::Button::Left)
				{
					float x = event.mouseButton.x;
					float y = event.mouseButton.y;

					// click in color table
					if (x >= 14 && x <= 524 && y >= 14 && y <= 524)
					{
						circle.changePosition({x,y});
						circle.update();
						clickInTab = true;
						clickInHue = false;
						initCirclePo = { x, y };
					}
					// click in color hue
					else if (x >= 580 && x <= 640 && y >= 15 && y <= 735)
					{
						triPo.y = y - 10;
						Hue = (y - 15) / resolution;
						sf::Color color = HueToRGB(Hue);
						PrimaryColor = Update(triPo, color, colortable, triangle, circle);
						clickInTab = false;
						clickInHue = true;
					}
					else
					{
						clickInTab = false;
						clickInHue = false;
					}
					Update_text(Text, codes, Hue, circle.Saturation, circle.Value);
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				Pressed = false;
			}
			if (event.type == sf::Event::MouseMoved)
			{
				if (Pressed)
				{
					float change_x = event.mouseMove.x - preMouseMoved.x;
					float change_y = event.mouseMove.y - preMouseMoved.y;
					float x = event.mouseMove.x;
					float y = event.mouseMove.y;

					// moved in color table
					if (x >= 14 && x <= 524 && y >= 14 && y <= 524 && clickInTab)
					{
						circle.changePosition({x, y});
						circle.update();
					}
					// moved in color hue
					else if (event.mouseMove.x >= 580 && event.mouseMove.x <= 640 && event.mouseMove.y >= 15 && event.mouseMove.y <= 733 && clickInHue)
					{
						triPo.y += change_y;
						Hue = (triPo.y - 4) / resolution;
						sf::Color color = HueToRGB(Hue);
						PrimaryColor = Update(triPo, color, colortable, triangle, circle);
					}
					Update_text(Text, codes, Hue, circle.Saturation, circle.Value);
				}
				preMouseMoved.x = event.mouseMove.x;
				preMouseMoved.y = event.mouseMove.y;
			}
		}
		window.clear(sf::Color(53, 53, 53));
		window.draw(colortable, 1020, sf::Lines);
		window.draw(colorhue, 1480, sf::Lines);
		window.draw(Text);
		window.draw(triVertex, 3, sf::TriangleStrip);
		window.draw(circle.circle);
		window.display();
	}
	return 0;
}

sf::Color HueToRGB(int Hue, float Saturation, float Value)
{
	sf::Color color;

	float M = 255 * Value;
	float m = M * (1 - Saturation);

	float Hh = static_cast<float>(Hue) / 60;
	int Hi = int(Hh);
	float Hf = Hh - Hi;

	float z = (M - m) * (1 - abs((Hi % 2) + Hf - 1));

	if (0 <= Hue && Hue < 60)
	{
		color.r = static_cast<uint8_t>(M);
		color.g = static_cast<uint8_t>(z + m);
		color.b = static_cast<uint8_t>(m);
	}
	else if (60 <= Hue && Hue < 120)
	{
		color.r = static_cast<uint8_t>(z + m);
		color.g = static_cast<uint8_t>(M);
		color.b = static_cast<uint8_t>(m);
	}
	else if (120 <= Hue && Hue < 180)
	{
		color.r = static_cast<uint8_t>(m);
		color.g = static_cast<uint8_t>(M);
		color.b = static_cast<uint8_t>(z + m);
	}
	else if (180 <= Hue && Hue < 240)
	{
		color.r = static_cast<uint8_t>(m);
		color.g = static_cast<uint8_t>(z + m);
		color.b = static_cast<uint8_t>(M);
	}
	else if (240 <= Hue && Hue < 300)
	{
		color.r = static_cast<uint8_t>(z + m);
		color.g = static_cast<uint8_t>(m);
		color.b = static_cast<uint8_t>(M);
	}
	else if (300 <= Hue && Hue < 360)
	{
		color.r = static_cast<uint8_t>(M);
		color.g = static_cast<uint8_t>(m);
		color.b = static_cast<uint8_t>(z + m);
	}
	return color;
}
int RGBToHue(sf::Color rgb)
{
	int Hue;
	double r = rgb.r;
	double g = rgb.g;
	double b = rgb.b;
	if (r == g && g == b)
		return 0;
	double Inradian = double(r - (g / 2) - (b / 2)) / sqrt(r * r + g * g + b * b - r * g - r * b - g * b);
	Hue = ceil(acos(Inradian) * 180 / PI);
	if (!(g >= b))
		if (b > g || Hue != 360)
			Hue = -Hue + 360;
	return Hue;
}
sf::Vertex* Get_ColorTable(sf::Color PrimaryColor)
{
	int resolution = 2;
	sf::Vector2f starting_pos(15, 15);

	sf::Vertex* colortable = new sf::Vertex[510* resolution]{ };
	const sf::Color PColor = PrimaryColor;

	int Y_increment = 0;
	float V = 255;
	float X = 0.0f;
	for (int i = 0; i < 255 * resolution; i++)
	{
		colortable[i * 2]   = sf::Vertex(sf::Vertex(sf::Vector2f(starting_pos.x                  , starting_pos.y + Y_increment), sf::Color(ceil(V), ceil(V), ceil(V))));
		colortable[i * 2 + 1] = sf::Vertex(sf::Vertex(sf::Vector2f(starting_pos.x + 255 * resolution, starting_pos.y + Y_increment), PrimaryColor));
		PrimaryColor.g = static_cast<uint8_t>(PColor.g - round((float(PColor.g) / 255 * floor(X))));
		PrimaryColor.b = static_cast<uint8_t>(PColor.b - round((float(PColor.b) / 255 * floor(X))));
		PrimaryColor.r = static_cast<uint8_t>(PColor.r - round((float(PColor.r) / 255 * floor(X))));
		Y_increment++;
		V -= 1.0f / resolution;
		X += 1.0f / resolution;
	}
	return colortable;
}
sf::Vertex* Get_ColorHue()
{
	int resolution = 2;
	sf::Vector2f starting_pos(580, 15);

	sf::Vertex* colorhue = new sf::Vertex[720 * resolution]{ };

	float hue = 0;
	int Y_increment = 0;
	for (int i = 0; i < 360 * resolution; i++)
	{
		colorhue[i * 2 ]    = sf::Vertex(sf::Vertex(sf::Vector2f(starting_pos.x                  , starting_pos.y + Y_increment), HueToRGB(floor(hue))));
		colorhue[i * 2 + 1] = sf::Vertex(sf::Vertex(sf::Vector2f(starting_pos.x + 30 * resolution, starting_pos.y + Y_increment), HueToRGB(floor(hue))));
		Y_increment++;
		hue += 1.0f / resolution;
	}
	return colorhue;
}
sf::Color Update(sf::Vector2f tripo, sf::Color Pcolor, sf::Vertex* Ctable, Triangle& triangle, Circle& circle)
{
	Update_ColorTable(Pcolor, Ctable);
	triangle.update(tripo, Pcolor);
	circle.Pcolor = Pcolor;
	circle.update();
	return Pcolor;
}
void Update_ColorTable(sf::Color PrimaryColor, sf::Vertex* colortable)
{
	int resolution = 2;
	const sf::Color PColor = PrimaryColor;

	float X = 0.0f;
	for (int i = 0; i < 255 * resolution; i++)
	{
		colortable[i * 2 + 1].color = PrimaryColor;
		PrimaryColor.g = static_cast<uint8_t>(PColor.g - round((float(PColor.g) / 255 * floor(X))));
		PrimaryColor.b = static_cast<uint8_t>(PColor.b - round((float(PColor.b) / 255 * floor(X))));
		PrimaryColor.r = static_cast<uint8_t>(PColor.r - round((float(PColor.r) / 255 * floor(X))));
		X += 1.0f / resolution;
	}
}
void Update_text(sf::Text& txt, ColorCodes* ccd, const int& hue, const float& saturation, const float& value)
{
	std::ostringstream osstr;
	const sf::Color SColor = HueToRGB(hue, saturation, value);
	uint8_t Red = SColor.r;
	uint8_t Gee = SColor.g;
	uint8_t Blu = SColor.b;
	float Val = value;
	float Sat = saturation;

	ccd[0].value = addSpaces(std::to_string(Red));
	ccd[1].value = addSpaces(std::to_string(hue));
	ccd[2].value = addSpaces(std::to_string(Gee));
	ccd[3].value = addSpaces(std::to_string(int(Sat * 100)));
	ccd[4].value = addSpaces(std::to_string(Blu));
	ccd[5].value = addSpaces(std::to_string(int(Val* 100)));

	osstr << ccd[0].name << "   \t" << ccd[0].value << "\t\t" << ccd[1].name << "   \t\t" << ccd[1].value << '°' << "\n" << std::endl;
	osstr << ccd[2].name << "\t" << ccd[2].value << "\t\t" << ccd[3].name << "  " << ccd[3].value << '%' << "\n" << std::endl;
	osstr << ccd[4].name << "   \t" << ccd[4].value << "\t\t" << ccd[5].name << "\t\t" << ccd[5].value << '%' << std::endl;

	txt.setString(osstr.str());
}
std::string addSpaces(std::string str)
{
	std::string spaces;
	for (int i = str.length(); i < 3; i++)
		spaces += " ";
	return spaces + str;
}
