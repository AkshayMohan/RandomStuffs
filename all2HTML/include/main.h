/*
_______________________________________________________________________________

		all2HTML - core file (main.h)

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
int convertFileTypeToHTML(char *sDPath, Bool bSubFolders); //1 = success | 0 = fail
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
		"UNIQUE(`prevFilePath`, `currFilePath`));"
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


int convertFileTypeToHTML(char *sDPath, Bool bSubFolders) {

	HANDLE hHandle;
	WIN32_FIND_DATA fdFileData;

	char
		*sFilePath,
		*sCurrPath,
		*tempStr,
		*sQuery,
		*szErrorMsg = 0
	;
	int sPathLen;

	sCurrPath 	= (char *)malloc(sizeof(char) * strlen(sDPath));
	sFilePath 	= (char *)malloc(sizeof(char) * (strlen(sDPath) + 5));
	tempStr 	= (char *)malloc(sizeof(char));

	strcpy(sCurrPath, sDPath);
	sprintf(sFilePath, "%s\\*.*", sDPath);

	if((hHandle = FindFirstFile(sFilePath, &fdFileData)) == INVALID_HANDLE_VALUE)
		return 0;

	while(FindNextFile(hHandle, &fdFileData)) {

		if(!strcmp(fdFileData.cFileName, "..") || !strcmp(fdFileData.cFileName, "."))
			continue;

		sFilePath = (char *)realloc(sFilePath, sizeof(char) * (strlen(sFilePath) + strlen(fdFileData.cFileName)));
		sprintf(sFilePath, "%s\\%s", sCurrPath, fdFileData.cFileName);

		if(fdFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

			tempStr = (char *)realloc(tempStr, sizeof(char) * (strlen(sCurrPath) + strlen(fdFileData.cFileName)));
			sprintf(tempStr, "%s\\%s", sCurrPath, fdFileData.cFileName);

			if(bSubFolders) 
				convertFileTypeToHTML(tempStr, True);
		} else {

			tempStr = (char *)realloc(tempStr, sizeof(char) * (strlen(sFilePath) + 6));
			strcpy(tempStr, sFilePath);

			sPathLen = strlen(tempStr) - 1;
			while(tempStr[sPathLen] != '.')
				sPathLen--;

			tempStr[sPathLen] = '\0';
			strcat(tempStr, ".html");

			sQuery = sqlite3_mprintf(
				"INSERT INTO filePathData "\
				"(prevFilePath, currFilePath) "\
				"VALUES ('%q', '%q');", sFilePath, tempStr)
			;

			if(sqlite3_exec(gSQLHandle, sQuery, 0, 0, &szErrorMsg) != SQLITE_OK) {

				//Could also be if duplicate entries are there.
				//printf("INSERTION ERROR ON SQL : %s\n", szErrorMsg); 
				//szErrorMsg contains the SQL error occurred.
				printf("ERROR : Could not change '%s' file. (A duplicate entry could probably exist!)\n", fdFileData.cFileName);
				sqlite3_free(szErrorMsg);
				sqlite3_free(sQuery);
				continue;
			}

			set_consoletext_color(FOREGROUND_GREEN);
			printf("%s ", sFilePath);

			set_consoletext_color(gwCurrCol);
			printf("-> ");

			set_consoletext_color(FOREGROUND_RED);
			printf("%s\n", tempStr);

			set_consoletext_color(gwCurrCol);

			sqlite3_free(sQuery);
			rename(sFilePath, tempStr);
		}
	}
	free(sFilePath);
	free(sCurrPath);
	free(tempStr);
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
		return 1;
	}

	set_consoletext_color(FOREGROUND_RED);
	printf("%s ", argv[1]);

	set_consoletext_color(gwCurrCol);
	printf("-> ");

	set_consoletext_color(FOREGROUND_GREEN);
	printf("%s\n", argv[0]);

	set_consoletext_color(gwCurrCol);
	return 0;
}