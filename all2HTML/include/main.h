/*
_______________________________________________________________________________

		all2HTML - core file (main.h)
		v1.0.3

MIT License

Copyright (c) 2017-2020 Akshay Mohan

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



#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include "..\\include\\sqlite3\\sqlite3.h"

enum Bool {

	False,
	True
};

typedef enum Bool Bool;

static sqlite3 *gSQLHandle;
static WORD gwCurrCol;

static WORD get_consoletext_color();
static int set_consoletext_color(WORD wCol);
static int rollback_callback(void *NotUsed, int argc, char **argv, char **sColName);


int setupSQLiteConnection(char *sDBName); //0 = success | 1 = fail
int closeSQLiteConnection(); //1 = success | 0 = fail
int changePathExtension(char *sDPath, char *sExtension, Bool bSubFolders, unsigned int *nFiles); //1 = success | (0 = path/privilege issue, -1 = heap issue)
int listConvertedData(); //returns total number of files converted. -1 if fails.
void rollbackFileTypes(); 

static WORD get_consoletext_color() {

	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbiInfo);

	return csbiInfo.wAttributes;
}

static int set_consoletext_color(WORD wCol) {

	return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wCol);
}


int setupSQLiteConnection(char *sDBName) {

	char *szErrorMsg = 0;
	const char *sQuery;
	int iretVal;

	iretVal = sqlite3_open(sDBName, &gSQLHandle);
	if(iretVal)
		return sqlite3_errcode(gSQLHandle);

	sQuery = 
		"CREATE TABLE IF NOT EXISTS `filePathData`("\
		"`filePathID` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"\
		"`prevFilePath` TEXT NOT NULL,"\
		"`currFilePath` TEXT NOT NULL,"\
		"UNIQUE(`prevFilePath`),"\
		"UNIQUE(`currFilePath`));"
	;
	
	iretVal = sqlite3_exec(gSQLHandle, sQuery, 0, 0, &szErrorMsg);
	if(iretVal != SQLITE_OK) {

		printf("SQL ERROR : %s\n", szErrorMsg);
		sqlite3_free(szErrorMsg);
		return 1;
	}
	gwCurrCol = get_consoletext_color();
	return 0;
}

int closeSQLiteConnection() {

	return (sqlite3_close(gSQLHandle) == SQLITE_OK) ? 1 : 0;
}

int changePathExtension(char *sDPath, char *sExtension, Bool bSubFolders, unsigned int *nFiles) {

	HANDLE hHandle;
	WIN32_FIND_DATA fdFileData;

	Bool bExtension;

	char
		*sPrevPath,
		*sCurrPath,
		*sQuery,
		*szErrorMsg = 0
	;
	unsigned int
				iDPathLen,
				idestPathLen
	;
	iDPathLen 	=	strlen(sDPath);
	sPrevPath 	=	(char *)malloc(sizeof(char) * (iDPathLen + 5));

	if(sPrevPath == NULL)
		return -1;

	sprintf(sPrevPath, "%s\\*.*", sDPath);
	hHandle = FindFirstFile(sPrevPath, &fdFileData);
	free(sPrevPath);

	if(hHandle == INVALID_HANDLE_VALUE)
		return 0;

	while(FindNextFile(hHandle, &fdFileData)) {

		if(!strcmp(fdFileData.cFileName, "..") || !strcmp(fdFileData.cFileName, ".")) 
			continue;

		if(fdFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

			if(bSubFolders) {

				sPrevPath = (char *)malloc(sizeof(char) * (iDPathLen + strlen(fdFileData.cFileName) + 2));
				if(sPrevPath == NULL)
					return -1;

				sprintf(sPrevPath, "%s\\%s", sDPath, fdFileData.cFileName);
				changePathExtension(sPrevPath, sExtension, True, nFiles);

				free(sPrevPath);
			}
		} else {

			sPrevPath = (char *)malloc(sizeof(char) * (iDPathLen + strlen(fdFileData.cFileName) + 2));
			if(sPrevPath == NULL)
				return -1;

			sprintf(sPrevPath, "%s\\%s", sDPath, fdFileData.cFileName);
			idestPathLen = strlen(sPrevPath);

			sCurrPath = (char *)malloc(sizeof(char) * (idestPathLen + strlen(sExtension) + 1));
			if(sCurrPath == NULL) {

				free(sPrevPath);
				return -1;
			}
			strcpy(sCurrPath, sPrevPath);
			bExtension = False;
			idestPathLen--;
			while(sCurrPath[idestPathLen] != '\\') {

				if(sCurrPath[idestPathLen] == '.') {

					bExtension = True;
					break;
				}
				idestPathLen--;
			}
			if(bExtension)
				sCurrPath[idestPathLen] = '\0';

			strcat(sCurrPath, sExtension);

			sQuery = sqlite3_mprintf(
				"INSERT INTO filePathData "\
				"(prevFilePath, currFilePath) "\
				"VALUES ('%q', '%q');", sPrevPath, sCurrPath)
			;
			if(sQuery == NULL) {

				free(sCurrPath);
				free(sPrevPath);

				return -1;
			}
			if(sqlite3_exec(gSQLHandle, sQuery, 0, 0, &szErrorMsg) != SQLITE_OK) {

				printf("ERROR : Could not change the extension of '%s'! (Reason : Likely to be a duplicate entry)\n", sPrevPath);

				free(sCurrPath);
				free(sPrevPath);

				sqlite3_free(sQuery);
				//printf("SQL ERROR : %s\n", szErrorMsg); //Uncomment to view SQL error.
				sqlite3_free(szErrorMsg);

				continue;
			}
			set_consoletext_color(FOREGROUND_GREEN);
			printf("%s ", sPrevPath);

			set_consoletext_color(gwCurrCol);
			printf("-> ");

			set_consoletext_color(FOREGROUND_RED);
			printf("%s\n", sCurrPath);

			set_consoletext_color(gwCurrCol);
			rename(sPrevPath, sCurrPath);
			(*nFiles)++;

			free(sCurrPath);
			free(sPrevPath);
			sqlite3_free(sQuery);

		}
	}
	FindClose(hHandle);
	return 1;
}

int listConvertedData() {

	sqlite3_stmt *stmt;
	const char *sQuery = "SELECT prevFilePath, currFilePath FROM filePathData;";
	int
		retVal,
		rowCounts = 0
	;
	retVal = sqlite3_prepare_v2(gSQLHandle, sQuery, -1, &stmt, NULL);
	if(retVal != SQLITE_OK) {

		printf("SQL ERROR : %s\n", sqlite3_errmsg(gSQLHandle));
		sqlite3_finalize(stmt);
		return -1;
	}
	printf("Listing converted data (Old | Current):\n");
	while((retVal = sqlite3_step(stmt)) == SQLITE_ROW) {

		set_consoletext_color(FOREGROUND_GREEN);
		printf("%s ", sqlite3_column_text(stmt, 0));

		set_consoletext_color(gwCurrCol);
		printf("| ");

		set_consoletext_color(FOREGROUND_RED);
		printf("%s\n", sqlite3_column_text(stmt, 1));

		set_consoletext_color(gwCurrCol);
		rowCounts++;
	}
	if(retVal != SQLITE_DONE)
		printf("SQL ERROR : %s\n", sqlite3_errmsg(gSQLHandle));

	printf("Total number of files : %d\n", rowCounts);
	sqlite3_finalize(stmt);
	return rowCounts;
}

void rollbackFileTypes() {

	const char *sQuery = "SELECT prevFilePath, currFilePath FROM filePathData;";
	char *szErrorMsg = 0;
	if(sqlite3_exec(gSQLHandle, sQuery, rollback_callback, 0, &szErrorMsg) != SQLITE_OK) {

		printf("SQL ERROR : %s\n", szErrorMsg);
		sqlite3_free(szErrorMsg);
	}
}

static int rollback_callback(void *NotUsed, int argc, char **argv, char **sColName) {

	char *sQuery, *szErrorMsg;
	if(argc != 2) {

		printf("ERROR : Number of columns do not match as expected!\n");
		return 1;
	}
	if(!argv[0] || !argv[1]) {

		printf("ERROR : Damaged data!\n");
		return 1;
	}
	rename(argv[1], argv[0]);
	sQuery = sqlite3_mprintf("DELETE FROM filePathData WHERE prevFilePath = '%q' AND currFilePath = '%q';", argv[0], argv[1]);

	if(sqlite3_exec(gSQLHandle, sQuery, 0, 0, &szErrorMsg) != SQLITE_OK) {

		printf("SQL ERROR : %s\n", szErrorMsg);
		sqlite3_free(szErrorMsg);
		sqlite3_free(sQuery);
		return 1;
	}
	sqlite3_free(sQuery);
	set_consoletext_color(FOREGROUND_RED);
	printf("%s ", argv[1]);

	set_consoletext_color(gwCurrCol);
	printf("-> ");

	set_consoletext_color(FOREGROUND_GREEN);
	printf("%s\n", argv[0]);

	set_consoletext_color(gwCurrCol);
	return 0;
}
