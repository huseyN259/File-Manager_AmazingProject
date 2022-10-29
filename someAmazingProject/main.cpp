#include<iostream>
#include<windows.h>
#include<conio.h>
#include<algorithm>
#include<filesystem>
#include<fstream>
#include<map>

using namespace std;
using namespace std::filesystem;

#pragma warning( disable : 4996 )

string dir = getenv("USERPROFILE");
pair<int, string> selectedFile;
string& s_fileName = selectedFile.second;

#include "resources.h"

int main()
{
	dir += "\\Desktop";
	unsigned short choice = 0;

	while (true)
	{
			clear_screen();

			unsigned short i = 0;
			directory_iterator pathIterator;
			
			try 
			{ 
				pathIterator = directory_iterator(dir); 
			}
			catch (const std::exception&)
			{ 
				cout << "Specifed Directory does not EXIST: " << dir << endl; reDirectory(); 
				continue;
			}
			
			for (const auto& file : pathIterator)
			{
				string currentPath = file.path().string();
				char prefix = ' ';
				if (i == choice) { selectedFile.first = i; s_fileName = currentPath; mySetColor(LIGHTGREEN, GREY); prefix = char(254); }
				cout << ' ' << prefix << ' ' << i++ << " - " << currentPath << endl;
				mySetColor(LIGHTGREEN, BLACK);
			}

			if (!i) cout << setw(60) << "FOLDER IS EMPTY" << endl;

		try 
		{ 
			manageConsoleInput(choice, i); 
		}
		catch (const std::exception& ex) 
		{ 
			cout << ex.what() << endl; 
			cin.get(); 
		}
	}

    return EXIT_SUCCESS;
}