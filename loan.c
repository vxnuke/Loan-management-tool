/* Copyright (c) 2006-2010, Timothy Williams <tim@securesec.com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Redis nor the names of its contributors may be used
* to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/
#include<stdio.h>
#include<stdlib.h>
#include<mysql.h>
#include<string.h>
#include<time.h>
int main(int argc, char **argv) {
// Database Connection Details
MYSQL *conn;
MYSQL_ROW row;
MYSQL_FIELD *field;
MYSQL_RES *result;
char *server = "localhost";
char *user = "root";
char *password = "password";
char *database = "loan";
// Connect to database
conn = mysql_init(NULL);
if (!mysql_real_connect(conn, server,
      user, password, database, 0, NULL, 0)) {
   fprintf(stderr, "%s\n", mysql_error(conn));
   exit(1);
}
// End Database
//Global
   char query[200];
//End Global
    if (!strcmp(argv[1], "new")) {
       char *agent = argv[2];
       sprintf(query, "INSERT INTO users (agent, profit) VALUES ('%s', 0);", agent);
       if (mysql_query(conn, query)) {
          fprintf(stderr, "%s", mysql_error(conn));
          exit(1);
       }
       printf("New Agent Sucessfully Added\n");
    }
    if (!strcmp(argv[1], "add")) {
       char *agent = argv[2];
       double amount = atof(argv[3]);
       double interest = atof(argv[4]);
       double profit;
       sprintf(query, "select profit from users where agent = ('%s');", agent);
       if (mysql_query(conn, query)) {
          fprintf(stderr, "%s", mysql_error(conn));
          exit(1);
       }
       int num_fields;
       int i;
       result = mysql_store_result(conn);
       num_fields = mysql_num_fields(result);
       MYSQL_ROW row1;
       row1 = mysql_fetch_row(result);
       double credit = interest * amount / 100;
       double remote_credit = atof(row1[0] ? row1[0] : "NULL");
       double total_profit = (credit + remote_credit);
       printf("Total Profit %f\n", total_profit);
       double credit_allowance = (75 * total_profit / 100);
       printf("Credit Allowance %f\n", credit_allowance);
       query[0] = '\0';
       sprintf(query, "UPDATE users SET profit='%f' WHERE agent='%s';", total_profit, agent);
       if (mysql_query(conn, query)) {
          fprintf(stderr, "%s", mysql_error(conn));
          exit(1);
       }
       query[0] = '\0';
       // Get time
       time_t raw;
       time(&raw);
       struct tm *time_ptr;
       time_ptr = localtime(&raw);
       char date[11];
       strftime(date, 11, "%d/%m/%Y", time_ptr);
       // End time
       sprintf(query, "UPDATE users SET date='%s' WHERE agent='%s';", date, agent);
       if (mysql_query(conn, query)) {
          fprintf(stderr, "%s", mysql_error(conn));
          exit(1);
       }
       printf("Database Successfully Updated\n");
       mysql_free_result(result);
    }
    if (!strcmp(argv[1], "show") && strcmp(argv[2], "all")) {
       MYSQL_ROW row2;
       char *agent = argv[2];
       int num_fields;
       int i;
       sprintf(query, "SELECT agent, profit, date, term, interest, status FROM users WHERE agent='%s';", agent);
       if (mysql_query(conn, query)) {
          fprintf(stderr, "%s", mysql_error(conn));
          exit(1);
       }
       result = mysql_store_result(conn);
       num_fields = mysql_num_fields(result);
       while ((row = mysql_fetch_row(result)))
       {
           for(i = 0; i < num_fields; i++)
           {
               if (i == 0) {
                  while(field = mysql_fetch_field(result)) {
                      printf("%s", field->name);
                      // Align
                      char *row_num;
                      sprintf(row_num, "%d", i);
                      printf(":");
                      if(!strcmp(row_num, "0")) {
                         printf("   ");
                      }
                      if(!strcmp(row_num, "1")) {
                         printf("  ");
                      }
                      if(!strcmp(row_num, "2")) {
                         printf("    ");
                      }
                      if(!strcmp(row_num, "3")) {
                         printf("    ");
                      }
                      if(!strcmp(row_num, "5")) {
                         printf("  ");
                      }
                     // if(!strcmp(row_num, "
                      // END
                      printf(" %s\n", row[i] ? row[i] : "NULL");
                      i++;
                  }
               printf("credit:   ");
               }
           }
       }
       query[0] = '\0';
       sprintf(query, "SELECT profit FROM users WHERE agent='%s';", agent);
       if (mysql_query(conn, query)) {
          fprintf(stderr, "%s", mysql_error(conn));
          exit(1);
       }
       result = mysql_store_result(conn);
       row2 = mysql_fetch_row(result);
       double profit = atof(row2[0] ? row2[0] : "NULL");
       // CREDIT RATIO FOR SKIDDIES :)
       double credit = 75 * profit / 100;
       printf("%f \n", credit);
       mysql_free_result(result);
    }
    if (!strcmp(argv[1], "del")) {
       char *agent = argv[2];
       printf("Deleting....%s\n", agent);
       sprintf(query, "DELETE FROM users WHERE agent='%s';", agent);
       if (mysql_query(conn, query)) {
          fprintf(stderr, "%s", mysql_error(conn));
          exit(1);
       }
       printf("User: %s has now been deleted from database\n", agent);
    }
    if (!strcmp(argv[1], "set") && !strcmp(argv[2], "term")) {
       char *agent = argv[3];
       char *term = argv[4];
       sprintf(query, "UPDATE users SET term='%s' WHERE agent='%s';", term, agent);
       if (mysql_query(conn, query)) {
          fprintf(stderr, "%s", mysql_error(conn));
          exit(1);
       }
       printf("Database Successfully Updated\n");
    }
    if (!strcmp(argv[1], "set") && !strcmp(argv[2], "profit")) {
       char *agent = argv[3];
       char *profit = argv[4];
       sprintf(query, "UPDATE users SET profit='%s' WHERE agent='%s';", profit, agent);
       if (mysql_query(conn, query)) {
          fprintf(stderr, "%s", mysql_error(conn));
          exit(1);
       }
       printf("Database Successfully Updated\n");
    }
    if (!strcmp(argv[1], "set") && !strcmp(argv[2], "interest")) {
       char *agent = argv[3];
       char *interest = argv[4];
       sprintf(query, "UPDATE users SET interest='%s' WHERE agent='%s';", interest, agent);
       if (mysql_query(conn, query)) {
          fprintf(stderr, "%s", mysql_error(conn));
          exit(1);
       }
       printf("Database Successfully Updated\n");
    }
    if (!strcmp(argv[1], "set") && !strcmp(argv[2], "status")) {
       char *agent = argv[3];
       char *status = argv[4];
       sprintf(query, "UPDATE users SET status='%s' WHERE agent='%s';", status, agent);
       if (mysql_query(conn, query)) {
          fprintf(stderr, "%s", mysql_error(conn));
          exit(1);
       }
       printf("Database Successfully Updated\n");
    }

    if (!strcmp(argv[1], "show") && !strcmp(argv[2], "all")) {
       //fuck im lazy why didn't i think of using this before.....
       char command[200];
       sprintf(command, "#/bin/bash \nmysql -u %s -p%s %s -e 'select * from users;'", user, password, database);
       system(command);
    }
    if (!strcmp(argv[1], "--help")) {
       #define ANSI_COLOR_RED     "\x1b[31m"
       #define ANSI_COLOR_RESET   "\x1b[0m"
       printf(ANSI_COLOR_RED "USAGE:\n" ANSI_COLOR_RESET);
       printf("./loan new <agent name>\n");
       printf("./loan add <agent name> <amount> <interest>\n");
       printf("./loan del <agent name>\n");
       printf("./loan show <agent name>\n");
       printf("./loan show all\n");
       printf("./loan set term <agent name> <term>\n");
       printf("./loan set profit <agent name> <profit>\n");
       printf("./loan set interest <agent name> <profit>\n");
       printf("Note: When you use add command the current date\n");
       printf("Will be added to database. The date feature is\n");
       printf("shown under the show command\n");
       printf("I AM AWESOME!!! Just saying...\n");
    mysql_close(conn);
    }
}
