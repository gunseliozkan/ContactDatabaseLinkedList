#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NAMELEN 50
#define MAXRECORD 500

typedef struct record_s {
    int id;
    char number[NAMELEN];
    struct record_s *next;
} Record;

typedef struct person_s {
    int id;
    char name[NAMELEN];
    double expenditure;
    Record *numbers;
} Person;

typedef struct people_s {
    Person data[MAXRECORD];
    int size;
} People;


void writePeople(People people, char* filename) {
	FILE * fp;
    Record* currentRecord;
	int i, numbers;

	fp = fopen(filename, "wb");
	if(fp==NULL)
		return;

    for(i=0; i<people.size; i++)
    {

        fwrite(&people.data[i], __builtin_offsetof(Person, numbers), 1, fp);
        /*fwrite(&people.data[i], sizeof(Person)-sizeof(Record *), 1, fp)*/
        numbers = 0;
        currentRecord = people.data[i].numbers;
        do {
            ++numbers;
        }while((currentRecord = currentRecord->next));

        fwrite(&numbers, sizeof(int), 1, fp); /*add number of telephone numbers too (PDF told me so..)*/

        currentRecord = people.data[i].numbers;
        do
        {
            fwrite(currentRecord->number, 11, 1, fp);
        }while((currentRecord = currentRecord->next));
    }

	fclose(fp);
}

void readPeople(People *people, char* filename) {
	FILE *fp;
	int numberCount = 0;
	int i = 0;
    people->size = 0;

	fp=fopen(filename, "rb");
	if(fp==NULL)
		return;

	while(fread(&people->data[people->size], __builtin_offsetof(Person, numbers), 1, fp) != 0)
	{
        Record* currentRecord = malloc(sizeof(Record));
        people->data[people->size].numbers = currentRecord; /*create my linked list*/

        fread(&numberCount, sizeof(int), 1, fp); /*i kept number of phone numbers in file*/

        for(i = 0; i < numberCount; ++i)
        {
            fread(currentRecord->number, 11, 1, fp); /*11 because every number has 11 digit*/
            currentRecord->number[11] = 0; /*don't forget NULL at the end of the string*/

            if(i != numberCount - 1) /*if i am not at the last phone number connect it to another number*/
            {
                currentRecord->next = malloc(sizeof(Record));
                currentRecord = currentRecord->next;
            }
        }

        currentRecord->next = NULL; /*i am on the last number so the next pointer will be NULL*/
        people->size++;
	}


	fclose(fp);

}

void imputation(People *people) {
    /*this function doesn't need so many comment lines, it is clear in the PDF file*/
    int i;
    double totalDebt = 0; /*total of KNOWN expenditure*/
    double totalNumber = 0, numberCount = 0;
    Record* currentRecord;

    for(i = 0; i < people->size; ++i)
    {
        if(people->data[i].expenditure != -1)
        {
            totalDebt += people->data[i].expenditure;

            currentRecord = people->data[i].numbers;
            do
            {
                ++totalNumber;
            } while((currentRecord = currentRecord->next));
        }
    }

    for(i = 0; i < people->size; ++i)
    {
        if(people->data[i].expenditure == -1)
        {
            numberCount = 0;
            currentRecord = people->data[i].numbers;
            do
            {
                ++numberCount;
            } while((currentRecord = currentRecord->next));
            /*expanditure was unknown so i found it*/
            /*use the equotion provided in PDF file*/
            people->data[i].expenditure = totalDebt / totalNumber * numberCount;
        }
    }
}

void read(char* filename, People *people) {
    FILE* fp;
    char surname[NAMELEN];
    int ret; /*return of fscanf*/

    fp=fopen(filename, "rb");
    if(fp==NULL)
        return;


    while(1)
    {
        Person* currentPeople = &people->data[people->size];
        Record* currentRecord = malloc(sizeof(Record));

        currentPeople->numbers = currentRecord;

        ret = fscanf(fp, "%d %s %s %lf", &currentPeople->id, currentPeople->name, surname, &currentPeople->expenditure);
        /*read everything and place it in my struct til phone numbers*/
        if(ret == EOF) /*end of file, break the while loop*/
            break;

        strcat(currentPeople->name, " "); /*classic strcat because fscanf format string with whitesapces*/
        strcat(currentPeople->name, surname);

        read_no:
            fscanf(fp, "%s", currentRecord->number); /*there are numbers left in line*/

            if(fgetc(fp) == ' ') /*there is another phone number(it is seperated with whitespace so i check if there is a whitespace)*/
            {
                currentRecord->next = malloc(sizeof(Record)); /*add it to my linked list*/
                currentRecord = currentRecord->next;
                goto read_no; /*do the cycle again*/
            }

        currentRecord->next = NULL; /*I am done with my one person and his/her numbers, finish my linked list*/
        ++people->size; /*of course, increase the size*/
    }
}

void print(People people) {
    int i,found = 0;
    Record *rec;
    /* header */
    printf("%-5s %-30s %-20s %-20s\n", "ID","NAME","EXPENDITURE","NUMBER(s)");
    /* line */
    for (i = 0; i < 78; ++i)
        printf("-");
    printf("\n");

    for (i = 0; i < people.size; ++i) {
        found = 0;
        printf("%-5d %-30s %-20.4f", people.data[i].id, people.data[i].name, people.data[i].expenditure);
        rec = people.data[i].numbers;
        while(rec) {
            if(found)
                printf("%57s", "");
            else
                found = 1;
            printf("%-20s\n", rec->number);
            rec = rec->next;
        }
        printf("\n");
    }
}

int isPeopleEq(People ppl1, People ppl2) {
    Record *rec1,*rec2;
    int i,found = 0;
    int p1size = 0, p2size = 0;



    if(ppl1.size != ppl2.size)
        return 0;
    for (i = 0; i < ppl1.size; ++i) {
        if(strcmp(ppl1.data[i].name,ppl2.data[i].name))
            return 0;
        if(ppl1.data[i].id != ppl2.data[i].id)
            return 0;

        p1size = p2size = 0;
        rec1 = ppl1.data[i].numbers;
        while(rec1) {
            ++p1size;
            rec1 = rec1->next;
        }

        rec2 = ppl2.data[i].numbers;
        while(rec2) {
            ++p2size;
            rec2 = rec2->next;
        }

        if(p1size != p2size) {
            return 0;
        }

        rec1 = ppl1.data[i].numbers;
        while(rec1) {
            rec2 = ppl2.data[i].numbers;
            found = 0;
            while(!found && rec2) {
                if(strcmp(rec1->number,rec2->number) == 0) {
                    found = 1;
                    break;
                }
                rec2 = rec2->next;
            }
            if(!found) {
                return 0;
            }
            rec1 = rec1->next;
        }
    }
    return 1;
}

int main(int argc, char** argv) {
    People people1,people2;
    people1.size = 0;
    read(argv[1],&people1);
    print(people1);
    writePeople(people1,"people.bin");
    readPeople(&people2,"people.bin");
    print(people2);
    printf("%s\n", isPeopleEq(people1,people2) ? "PEOPLE ARE SAME" : "PEOPLE ARE DIFFERENT!");
    printf("Making imputation\n");
    imputation(&people1);
    print(people1);
    return 0;
}
