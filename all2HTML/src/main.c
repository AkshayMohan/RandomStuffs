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


int main() {

	char sFilePath[MAX_PATH];
	unsigned short int iOption;
	Bool bSubFolders = False;

	printf(
		"_______________________________________\n"\
		"\n"\
		"\tFiles to '.html'\n"\
		"_______________________________________\n");

	if(setupSQLiteConnection("all2HTML.db")) {

		printf("ERROR : Could not establish a connection with database!\n");
		return 1;
	}
	while(1) {

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

				if(convertFileTypeToHTML(sFilePath, bSubFolders))
					printf("Files have been converted to .html!\n");
				else
					printf("ERROR : Failed converting to .html! Make sure this program is running as administrator or if the path exists.\n");

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
	return 0;
}