#include <math.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <complex>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "CNumber.h"

long double modulus(sf::Vector2f c);

void seqFunc(sf::Vector2f* seq, sf::Vector2f c);
void seqFunc(std::complex<long double>* seq, std::complex<long double> c);
void seqFunc(CNumber<long double>* seq, CNumber<long double> c);

void calcCol(sf::RenderWindow& window, sf::VertexArray* cList, int x, int x_acc, int y_acc, sf::RectangleShape calcArea, int maxIter, long double zoomFactor);

void calcCList(sf::RenderWindow& window, sf::VertexArray* cList, int x_acc, int y_acc, int maxIter, sf::RectangleShape calcArea);

bool belongsToVArray(sf::VertexArray* vArray, sf::Vector2f position);  // Not usable, VERY slow

std::mutex mtx;
const int threadNumber = 10; // Number of threads running at the same time
long double lastZoomFactor = 0;  // Last Zoom Factor
sf::RectangleShape lastCalcArea;
sf::Image lastImg;

int main()
{

	///------- Creation of the window -------///

	// The following line registers all fullscreen modes are available for this computer
	//std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	sf::VideoMode deskMode = sf::VideoMode::getDesktopMode();

	// Using the following method make the window be in fullscreen mode at the start of the program
	//sf::RenderWindow window(modes[0], "Conway's Game of Life simulation", sf::Style::Fullscreen);
	sf::ContextSettings settings(0U, 0U, 16);
	// Initializes the window with a width of 512 pixels and a height of 256 pixels
	sf::RenderWindow window(deskMode, "Mandelbrot Set", sf::Style::Default, settings);

	// Creates a render-texture, which will contain what the window displays
	sf::RenderTexture texture;
	if (!texture.create(window.getSize().x, window.getSize().y))
		return -1;

	// Sets the framerate limit of the window
	window.setFramerateLimit(60);

	///--------------------------------------///

	// Variables declarations //

	sf::VertexArray cList(sf::PrimitiveType::Points, 0);
	float accFactor = 1.1;
	int x_acc = deskMode.width * accFactor; // X Accuracy
	int y_acc = deskMode.height * accFactor; // Y Accuracy

	bool hasCalculated = false;
	bool hasUpdated = false;

	int maxIter = 400; // Number of iterations before concluding a point belongs to the Mandelbrot Set

	//------------------------//

	// Rectangle around cursor //

	int zoomRectBaseWidth = window.getSize().x / 10;
	sf::RectangleShape zoomRect(sf::Vector2f(zoomRectBaseWidth, zoomRectBaseWidth * window.getSize().y / window.getSize().x));
	zoomRect.setPosition(sf::Vector2f(0, 0));
	zoomRect.setFillColor(sf::Color::Transparent);
	zoomRect.setOutlineThickness(2);
	zoomRect.setOrigin(sf::Vector2f(zoomRect.getSize().x / 2, zoomRect.getSize().y / 2));

	//-------------------------//

	///------- Main loop -------///
	while (window.isOpen())
	{
		///--- Event handler ---///

		sf::Event event;
		while (window.pollEvent(event))
		{
			// Checking the event type
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;

				case sf::Event::MouseWheelScrolled:
					if (zoomRect.getScale().x > 0.1 || event.mouseWheelScroll.delta < 0)
						zoomRect.setScale(sf::Vector2f(zoomRect.getScale().x + event.mouseWheelScroll.delta / -10, zoomRect.getScale().y + event.mouseWheelScroll.delta / -10));
					else
						zoomRect.setScale(0.1, 0.1);
					break;

				case sf::Event::Resized:
				{
					sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
					window.setView(sf::View(visibleArea));  // Unstretching the figure
					texture.setView(sf::View(visibleArea));

					x_acc = event.size.width * accFactor;  // Resizing the pixels number in the vertex array
					y_acc = event.size.height * accFactor;

					zoomRectBaseWidth = event.size.width / 10;
					zoomRect.setSize(sf::Vector2f(zoomRectBaseWidth, zoomRectBaseWidth * event.size.height / event.size.width));
					zoomRect.setOrigin(sf::Vector2f(zoomRect.getSize().x / 2, zoomRect.getSize().y / 2)); // Resizing and repositionning the zoom rectangle
					std::cout << "Resized : " << window.getSize().x << ' ' << window.getSize().y << std::endl;

					hasUpdated = true;
				}

				case sf::Event::MouseButtonReleased:  // Zooming
				{
					if (event.mouseButton.button == sf::Mouse::Middle)
					{
						cList.clear();
						sf::RectangleShape calcArea;

						calcArea.setSize(sf::Vector2f(zoomRect.getSize().x * zoomRect.getScale().x / lastZoomFactor, zoomRect.getSize().y * zoomRect.getScale().y / lastZoomFactor));
						calcArea.setOrigin(0, 0);
						calcArea.setPosition(zoomRect.getPosition().x / lastZoomFactor - calcArea.getSize().x / 2 + lastCalcArea.getPosition().x, zoomRect.getPosition().y / lastZoomFactor - calcArea.getSize().y / 2 + lastCalcArea.getPosition().y);

						std::cout << lastZoomFactor << ' ' << calcArea.getSize().x << ' ' << calcArea.getSize().y << std::endl;
						std::cout << lastZoomFactor << ' ' << lastCalcArea.getPosition().x << ' ' << calcArea.getPosition().y << std::endl;

						calcCList(std::ref(window), &cList, x_acc, y_acc, maxIter, calcArea);

						std::cout << std::endl << cList[0].position.x << ' ' << cList[0].position.y << std::endl;
						std::cout << std::endl << cList[1].position.x << ' ' << cList[1].position.y << std::endl;
						std::cout << std::endl << cList[2].position.x << ' ' << cList[2].position.y << std::endl;
						std::cout << cList[cList.getVertexCount() - 1].position.x << ' ' << cList[cList.getVertexCount() - 1].position.y << std::endl;

						hasUpdated = true;
					}

					break;
				}

				default:
					break;
			}
		}

		///---------------------///


		///--- Program logics ---///

		zoomRect.setPosition(sf::Vector2f(window.mapPixelToCoords(sf::Vector2i(sf::Mouse::getPosition(window)))));

		if (!hasCalculated)
		{
			sf::RectangleShape calcArea(sf::Vector2f(3, 2.5));
			calcArea.setPosition(sf::Vector2f(-2, -1.25));
			calcCList(std::ref(window), &cList, x_acc, y_acc, maxIter, calcArea);

			hasCalculated = true;

			std::cout << "\n Calculation done" << '\n';
			std::cout << "Antialiasing level : " << window.getSettings().antialiasingLevel << std::endl;

			hasUpdated = true;
		}

		///----------------------///

		///--- Rendering the window ---///

		if (hasUpdated)
		{
			lastImg = texture.getTexture().copyToImage();
			if (lastImg.saveToFile("LastImg.png"))
				std::cout << "Image saved succesfully !" << std::endl;

			texture.clear(sf::Color(50, 10, 255));
			texture.draw(cList);
			texture.display();

			hasUpdated = false;
		}

		// This method fills the screen with white, acts as a refresh
		window.clear(sf::Color(50, 10, 255));

		window.draw(sf::Sprite(texture.getTexture()));
		//window.draw(cList);
		window.draw(zoomRect);

		/// Here will go the drawings (e.g. window.draw(someVertex); );

		// This method shows on the monitor all we have drawn on the window
		window.display();

		///----------------------------///
	}

	return 0;
}

long double modulus(sf::Vector2f c)
{
	// The modulus of a complex number
	return sqrt(pow(c.x, 2) + pow(c.y, 2));
}

void seqFunc(sf::Vector2f* seq, sf::Vector2f c)
{
	// The function that determines if a point belongs to the Mandelbrot Set
	//*seq = sf::Vector2f(pow(seq->x, 3) - 3 * seq->x * pow(seq->y, 2) + c.x, 3 * pow(seq->x, 3) * seq->y - pow(seq->y, 3) + c.y);
	*seq = sf::Vector2f(pow(seq->x, 2) - pow(seq->y, 2) + c.x, 2 * seq->x * seq->y + c.y);
}

void seqFunc(std::complex<long double>* seq, std::complex<long double> c)
{
	// The function that determines if a point belongs to the Mandelbrot Set
	//*seq = std::pow(*seq, 3) + c;
	*seq = std::pow(*seq, 2) + c;
}

void seqFunc(CNumber<long double>* seq, CNumber<long double> c)
{
	// The function that determines if a point belongs to the Mandelbrot Set
	//*seq = std::pow(*seq, 3) + c;
	seq->raiseToPow(2);
	*seq += c;
}

void calcCol(sf::RenderWindow& window, sf::VertexArray* cList, int x, int x_acc, int y_acc, sf::RectangleShape calcArea, int maxIter, long double zoomFactor)
{
	// This function determines which point of a set column belongs to the Mandelbrot Set
	for (int y = 0; y < y_acc; y++)  // y_acc = number of points in column
	{
		//sf::Vector2f seq(0, 0); // The start of the sequence
		std::complex<long double> zSeq(0, 0);
		CNumber<long double> cSeq(0, 0);

		sf::Vector2f c(0, 0);  // The point we are trying to determine :
		//std::complex<long double> z((calcArea.getSize().x * (long double)x / (long double)x_acc) + calcArea.getPosition().x, (calcArea.getSize().y * (long double)y / (long double)y_acc) + calcArea.getPosition().y);
		CNumber<long double> z((calcArea.getSize().x * (long double)x / (long double)x_acc) + calcArea.getPosition().x, (calcArea.getSize().y * (long double)y / (long double)y_acc) + calcArea.getPosition().y);
		sf::Color cl(0, 0, 0);
		for (int i = 0; i < maxIter; i++)
		{
			seqFunc(&cSeq, z);
			if (cSeq.mag() > 2)
			{
				long double clVal = ((long double)i / (long double)400) * 255; // Color change Value, we are making the color of the points not belonging to the Mandelbrot Set
				cl.r = 10 + clVal * 2;
				cl.g = 10 + clVal * 2;
				cl.b = 255 - clVal * 2;
				break;
			}
		}

		c.x = (z.getReal() - calcArea.getPosition().x) * zoomFactor; // c contains the coordinates of the point, which are usually contained between -3 and 3
		c.y = (z.getImag() - calcArea.getPosition().y) * zoomFactor; // Here we enlarge the resulting image, because before that point it is approximatively  6 pixels * 6 pixels, which is small
		mtx.lock();
		cList->append(sf::Vertex(c, cl));
		mtx.unlock();
	}
}

void calcCList(sf::RenderWindow& window, sf::VertexArray* cList, int x_acc, int y_acc, int maxIter, sf::RectangleShape calcArea)
{
	std::thread ths[threadNumber];  // Threads
	long double zoomFactor = window.getSize().y / (long double)calcArea.getSize().y;
	lastZoomFactor = zoomFactor;
	lastCalcArea = calcArea;
	for (int x = 0; x < int((x_acc) / threadNumber); x++) // x_acc = number of columns
	{
		std::cout << x << '\n';
		for (int t = 0; t < threadNumber; t++)
		{
			ths[t] = std::thread(calcCol, std::ref(window), cList, threadNumber * x + t, x_acc, y_acc, calcArea, maxIter, zoomFactor); // We initialize the threads
		}

		for (int t = 0; t < threadNumber; t++) // We wait for the threads to finish before creating the next ones
		{
			ths[t].join();
		}
	}
	std::cout << cList->getVertexCount() << std::endl;
}

bool belongsToVArray(sf::VertexArray* vArray, sf::Vector2f position)
{
	bool belongs = false;
	for (std::size_t i = 0; i < vArray->getVertexCount() || belongs == true; i++)
	{
		belongs = (vArray->operator[](i).position.x == position.x && vArray->operator[](i).position.y == position.y);
	}
	return belongs;
}
