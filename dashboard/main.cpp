// test dashboard
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql.h"

void drop_machine_table(int num_of_db, MYSQL *conn);

void create_machine_table(int num_of_db, MYSQL *conn);

void insert_value_into_machine_table(int mark, int value, MYSQL *conn);

void create_variables(int num_of_db, MYSQL *conn);


int main(int argc, char* argv[]){
        MYSQL *conn;
        conn = mysql_init(NULL);
        if (conn == NULL){
                printf("mysql_init failed!\n");
                return EXIT_FAILURE;
        }

        conn = mysql_real_connect(conn, "127.0.0.1", "root", "mysql", "grafana",
        0, NULL, 0);

        // create three machines
        // three db
        if(conn){

                printf("Connection success!\n");

                if (mysql_query(conn, "USE grafana"))
                {
                        fprintf(stderr, "%s\n", mysql_error(conn));
                        mysql_close(conn);
                        exit(1);
                }

                printf("stage one\n");
                char drop_table[100];
                strcpy(drop_table, "DROP TABLE IF EXISTS random_data");
                if (mysql_query(conn, drop_table))
                {
                	    printf("cannot drop table\n");
                        fprintf(stderr, "%s\n", mysql_error(conn));
                        mysql_close(conn);
                        exit(1);
                }


                printf("stage two\n");
                char create_table[200];
                strcpy(create_table, "CREATE TABLE random_data(page_in INT, machine_num INT, used INT, not_used INT, date TIMESTAMP, other INT)");
                if (mysql_query(conn, create_table))
                {
                	printf("cannot create table\n");
                        fprintf(stderr, "%s\n", mysql_error(conn));
                        mysql_close(conn);
                        exit(1);
                }

                //create three machine table
                int num_of_db = 3;
                drop_machine_table(num_of_db, conn);
                create_machine_table(num_of_db, conn);  
                create_variables(num_of_db, conn);            

                while(1){
                        //fp = fopen("memory_usage.txt", "r");
                        int value1 = rand()%100;
                        int num = rand()%5;
                        int value3 = rand()%100;
                        int value4 = rand()%100;
                        int other = 100-value3-value4;
                        //fscanf(fp, "%d%d%f", &swap, &virt, &phy);
                        printf("To insert: page_in %d, num_machine %d, used %d, not_used %d\n",value1, (int)num, value3, value4);
                        //fclose(fp);
                        printf("stage three\n");
                        char str[200];
                        sprintf(str,
                        "INSERT INTO random_data (date, page_in, machine_num, used, not_used, other) VALUES ( NOW(), %d, %d, %d, %d, %d)",
                        (int) value1, num, value3, value4, other);
                        int res = mysql_query(conn, str);
                        if (!res){
                        		printf("stage four\n");
                                printf("Inserted %lu rows \n", (unsigned long)mysql_affected_rows(conn));
                        }
                        else{
                        		printf("stage five error\n");
                                fprintf(stderr, "Insert error %d: %s\n", mysql_errno(conn),
                                mysql_error(conn));
                        }
                        printf("finish one level\n");
                        for (int i = 0; i < num_of_db; i++){
                        	int value = rand()%100;
                        	insert_value_into_machine_table(i, value, conn);
                        }
                        sleep(5);
                }

        }
        else{
                printf("Connection failed!\n");
        }

        mysql_close(conn);
        return 0;
}


void create_variables(int num_of_db, MYSQL *conn)
{
    char drop_table[100];
    strcpy(drop_table, "DROP TABLE IF EXISTS variable_table");   
    mysql_query(conn, drop_table);
    char create_table[100];
    strcpy(create_table, "CREATE TABLE variable_table (variable_name VARCHAR(50) NOT NULL, number INT)");
    mysql_query(conn, create_table);
    for (int i = 0; i < num_of_db; i++)
    {
        char insert[100];
        sprintf(insert, "INSERT INTO variable_table (variable_name, number) VALUE ('machine_%d', %d)", i, i);
        printf("insert message: %s\n",insert);
        int res = mysql_query(conn, insert);
        if (!res){
            printf("stage four\n");
            printf("Inserted %lu rows \n", (unsigned long)mysql_affected_rows(conn));
        }
        else{
            printf("error\n");
        }
    }
}


void drop_machine_table(int num_of_db, MYSQL *conn)
{
    for (int i=0; i < num_of_db; i++){
        char drop_machine[100];
        sprintf(drop_machine, "DROP TABLE IF EXISTS machine_%d", i);
        if (mysql_query(conn, drop_machine))
        {
            printf("cannot drop machine table\n");
            fprintf(stderr, "%s\n", mysql_error(conn));
            mysql_close(conn);
            exit(1);
        }
    }
}


void create_machine_table(int num_of_db, MYSQL *conn)
{
	for (int i=0; i<num_of_db; i++){
		char create_machine[200];
		sprintf(create_machine, "CREATE TABLE machine_%d (page_in INT, date TIMESTAMP)", i);
        if (mysql_query(conn, create_machine))
        {
            printf("cannot create machine table\n");
            fprintf(stderr, "%s\n", mysql_error(conn));
            mysql_close(conn);
            exit(1);
        }
	}
}


void insert_value_into_machine_table(int mark, int value, MYSQL *conn)
{
	printf("to insert page_in %d into TABLE machine_%d", value, mark);
	char insert[200];
	sprintf(insert, "INSERT INTO machine_%d (page_in, date) VALUES(%d, NOW())", mark, value);
	int res = mysql_query(conn, insert);
	if (!res){
        printf("Inserted %lu rows \n", (unsigned long)mysql_affected_rows(conn));
    }
    else{
        fprintf(stderr, "Insert error %d: %s\n", mysql_errno(conn),
        mysql_error(conn));
    }
}