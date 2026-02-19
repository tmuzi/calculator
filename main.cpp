// g++ main.cpp `pkg-config --cflags --libs sdl3`

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include <vector>

/* Types */
typedef struct {
	const std::string label;
	const int rgba[4];
	bool special;
	//SDL_FRect rect;
} Button;

/* Constants */
//Screen dimension constants
constexpr int kScreenHeight{ 518 };
constexpr int kScreenWidth{ 320 };

/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow{ nullptr };

//The surface contained by the window
SDL_Surface* gScreenSurface{ nullptr };
SDL_Renderer* gRenderer{ nullptr };

// mouse pos
float mouseX {}, mouseY {};
bool mouseLClicked { false };

/* output */
bool evaluated { false };

class Calculator
{
	public:
		std::string screenOutput { "" };
		std::string evalExpr { "" };

		//screen
		float startX { 0 };
		float startY { 0 };

		float screenHeight { kScreenHeight * 0.25f };
		float screenWidth { kScreenWidth };

		//board
		float boardTopY { screenHeight };
		float boardTopX { startX };

		float boardHeight { kScreenHeight * 0.75f };
		float boardWidth { kScreenWidth };

		float buttonHeight { boardHeight * 0.1667f };
		float buttonWidth { boardWidth * 0.25f };

		// button layout
		int lWidth {4}, lHeight {6};

		Button buttons [6][4] = {
			{{"", {0xA9, 0xA9, 0xA9, 0xFF}, false}, {"", {0xA9, 0xA9, 0xA9, 0xFF}, false}, {"", {0xA9, 0xA9, 0xA9, 0xFF}, false}, {"", {0xA9, 0xA9, 0xA9, 0xFF}, false}}, // empty first row
			{{"AC", {0xFF, 0xA5, 0xFF, 0xFF}, true}, {"(", {0xA5, 0xA5, 0xA5, 0xFF}, false}, {")", {0xA5, 0xA5, 0xA5, 0xFF}, false}, {"/", {0xA5, 0xA5, 0xA5, 0xFF}, false}},
			{{"7", {0x00, 0x00, 0xFF, 0xFF}, false}, {"8", {0x00, 0x00, 0xFF, 0xFF}, false}, {"9", {0x00, 0x00, 0xFF, 0xFF}, false}, {"*", {0xA5, 0xA5, 0xA5, 0xFF}, false}},
			{{"4", {0x00, 0x00, 0xFF, 0xFF}, false}, {"5", {0x00, 0x00, 0xFF, 0xFF}, false}, {"6", {0x00, 0x00, 0xFF, 0xFF}, false}, {"-", {0xA5, 0xA5, 0xA5, 0xFF}, false}},
			{{"1", {0x00, 0x00, 0xFF, 0xFF}, false}, {"2", {0x00, 0x00, 0xFF, 0xFF}, false}, {"3", {0x00, 0x00, 0xFF, 0xFF}, false}, {"+", {0xA5, 0xA5, 0xA5, 0xFF}, false}},
			{{".", {0xA5, 0xA5, 0xFF, 0xFF}, false}, {"0", {0x00, 0x00, 0xFF, 0xFF}, false}, {"DEL", {0xA5, 0x00, 0x00, 0xFF}, true}, {"=", {0xA5, 0xFF, 0xFF, 0xFF}, true}}
		};

				
		void drawLayoutScreen()
		{
			SDL_FRect screenRect { startX, startY, screenWidth, screenHeight };

			SDL_SetRenderDrawColor(gRenderer, 0xD3, 0xD3, 0xD3, 0xFF); // light gray
			SDL_RenderFillRect(gRenderer, &screenRect);

			//	write to screen
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_SetRenderScale(gRenderer, 3.5f, 3.5f);
			SDL_RenderDebugText(gRenderer, 10, 20, screenOutput.c_str()); /* update screen */

			SDL_SetRenderScale(gRenderer, 1.0f, 1.0f); // reset scale
		}

		void drawLayoutBoard()
		{
			SDL_FRect boardRect { boardTopX, boardTopY, boardWidth, boardHeight };

			SDL_SetRenderDrawColor(gRenderer, 0xA9, 0xA9, 0xA9, 0xFF); // dark gray (board background)
			SDL_RenderFillRect(gRenderer, &boardRect);

			//buttons
			SDL_FRect buttonRect { boardTopY, boardTopY, buttonWidth - 1.0f, buttonHeight - 1.0f }; // button with gap

			for (int row = 0; row < lHeight; row++)
			{
				buttonRect.y = boardTopY + buttonHeight * row;
				for (int col = 0; col < lWidth; col++)
				{
					int gridIdx { (row * (4)) + col + 1 };

					buttonRect.x = boardTopX + buttonWidth * col;

					SDL_SetRenderDrawColor(gRenderer,
						buttons[row][col].rgba[0],
						buttons[row][col].rgba[1],
						buttons[row][col].rgba[2],
						buttons[row][col].rgba[3]
					);

					SDL_RenderFillRect(gRenderer, &buttonRect);

					//	write to button
					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF); // dark gray
					//SDL_SetRenderScale(gRenderer, 3.5f, 3.5f);

					std::string button { buttons[row][col].label };
					int buttonLen { (int)button.length() };

					SDL_RenderDebugText(
						gRenderer,
						(buttonRect.x + buttonWidth / 2) - (4 * buttonLen),
						(buttonRect.y + buttonHeight / 2) - 4,
						button.c_str()
					); // default char dim 8x8 hence -4 to center /* update screen */

				}
				buttonRect.x = boardTopX;
			}
		}

		void calculatorOperations()
		{
			//test mouse co-ords
			if (mouseLClicked) {
				if (evaluated)
				{
					screenOutput = "";
					evaluated = false;
				}

				if (mouseX > 0 && mouseX < boardWidth)
				{
					if (mouseY > screenHeight && mouseY < kScreenHeight) // full screen width to bottom
					{
						int row { (int)((mouseY - (screenHeight)) / buttonHeight) }; // convert to board coords
						int col { (int)(mouseX / buttonWidth) };

						Button button { buttons[row][col] };

						if (button.special)
						{
							if (button.label == "AC")
								screenOutput = "";
							else if (button.label == "DEL")
							{
								if (!screenOutput.empty())
									screenOutput.pop_back();
							}
							else if (button.label == "=")
							{
								evalExpr = evaluate(screenOutput); // evaluate
								screenOutput = evalExpr;
								evaluated = true;
							}
						}
						else
						{
							if (button.label != "(" && button.label != ")")
								screenOutput.push_back(button.label[0]);
						}
					}
				}
				mouseLClicked = false;
			}
		}

		std::string evaluate(std::string expression)
		{
			std::string opLeft { "" }, opRight { "" };
			char currentOp { ' ' };
			float result { 0.0f };

				int i = 0;

				while (i < expression.length())
				{
					char c { expression[i] };

					if (c == '+' || c == '-' || c == '*' || c == '/')
						break;

					opLeft.push_back(c);

					i++;
				}
				
				result = { std::stof(opLeft) };
				std::string newPlainBuffer = expression.substr(i);

				for ( char c: newPlainBuffer)
				{
					if (c == '+' || c == '-' || c == '*' || c == '/')
					{
						if (!opRight.empty())
						{
							result = evaluateHelper(std::to_string(result), currentOp, opRight); // check 0 edge case return

							opRight = "";
							currentOp = c;
						}
						else
						{
							currentOp = c;
							continue;
						}
					}
					else
						opRight.push_back(c);
						
				}

			result = evaluateHelper(std::to_string(result), currentOp, opRight); // final op

			//SDL_Log("ANSWER: %f", result);
			return std::to_string(result);

		}

		float evaluateHelper(const std::string opLeft, char op, const std::string opRight)
		{
			SDL_Log("To eval %s %c %s", opLeft.c_str(), op, opRight.c_str());

			switch (op)
			{
			case '+':
				return (std::stof(opLeft) + std::stof(opRight));

			case '-':
				return (std::stof(opLeft) - std::stof(opRight));
							
			case '*':
				return (std::stof(opLeft) * std::stof(opRight));

			case '/':
				return (std::stof(opLeft) / std::stof(opRight));

			default:
				break;
			}
			return 0.0f;
		}
};

/* Function Prototypes */
bool init();

bool key_events();
void draw(Calculator calculator);

void close();

int main()
{
	Calculator calculator;

	if (!init())
		return 1;

	while (key_events())
	{
		draw(calculator);
		calculator.calculatorOperations();
	}

	close();

	return 0;
}

void draw(Calculator calculator)
{
	//Fill the surface white
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	/*
		layout:

		screen 25%:
		-------------
		|			|
		|			|
		board 75%:
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		-------------
	*/

	calculator.drawLayoutScreen();

	calculator.drawLayoutBoard();

	SDL_RenderPresent(gRenderer);
}

bool init()
{


	if (SDL_Init(SDL_INIT_VIDEO) == false)
	{
		SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());

		return false;
	}

	if (!SDL_CreateWindowAndRenderer("Calculator", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer))
	{
		SDL_Log("Window/Renderer could not be created! SDL error: %s\n", SDL_GetError());

		return false;
    }

	return true;
}

bool key_events()
{
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		switch (e.type) {
			case SDL_EVENT_QUIT:
return false;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				//SDL_Log("Mouse Pressed");
				if (e.button.button == SDL_BUTTON_LEFT)
					{
						SDL_GetMouseState(&mouseX, &mouseY);
						mouseLClicked = true;
					}
		}
	}
	return true;
}

void close()
{

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = nullptr;
	gScreenSurface = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}
