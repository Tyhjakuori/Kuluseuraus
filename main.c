#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sqlite3.h>

#define MAXLEN 100

static int callback(void *NotUsed, int argc, char **argv, char **azColName);
double haetupla(double *summa); 
void searchdb();
void editdb();
sqlite3 *db;

int main() {
	
	int rc;
	double summa;
	char *errmsg = 0;
	char *ptr = malloc(MAXLEN);
	time_t rawtime;
	struct tm *currentTime;
	const int TIME_LENGHT = 20;
	char buffer[TIME_LENGHT];
	sqlite3_stmt *stmt;
	unsigned int current_size = 0;
	
	time(&rawtime);
	currentTime = localtime(&rawtime);
	current_size = MAXLEN;
	printf("Kulutus käyttökohde: ");
	if(ptr != NULL) {
		int c = EOF;
		unsigned int i = 0;
		while ((c = getchar()) != '\n' && c != EOF) {
			ptr[i++] = tolower((char)c);
			if (i == current_size) {
				current_size = i + MAXLEN;
				ptr = (char *) realloc(ptr, current_size);
			}
		}
	}
	haetupla(&summa);
	printf("Käyttökohde: %s | Summa: %f\n", ptr, summa);
	
	rc = sqlite3_open("./testi.db", &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	printf("Opened/created database successfully\n");
	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS expenses(id INTEGER PRIMARY KEY AUTOINCREMENT, date DATE, amount REAL, desc TEXT);", NULL, 0, &errmsg);
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
	sqlite3_close(db);
	free(ptr);
	searchdb();
	editdb();
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

double haetupla(double *summa) {
	
	char s[20];

	printf("Summa: ");
	fgets(s, sizeof(s), stdin);
	sscanf(s, "%lf", *&summa);
	return *summa;
}

void searchdb() {

	unsigned int len_max = 200;
	int rc, size1, temp, answer, sscanf_result;
	char line[4];
	char *errmsg = 0;
	char *naytacolums, *marks[4], *statement = malloc(len_max), *hakusana = malloc(len_max);

	rc = sqlite3_open("./testi.db", &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	printf("|||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("CHOOSE WHICH COLUMN YOU WANT TO USE FOR THE SEARCH\n\n");
	naytacolums = "pragma table_info(expenses);";
	rc = sqlite3_exec(db, naytacolums, callback, 0, &errmsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't select data from the table: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	
	answer = -1;
	while (answer != 0) {
		printf("[1] ID\n");
		printf("[2] Date\n");
		printf("[3] Amount\n");
		printf("[4] Description\n");
		printf("[5] Print the whole database\n");
		fgets (line, sizeof(line), stdin);
		sscanf_result = sscanf(line, "%d", &answer);
		if ((sscanf_result == 0) | (sscanf_result == EOF)) {
			printf("Answer needs to be a number\n");
			answer = -1;
			break;
		}
		switch (answer) {
			case 0:
				break;
			case 1:
				printf("Hakusana: ");
				fgets(hakusana, sizeof(hakusana), stdin);
				hakusana[strcspn(hakusana, "\n")] = 0;
				temp = sizeof(hakusana);
				size1 = temp + 45;
				strcpy(marks, ";");
				strcpy(statement, "SELECT * FROM expenses WHERE id=");
				strncat(statement, hakusana, size1);
				strncat(statement, marks, sizeof(statement+2));
				answer = 0;
				break;
			case 2:
				printf("Hakusana: ");
				fgets(hakusana, sizeof(hakusana), stdin);
				hakusana[strcspn(hakusana, "\n")] = 0;
				temp = sizeof(hakusana);
				size1 = temp + 45;
				strcpy(marks, "%';");
				strcpy(statement, "SELECT * FROM expenses WHERE date LIKE '%");
				strncat(statement, hakusana, size1);
				strncat(statement, marks, sizeof(statement+4));
				answer = 0;
				break;
			case 3:
				printf("Hakusana: ");
				fgets(hakusana, sizeof(hakusana), stdin);
				hakusana[strcspn(hakusana, "\n")] = 0;
				temp = sizeof(hakusana);
				size1 = temp + 45;
				strcpy(marks, "%';");
				strcpy(statement, "SELECT * FROM expenses WHERE amount LIKE '%");
				strncat(statement, hakusana, size1);
				strncat(statement, marks, sizeof(statement+4));
				answer = 0;
				break;
			case 4:
				printf("Hakusana: ");
				fgets(hakusana, sizeof(hakusana), stdin);
				hakusana[strcspn(hakusana, "\n")] = 0;
				temp = sizeof(hakusana);
				size1 = temp + 45;
				strcpy(marks, "%';");
				strcpy(statement, "SELECT * FROM expenses WHERE desc LIKE '%");
				strncat(statement, hakusana, size1);
				strncat(statement, marks, sizeof(statement+4));
				answer = 0;
				break;
			case 5:
				strcpy(statement, "SELECT * FROM expenses;");
				answer = 0;
				break;
			default:
				break;
		}
	}
	printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("SEARCH RESULTS\n\n");
	rc = sqlite3_exec(db, statement, callback, 0, &errmsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't select data from the table: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	free(statement);
	free(hakusana);
}

void editdb() {

	unsigned int max_len = 150;
	int rc, answer, sscanf_result;
	char *errmsg = 0, *placeh[13], *sana = malloc(max_len), *line[5], *desc = malloc(max_len);
	char *statement = malloc(200), *marks[3], *paiva[10], *numero[3], *amount[10];

	rc = sqlite3_open("./testi.db", &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	/* esim sql statement edit: "UPDATE expenses SET amount = 4.99 WHERE id = 4;"*/
	strcpy(statement, "UPDATE expenses SET ");
	strcpy(placeh, " WHERE id = ");
	strcpy(marks, ";");
	answer = -1;
	while (answer != 0) {
		printf("|||||||||||||||||||||||||||||||||||||\n");
		printf("SELECT WHICH FIELD YOU WANT TO EDIT\n");
		printf("[1] Date\n");
		printf("[2] Amount\n");
		printf("[3] Description\n");
		printf("[0] Don't edit anything\n");
		fgets (line, sizeof(line), stdin);
		sscanf_result = sscanf(line, "%d", &answer);
		if ((sscanf_result == 0) | (sscanf_result == EOF)) {
			printf("Answer needs to be a number between 1-3\n");
			answer = -1;
			break;
		}
		switch (answer) {
			case 0:
				exit(0);
			case 1:
				printf("Id of the entry you want to edit: ");
				fgets(numero, sizeof(numero), stdin);
				printf("Numero = %s\n", numero);
				numero[strcspn(numero, "\n")] = 0;
				printf("Example date: \"2021-11-10 10:12:00\"\n");
				printf("New date for this entry: ");
				fgets(sana, 23, stdin);
				strcpy(paiva, "date = ");
				strncat(statement, paiva, 8);
				strncat(statement, sana, strlen(sana));
				strncat(statement, placeh, 13);
				strncat(statement, numero, 2);
				strncat(statement, marks, 2);
				answer = 0;
				break;
			case 2:
				printf("Id of the entry you want to edit: ");
				fgets(numero, sizeof(numero), stdin);
				numero[strcspn(numero, "\n")] = 0;
				printf("Example amount: \"5.99\"\n");
				printf("New Amount for this entry: ");
				fgets(sana, 20, stdin);
				strcpy(amount, "amount = ");
				strncat(statement, amount, 10);
				strncat(statement, sana, strlen(sana));
				strncat(statement, placeh, 13);
				strncat(statement, numero, 2);
				strncat(statement, marks, 2);
				answer = 0;
				break;
			case 3:
				printf("Id of the entry you want to edit: ");
				fgets(numero, sizeof(numero), stdin);
				printf("Numero = %s\n", numero);
				numero[strcspn(numero, "\n")] = 0;
				printf("Example description: \"Electricity bill\"\n");
				printf("New description for this entry: ");
				fgets(sana, max_len, stdin);
				strcpy(desc, "desc = ");
				strncat(statement, desc, 8);
				strncat(statement, sana, strlen(sana));
				strncat(statement, placeh, 13);
				strncat(statement, numero, 2);
				strncat(statement, marks, 2);
				answer = 0;
				break;
			default:
				break;
		}
	}
	printf("Statement was: %s\n", statement);
	rc = sqlite3_exec(db, statement, callback, 0, &errmsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't select data from the table: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	free(statement);
	free(sana);
	free(desc);
	sqlite3_close(db);
}
