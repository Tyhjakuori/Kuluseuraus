#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sqlite3.h>

#define MAXLEN 100

static int callback(void *NotUsed, int argc, char **argv, char **azColName);
sqlite3 *db;

int main() {
	
	struct tm *currentTime;
	unsigned int current_size = 0;
	const int TIME_LENGHT = 20;
	int rc;
	double summa;
	char *ptr = malloc(MAXLEN);
	char *zErrMsg = 0;
	char buffer[TIME_LENGHT];
	char placeh[100];
	time_t rawtime;
	sqlite3_stmt *stmt;
	
	time(&rawtime);
	currentTime = localtime(&rawtime);
	current_size = MAXLEN;
	printf("Kulutus käyttökohde: ");
	if(ptr != NULL) {
		int c = EOF;
		unsigned int i = 0;
		while ((c = getchar()) != '\n' && c != EOF) {
			ptr[i++] = (char)c;
			if (i == current_size) {
				current_size = i + MAXLEN;
				ptr = (char *) realloc(ptr, current_size);
			}
		}
	}
	printf("%s\n", ptr);
	printf("Summa: ");
	fgets(placeh, sizeof(placeh), stdin);
	summa = strtod(placeh, NULL);
	printf("Tarkoitus: %s | Summa: %f\n",ptr, summa);
	
	rc = sqlite3_open("./testi.db", &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	printf("Opened/created database successfully\n");
	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS expenses(id INTEGER PRIMARY KEY AUTOINCREMENT, date DATE, amount REAL, desc TEXT);", NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't create a table: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	printf("Successfully added a table\n");

	if (sqlite3_prepare_v2(db, "INSERT INTO expenses (id, date, amount, desc) VALUES (?1, ?2, ?3, ?4)", -1, &stmt, 0) != SQLITE_OK) {
		printf("Coudn't prepare statement\n");
		return 1;
	}
	strftime(buffer, TIME_LENGHT, "%Y-%m-%d %H:%M:%S", currentTime);	
	if (sqlite3_bind_null(stmt, 1) != SQLITE_OK) {
		printf("Coudn't bind text\n");
		return 1;
	}
	if (sqlite3_bind_text(stmt, 2, buffer, -1, NULL) != SQLITE_OK) {
		printf("Coudn't bind text\n");
		return 1;
	}
	if (sqlite3_bind_double(stmt, 3, summa) != SQLITE_OK) {
		printf("Couldn't bind float(double)\n");
		return 1;
	}
	if (sqlite3_bind_text(stmt, 4, ptr, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
		printf("Couldn't bind pointer\n");
		return 1;
	}
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	printf("Successfully inserted data into the table\n");
	
	rc = sqlite3_exec(db, "SELECT * FROM expenses;", callback, 0, zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't select data from the table: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	sqlite3_close(db);
	return 0;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	int i;
	for(i=0; i<argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}
