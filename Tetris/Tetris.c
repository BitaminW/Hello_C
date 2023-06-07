
#include <stdio.h>
#include <windows.h>


void renderTitle();
void gameLoop();
void initGame();
void gotoxy(int, int);


void centerConsoleWindow();



void gameLoop() {
	initGame();
	renderTitle();


}


void renderTitle() {

	gotoxy(15, 20);


}


void initGame() {
	centerConsoleWindow();
	system("mode con:cols=80 lines=50");
}

void gotoxy(int x, int y) {
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void centerConsoleWindow() {
	HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (console_handle == INVALID_HANDLE_VALUE)
		return;

	CONSOLE_SCREEN_BUFFER_INFO console_info;
	if (!GetConsoleScreenBufferInfo(console_handle, &console_info)) 
		return;
	

	SMALL_RECT window_rect = console_info.srWindow;
	COORD window_size = { window_rect.Right - window_rect.Left + 1, window_rect.Bottom - window_rect.Top + 1 };
	
	HWND console_window = GetConsoleWindow();
	if (console_window == NULL)
		return;

	RECT desktop_rect;
	GetWindowRect(GetDesktopWindow(), &desktop_rect);

	int screen_width = desktop_rect.right - desktop_rect.left;
	int screen_height = desktop_rect.bottom - desktop_rect.top;

	int window_left = (screen_width - window_size.X) / 2 - 200;
	int window_top = (screen_height - window_size.Y) / 2 - 500;

	MoveWindow(console_window, window_left, window_top, window_size.X, window_size.Y, TRUE);
}


int main(){


	gameLoop();

	
	
	return 0;
}
