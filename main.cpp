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

// mouse pos
float mouseX {}, mouseY {};
bool mouseLClicked { false };

/* output */
bool evaluated { false };

class SDL_Instance
{
	/* Global Variables */
	//The window we'll be rendering to
	SDL_Window* gWindow{ nullptr };

	//The surface contained by the window
	SDL_Surface* gScreenSurface{ nullptr };
	SDL_Renderer* gRenderer{ nullptr };

	public:
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

		// helper functions
		void clearBg(std::vector<int> hexArr)
		{
			SDL_SetRenderDrawColor(gRenderer, hexArr[0], hexArr[1], hexArr[2], hexArr[3]);
			SDL_RenderClear(gRenderer);
		}

		void drawRect(std::vector<int> hexArr, SDL_FRect& screenRect)
		{
			SDL_SetRenderDrawColor(gRenderer, hexArr[0], hexArr[1], hexArr[2], hexArr[3]); // light gray
			SDL_RenderFillRect(gRenderer, &screenRect);/*neccessary to use &?*/
		}

		void drawText(std::vector<int> hexArr, std::string screenOutput)
		{
			SDL_SetRenderDrawColor(gRenderer, hexArr[0], hexArr[1], hexArr[2], hexArr[3]);
			SDL_SetRenderScale(gRenderer, 3.5f, 3.5f);
			SDL_RenderDebugText(gRenderer, 10, 20, screenOutput.c_str()); /* update screen */

			SDL_SetRenderScale(gRenderer, 1.0f, 1.0f); // reset scale
		}

		void setCustomColorT(std::vector<int> hexArr) { SDL_SetRenderDrawColor(gRenderer, hexArr[0], hexArr[1], hexArr[2], hexArr[3]); };

		void drawCleanTextT(std::vector<float> posArr, std::string output)
		{
			SDL_RenderDebugText(gRenderer, posArr[0], posArr[1], output.c_str()); /* update screen */
		}

		void present() { SDL_RenderPresent(gRenderer); }
};

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

				
		void drawLayoutScreen(SDL_Instance mySdl)
		{
			SDL_FRect screenRect { startX, startY, screenWidth, screenHeight };

			mySdl.drawRect({0xD3, 0xD3, 0xD3, 0xFF}, screenRect);

			//	write to screen
			mySdl.drawText({0x00, 0x00, 0x00, 0xFF}, screenOutput);
		}

		void drawLayoutBoard(SDL_Instance mySdl)
		{
			SDL_FRect boardRect { boardTopX, boardTopY, boardWidth, boardHeight };

			mySdl.drawRect({0xA9, 0xA9, 0xA9, 0xFF}, boardRect); // dark gray (board background)

			//buttons
			SDL_FRect buttonRect { boardTopY, boardTopY, buttonWidth - 1.0f, buttonHeight - 1.0f }; // button with gap

			for (int row = 0; row < lHeight; row++)
			{
				buttonRect.y = boardTopY + buttonHeight * row;
				for (int col = 0; col < lWidth; col++)
				{
					int gridIdx { (row * (4)) + col + 1 };

					buttonRect.x = boardTopX + buttonWidth * col;

					mySdl.drawRect(
						
						{
							buttons[row][col].rgba[0],
							buttons[row][col].rgba[1],
							buttons[row][col].rgba[2],
							buttons[row][col].rgba[3]
						},
						buttonRect
					);

					//	write to button
					mySdl.setCustomColorT({0x00, 0x00, 0x00, 0xFF}); // dark gray
					//SDL_SetRenderScale(gRenderer, 3.5f, 3.5f);

					std::string button { buttons[row][col].label };
					int buttonLen { (int)button.length() };

					mySdl.drawCleanTextT(
						{
							(buttonRect.x + buttonWidth / 2) - (4 * buttonLen),
							(buttonRect.y + buttonHeight / 2) - 4
						},
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
				
				result = { !opLeft.empty() ? std::stof(opLeft) : 0 }; /* update scree accordingly (show the zero on OP press before #0) */
	
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

			SDL_Log("ANSWER: %f", result);
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
				// apparently / by zero is taken care of did it anyways
				return std::stof(opLeft) == 0 ? 0 : (std::stof(opLeft) / std::stof(opRight));

			default:
				break;
			}
			return 0.0f;
		}
};


int main()
{
	Calculator calculator;
	SDL_Instance mySdl;

	if (!mySdl.init())
		return 1;

	while (mySdl.key_events())
	{
		//Fill the surface white
		mySdl.clearBg({0xFF, 0xFF, 0xFF, 0xFF});

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

		calculator.drawLayoutScreen(mySdl);

		calculator.drawLayoutBoard(mySdl);

		mySdl.present();
		calculator.calculatorOperations();
	}

	mySdl.close();

	return 0;
}

