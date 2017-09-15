/*
_______________________________________________________________________________

		all2HTML - main file (main.c)


MIT License

Copyright (c) 2017 Akshay Mohan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
_______________________________________________________________________________		*/

#include <conio.h>
#include "..\\include\\main.h"


int main(int argc, char **argv) {

	char sFilePath[MAX_PATH];
	unsigned short int iOption;
	Bool bSubFolders = False;

	unsigned long int tickCounts;
	int pathretVal;
	unsigned int nFiles;

	printf(
		"_______________________________________\n"\
		"\n"\
		"\tFiles to '.html'\n"\
		"_______________________________________\n");

	if(setupSQLiteConnection("all2HTML.db")) {

		printf("ERROR : Could not establish a connection with database!\n");
		return 1;
	}
	if(argc > 1) {

		if(!strcmp(argv[1], "--help")) {

			printf(
				"all2HTML - Command line arguments guide.\n"\
				"Syntax : all2html.exe [mode] [path]\n"\
				"Modes:\n"\
				"-a\t-\tConvert all files and subfolders inside the given path.\n"\
				"-f\t-\tConvert all files only inside the given path.\n"\
				"-l\t-\tList all converted files.\n"\
				"-r\t-\tRollback/Undo all conversions.\n"\
				"--help\t-\tall2HTML - Command line arguments guide.\n");
		}
		else if(!strcmp(argv[1], "-a") || !strcmp(argv[1], "-f")) {

			if(argc >= 3) {

				nFiles		= 0;
				tickCounts 	= GetTickCount();
				pathretVal 	= changePathExtension(argv[2], ".html", (argv[1][1] == 'a') ? True : False, &nFiles);
				if(pathretVal == 1)
					printf("%d files have been converted to .html! (Time taken : %0.2f seconds)\n", nFiles, (float)((float)(GetTickCount() - tickCounts) / 1000.00));
				else if(pathretVal == 0)
					printf("ERROR : Failed converting to .html! Make sure this program is running as administrator or if the path exists.\n");
				else
					printf("ERROR : Failed to allocate more memory!\n");
			} else 
				printf("ERROR : Path not set. Try \"all2html.exe --help\" for help.\n");
		}
		else if(!strcmp(argv[1], "-l"))
			listConvertedData();
		else if(!strcmp(argv[1], "-r")) 
			rollbackFileTypes();
		else
			printf("ERROR : Invalid mode/syntax. Use \"all2HTML.exe --help\" for help.\n");
	} else {

		while(1) {

			nFiles = 0;
			printf(
				"\n1. Convert files from a path to .html.\n"\
				"2. List converted files.\n"\
				"3. Undo all conversions.\n"\
				"4. Exit.\n"\
				"Choose an operation : ");
			scanf("%hd", &iOption);

			switch(iOption) {

				case 1 : {

					printf("Path : ");
					scanf("%s", sFilePath);

					printf("Do you want to include subfolders? (1 = Yes / 0 = No) : ");
					scanf("%hd", &iOption);

					if(iOption)
						bSubFolders = True;

					tickCounts = GetTickCount();
					pathretVal = changePathExtension(sFilePath, ".html", bSubFolders, &nFiles);
					if(pathretVal == 1)
						printf("%d files have been converted to .html! (Time taken : %0.2f seconds)\n", nFiles, (float)((float)(GetTickCount() - tickCounts) / 1000.00));
					else if(pathretVal == 0)
						printf("ERROR : Failed converting to .html! Make sure this program is running as administrator or if the path exists.\n");
					else
						printf("ERROR : Failed to allocate more memory!\n");

					break;
				}
				case 2 : {

					listConvertedData();
					break;
				}
				case 3 : {

					rollbackFileTypes();
					break;
				}
				case 4 : {

					if(!closeSQLiteConnection())
						printf("ERROR : Could not close the connection. Make sure no other processes are affecting all2HTML.db\n");
					else {

						printf("Closing...");
						Sleep(300);
						exit(0);
						break;
					}
				}
				default:
					printf("ERROR : Invalid option chosen!\n");
			}
		}
	}
	getch();
	return 0;
}
