#pragma once

typedef unsigned short U_short;

enum Color { BLACK = 0, GREEN = 2, AQUA = 3, RED = 4, DEFAULT = 7, GREY = 8, LIGHTGREEN = 10, LIGHTPURPLE = 13 };
enum Keys { LEFT = 75, UP = 72, RIGHT = 77, DOWN = 80, ENTER = 13, SPACE = 32, DEL = 83, ESC = 27 };


void mySetColor(int fg, int bg) 
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, fg | (bg << 4));
}

void find_and_replace(string& text, char find, char replace) 
{
	for (size_t i = 0; i < text.size(); i++)
		if (text[i] == find) text[i] = replace;
}

void emptyItem(string path) 
{
	for (const auto& file : directory_iterator(path)) 
	{
		string currentPath = file.path().string();

		if (is_directory(currentPath)) 
			emptyItem(currentPath);
		
		remove(currentPath);
	}
}

bool startsWith(string main, string prefix) 
{
	string sourcePath = main + (main.back() == '\\' ? "" : "\\") + s_fileName.substr(s_fileName.rfind('\\') + 1);
	return  sourcePath == prefix.substr(0, sourcePath.size());
}

void cloneFolder(string from, string to) 
{
	to += ((to.back() == '\\') ? "" : "\\") + from.substr(from.rfind('\\') + 1);
	filesystem::create_directory(to);

	for (const auto& file : directory_iterator(from))
	{
		if (file.is_directory())cloneFolder(file.path().string(), to);
		else 
			filesystem::copy(file.path().string(), to);
	}
}

void clear_screen(char fill = ' ') 
{
	COORD tl = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO s;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(console, fill, cells, tl, &written);
	FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
	SetConsoleCursorPosition(console, tl);
}

void newItem() 
{
	string newName;

	if (::GetKeyState(VK_SHIFT) & 0x8000) 
	{
		cout << "Enter Diretory Name: ";
		getline(cin, newName);
		create_directory(dir + ((dir.back() == '\\') ? "" : "\\") + newName);
	}
	else 
	{
		cout << "Enter New File Name with Extension: ";
		getline(cin, newName);
		string newPath = dir + (dir.back() == '\\' ? "" : R"(\)") + newName;
		wstring w_Path(newPath.begin(), newPath.end());
		HANDLE hFile = CreateFile(w_Path.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(hFile);
	}
}

void helpMenu(bool miniEnvoirement = false) 
{
	clear_screen();

	cout << " ENTER       - OPEN Directory\n";
	if(!miniEnvoirement) cout << " ENTER       - OPEN File\n";
	cout << " M           - Move File or Directory\n";
	cout << " " << char(200) << char(205) << ">Shift + M - Confirm Move Path\n";
	cout << " Escape      - Back Directory\n";
	if (!miniEnvoirement) {
		cout << " DELETE      - Delete File\n";
		cout << " N           - New File\n";
		cout << " P           - Path Redirection\n";
		cout << " R           - Rename File\n";
		cout << " Shift + E   - EXIT CONSOLE\n";
		cout << " Shift + N   - New Directory\n";
	}

	cin.get();
}

string miniEnvironment(string startingPath) 
{
	/*The main program is a function that switches between directories
	It's just a simplified version for a specific purpose.

	When moving, I could store bool in global for additional behavior
	For example, IsMoveand I would check every time for behavior according to move
	if (IsMove) {} but in the main functions every time you do this command
	I wrote the miniEnvironment(small muxit) function so that it doesn't check hard.*/

	unsigned short choice = 0;
	string currentFileName;

	while (true)
	{
		clear_screen();

		cout << "PRESS SHIFT + M in the Directory you DESIRE TO MOVE:\n\n";
		unsigned short i = 0;

		for (const auto& file : directory_iterator(startingPath))
		{
			string currentPath = file.path().string();
			char prefix = ' ';
			if (i == choice) { currentFileName = currentPath; mySetColor(LIGHTGREEN, GREY); prefix = char(254); }
			cout << ' ' << prefix << ' ' << i++ << " - " << currentPath << endl;
			mySetColor(LIGHTGREEN, BLACK);
		}

		if (!i) cout << setw(50) << "FOLDER IS EMPTY" << endl;

		char kb = _getch();
		if (kb == 224 || kb == -32) 
		{
			kb = _getch();

			switch (kb) 
			{

			case DOWN:
			case RIGHT:
				choice++;
				choice %= i;
				break;
			case UP:
			case LEFT:
				if (choice == 0) choice = i;
				choice--;
				break;
			}
		}
		else 
		{
			switch (kb)
			{
			case ESC:
				startingPath = startingPath.substr(0, startingPath.rfind('\\'));
				choice = 0;
				break;
			case 'w':
			case 'W':
			case 'a':
			case 'A':
				if (choice == 0) choice = i;
				choice--;
				break;
			case 's':
			case 'S':
			case 'd':
			case 'D':
				choice++;
				choice %= i;
				break;
			case ENTER:
				keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
				if (is_directory(currentFileName)) 
				{ 
					startingPath = currentFileName; 
					choice = 0; 
				}
				break;
			case 'm':
			case 'M':
				if (::GetKeyState(VK_SHIFT) & 0x8000) return startingPath;

			case 'h':
			case 'H':
				if (::GetKeyState(VK_SHIFT) & 0x8000) helpMenu(true);
			}
		}
	}
}

void reDirectory()
{
	cout << "Enter directory: ";
	getline(cin, dir);
	find_and_replace(dir, '/', '\\');
	if (dir.back() == '\\') 
		dir.pop_back();
}

void moveItem() 
{
	string newDir = miniEnvironment(dir);
	if (startsWith(dir, newDir)) 
		throw exception("Folder can not be moved into Source Folder... ");
	
	if (filesystem::is_directory(s_fileName)) 
	{
		cloneFolder(s_fileName, newDir);
		emptyItem(s_fileName);
	}
	else  
		copy(s_fileName, newDir);
	
	remove(s_fileName);
}

void renameItem() 
{
	string newName;
	cout << "Enter new NAME (without extension): ";
	getline(cin, newName);
	rename(s_fileName, s_fileName.substr(0, s_fileName.rfind('\\') + 1)
		+ newName + ((!is_directory(s_fileName)) ? s_fileName.substr
		(s_fileName.rfind('.')) : ""));
}


void manageConsoleInput(unsigned short& choice, unsigned short& answerCount)
{
	{
		// auto getKey = GetAsyncKeyState;
		// // Uİ'da dəyərlərin keyboardla idarəsini təmin edir.
		// while (true) {
		// 	if (getKey(0x57) || getKey(0x41) || getKey(VK_UP) || getKey(VK_LEFT)) {
		// 		if (choice == 0) choice = answerCount;
		// 		choice--;
		// 
		// 		return;
		// 	}
		// 	else if (getKey(0x53) || getKey(0x44) || getKey(VK_DOWN) || getKey(VK_RIGHT)) {
		// 		++choice %= answerCount;
		// 
		// 		return;
		// 	}
		// 	else if (getKey(VK_RETURN)) {
		// 		keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
		// 		if (is_directory(s_fileName)) { dir = s_fileName; choice = 0; }
		// 		else { ShellExecute(NULL, NULL, wstring(s_fileName.begin(), s_fileName.end()).c_str(), NULL, NULL, SW_SHOW); }
		// 
		// 		return;
		// 	}
		// 	else if (getKey(VK_DELETE)) {
		// 		keybd_event(VK_DELETE, 0, KEYEVENTF_KEYUP, 0);
		// 
		// 		if (is_directory(s_fileName)) emptyFile(s_fileName);
		// 		remove(s_fileName);
		// 
		// 		return;
		// 	}
		// 	else if (getKey(VK_ESCAPE)) {
		// 		keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0);
		// 
		// 		dir = dir.substr(0, dir.rfind('\\'));
		// 
		// 		return;
		// 	}
		// 	else if (getKey(0x52)) {
		// 		keybd_event(0x52, 0, KEYEVENTF_KEYUP, 0);
		// 
		// 		string newName;
		// 		getline(cin, newName);
		// 		rename(s_fileName, s_fileName.substr(0, s_fileName.rfind('\\') + 1)
		// 			+ newName + ((!is_directory(s_fileName)) ?
		// 				s_fileName.substr(s_fileName.rfind('.')) : ""));
		// 
		// 
		// 		return;
		// 	}
		// 	else if (getKey(0x4D)) {
		// 		keybd_event(0x4D, 0, KEYEVENTF_KEYUP, 0);
		// 
		// 		string newDir;
		// 		getline(cin, newDir);
		// 		if (filesystem::is_directory(s_fileName)) {
		// 			cloneRep(s_fileName, newDir);
		// 			emptyFile(s_fileName);
		// 		}
		// 		else {
		// 			copy(s_fileName, newDir);
		// 		}
		// 		remove(s_fileName);
		// 
		// 		return;
		// 	}
		// 	else if (getKey(0x4E)) {
		// 		string newName;
		// 		keybd_event(0x4E, 0, KEYEVENTF_KEYUP, 0);
		// 
		// 
		// 		if (GetKeyState(VK_SHIFT)) {
		// 			cout << "Enter Diretory Name: ";
		// 			getline(cin, newName);
		// 			create_directory(dir + ((dir.back() == '\\') ? "" : "\\") + newName);
		// 		}
		// 		else {
		// 			cout << "Enter new Name with Extension: ";
		// 			getline(cin, newName);
		// 			string newPath = dir + (dir.back() == '\\' ? "" : R"(\)") + newName;
		// 			wstring w_Path(newPath.begin(), newPath.end());
		// 			HANDLE hFile = CreateFile(w_Path.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		// 			CloseHandle(hFile);
		// 		}
		// 
		// 		cout << "New Item has Been Succesfully Created!" << endl;
		// 		Sleep(1000);
		// 
		// 		return;
		// 	}
		// 
		// }
	}
	// Ferqli cur input methodlarina el atdim _getch() da duymelerin deyerleri
	// Herflerle ve diger simvollarla ust-uste dushe bilirdi. Amma istifade elediyim
	// yuxaridaki versiyada millisaniyeler icinde 10-larla input alib yerine yetirirdi.
	// Ona gore en yaxsi netice uchun ikisinin qarsighindan istifade eledim.

	char kb = _getch();
	if (kb == 224 || kb == -32) 
	{
		kb = _getch();

		switch (kb) {

		case DEL:
			if (is_directory(s_fileName)) emptyItem(s_fileName);
			remove(s_fileName);
			return;

		case UP:
		case LEFT:
			if (choice == 0) choice = answerCount;
			choice--;
			return;

		case DOWN:
		case RIGHT:
			choice++;
			choice %= answerCount;
			return;
		};
	}

	switch (kb)
	{
	case ESC:
		dir = dir.substr(0, dir.rfind('\\'));
		choice = 0;

		return;
	case 'w':
	case 'W':
	case 'a':
	case 'A':
		if (choice == 0) choice = answerCount;
		choice--;
		return;
	case 's':
	case 'S':
	case 'd':
	case 'D':
		choice++;
		choice %= answerCount;
		return;
	case ENTER:
		keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
		if (is_directory(s_fileName)) { dir = s_fileName; choice = 0; }
		else { ShellExecute(NULL, NULL, wstring(s_fileName.begin(), s_fileName.end()).c_str(), NULL, NULL, SW_SHOW); }
		return;
	case 'R':
	case 'r':
		renameItem();
		return;
	case 'm':
	case 'M':
		if (!(::GetKeyState(VK_SHIFT) & 0x8000)) moveItem();
		return;
	case 'n':
	case 'N':
		newItem();
		return;
	case 'E':
	case 'e':
		if (::GetKeyState(VK_SHIFT) & 0x8000) exit(777);
		return;
	case 'h':
	case 'H':
		if (::GetKeyState(VK_SHIFT) & 0x8000) helpMenu();
		return;
	case 'p':
	case 'P':
		reDirectory();
		return;
	default:
		return;
	}
}