// Smart Parking Management System - Group 4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FLOORS 3
#define SLOTS 8
#define MAX_RECORDS 50

int parking[FLOORS][SLOTS];   // 0 = empty, 1 = filled
char vehicleNo[FLOORS][SLOTS][15];
int vehicleType[FLOORS][SLOTS];   // 1 = bike, 2 = car
int entryTime[FLOORS][SLOTS];

char recVehicle[MAX_RECORDS][15];
int recFloor[MAX_RECORDS], recSlot[MAX_RECORDS];
int recIn[MAX_RECORDS], recOut[MAX_RECORDS], recCharge[MAX_RECORDS];
int recCount = 0;

char *typeName[3] = {"", "Bike", "Car"};

void readLine(char line[], int size);
int readInt(int min, int max);
void readVehicleNo(char vno[]);
void displayParking(int (*p)[SLOTS]);
int allocateNearestSlot(int (*p)[SLOTS], int *floor, int *slot);
int findVehicle(char vno[], int *floor, int *slot);
void vehicleEntry();
void vehicleExit();
void emptySlot(int **slotPtr);
int calculateCharges(int minutes, int type);
int countOccupied(int *row);
void floorWithMaxOccupancy();
int countFreeFrom(int *row, int start);
void findConsecutiveSlots(int (*p)[SLOTS]);
void searchVehicle();
void showRecords();
void printType(char **names, int type);
void updateStats(int action, int amount);

int main()
{
    int choice;

    printf("\n===== SMART PARKING MANAGEMENT SYSTEM =====\n");
    printf("Total slots: %d floors x %d slots = %d\n", FLOORS, SLOTS, FLOORS * SLOTS);

    do {
        printf("\n---------- MENU ----------\n");
        printf("1. Display parking\n");
        printf("2. Vehicle entry\n");
        printf("3. Vehicle exit\n");
        printf("4. Floor with maximum occupancy\n");
        printf("5. Find consecutive free slots\n");
        printf("6. Search vehicle\n");
        printf("7. Show exit records\n");
        printf("8. Show statistics\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");

        choice = readInt(1, 9);

        switch (choice) {
            case 1: displayParking(parking); break;
            case 2: vehicleEntry(); break;
            case 3: vehicleExit(); break;
            case 4: floorWithMaxOccupancy(); break;
            case 5: findConsecutiveSlots(parking); break;
            case 6: searchVehicle(); break;
            case 7: showRecords(); break;
            case 8: updateStats(0, 0); break;
            case 9: printf("Thank you! Exiting...\n"); break;
        }
    } while (choice != 9);

    return 0;
}

// reads a full line and removes the '\n'
void readLine(char line[], int size)
{
    int i, c;

    if (fgets(line, size, stdin) == NULL) {
        printf("\nInput ended. Exiting...\n");
        exit(0);
    }

    for (i = 0; line[i] != '\0'; i++) {
        if (line[i] == '\n') {
            line[i] = '\0';
            return;
        }
    }

    while ((c = getchar()) != '\n' && c != EOF);
}

int readInt(int min, int max)
{
    char line[50];
    int i, num, valid;

    while (1) {
        readLine(line, 50);
        valid = 1;

        if (strlen(line) == 0 || strlen(line) > 5)
            valid = 0;

        for (i = 0; line[i] != '\0'; i++) {
            if (line[i] < '0' || line[i] > '9')
                valid = 0;
        }

        if (valid == 1) {
            num = atoi(line);
            if (num >= min && num <= max)
                return num;
        }

        printf("Invalid input! Enter a number between %d and %d: ", min, max);
    }
}

void readVehicleNo(char vno[])
{
    char line[50];
    int i, len, valid;

    while (1) {
        printf("Enter vehicle number (e.g. MH12AB1234): ");
        readLine(line, 50);

        len = strlen(line);
        valid = 1;
        if (len < 4 || len > 12)
            valid = 0;

        for (i = 0; i < len; i++) {
            if (line[i] >= 'a' && line[i] <= 'z')
                line[i] = line[i] - 32;
            else if (!((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= '0' && line[i] <= '9')))
                valid = 0;
        }

        if (valid == 1) {
            strcpy(vno, line);
            return;
        }
        printf("Invalid! Use 4 to 12 letters/digits only (no spaces or symbols).\n");
    }
}

void displayParking(int (*p)[SLOTS])
{
    int f, s, total = 0;

    printf("\n         ");
    for (s = 0; s < SLOTS; s++)
        printf("S%d  ", s + 1);
    printf("\n");

    for (f = 0; f < FLOORS; f++) {
        printf("Floor %d: ", f + 1);
        for (s = 0; s < SLOTS; s++) {
            if (p[f][s] == 0)
                printf("--  ");
            else if (vehicleType[f][s] == 1)
                printf("B   ");
            else
                printf("C   ");
        }
        printf("  (%d free)\n", SLOTS - countOccupied(p[f]));
        total = total + countOccupied(p[f]);
    }

    printf("\n-- = Empty, B = Bike, C = Car\n");
    printf("Occupied: %d   Available: %d\n", total, FLOORS * SLOTS - total);
}

// gives the first empty slot (floor 1 slot 1 is nearest)
int allocateNearestSlot(int (*p)[SLOTS], int *floor, int *slot)
{
    int f, s;
    for (f = 0; f < FLOORS; f++) {
        for (s = 0; s < SLOTS; s++) {
            if (p[f][s] == 0) {
                *floor = f;
                *slot = s;
                return 1;
            }
        }
    }
    return 0;
}

int findVehicle(char vno[], int *floor, int *slot)
{
    int f, s;
    for (f = 0; f < FLOORS; f++) {
        for (s = 0; s < SLOTS; s++) {
            if (parking[f][s] == 1 && strcmp(vehicleNo[f][s], vno) == 0) {
                *floor = f;
                *slot = s;
                return 1;
            }
        }
    }
    return 0;
}

void vehicleEntry()
{
    int f, s, ff, ss, type, hour, min;
    char vno[15];

    if (allocateNearestSlot(parking, &f, &s) == 0) {
        printf("Sorry, parking is full!\n");
        return;
    }

    readVehicleNo(vno);

    if (findVehicle(vno, &ff, &ss) == 1) {
        printf("This vehicle is already parked!\n");
        return;
    }

    printf("Vehicle type (1 = Bike, 2 = Car): ");
    type = readInt(1, 2);
    printf("Entry time - hour (0-23): ");
    hour = readInt(0, 23);
    printf("Entry time - minute (0-59): ");
    min = readInt(0, 59);

    parking[f][s] = 1;
    strcpy(vehicleNo[f][s], vno);
    vehicleType[f][s] = type;
    entryTime[f][s] = hour * 60 + min;

    updateStats(1, 0);

    printf("\nVehicle %s parked at Floor %d, Slot %d\n", vno, f + 1, s + 1);
}

void vehicleExit()
{
    int f, s, hour, min, exitTime, minutes, charge;
    int *slotPtr;
    char vno[15];

    readVehicleNo(vno);

    if (findVehicle(vno, &f, &s) == 0) {
        printf("Vehicle not found!\n");
        return;
    }

    slotPtr = &parking[f][s];

    printf("Entry time was %02d:%02d\n", entryTime[f][s] / 60, entryTime[f][s] % 60);
    printf("Exit time - hour (0-23): ");
    hour = readInt(0, 23);
    printf("Exit time - minute (0-59): ");
    min = readInt(0, 59);
    exitTime = hour * 60 + min;

    if (exitTime < entryTime[f][s]) {
        printf("Exit time cannot be before entry time!\n");
        return;
    }

    minutes = exitTime - entryTime[f][s];
    charge = calculateCharges(minutes, vehicleType[f][s]);

    printf("\n------ BILL ------\n");
    printf("Vehicle : %s (", vno);
    printType(typeName, vehicleType[f][s]);
    printf(")\n");
    printf("Slot    : Floor %d, Slot %d\n", f + 1, s + 1);
    printf("Time    : %d hr %d min\n", minutes / 60, minutes % 60);
    printf("Amount  : Rs. %d\n", charge);

    if (recCount < MAX_RECORDS) {
        strcpy(recVehicle[recCount], vno);
        recFloor[recCount] = f;
        recSlot[recCount] = s;
        recIn[recCount] = entryTime[f][s];
        recOut[recCount] = exitTime;
        recCharge[recCount] = charge;
        recCount++;
    }

    emptySlot(&slotPtr);
    strcpy(vehicleNo[f][s], "");

    updateStats(2, charge);
}

// pointer to pointer: empties the slot and sets the pointer to NULL
void emptySlot(int **slotPtr)
{
    **slotPtr = 0;
    *slotPtr = NULL;
}

// bike: 10 first hour, 5 after. car: 30 first hour, 20 after
int calculateCharges(int minutes, int type)
{
    int hours = minutes / 60;
    if (minutes % 60 != 0)
        hours++;
    if (hours == 0)
        hours = 1;

    if (type == 1)
        return 10 + (hours - 1) * 5;
    else
        return 30 + (hours - 1) * 20;
}

int countOccupied(int *row)
{
    int i, count = 0;
    for (i = 0; i < SLOTS; i++) {
        if (*(row + i) == 1)
            count++;
    }
    return count;
}

void floorWithMaxOccupancy()
{
    int f, max = 0;
    int count[FLOORS];

    for (f = 0; f < FLOORS; f++) {
        count[f] = countOccupied(parking[f]);
        printf("Floor %d: %d vehicles\n", f + 1, count[f]);
        if (count[f] > max)
            max = count[f];
    }

    if (max == 0) {
        printf("Parking is empty.\n");
        return;
    }

    printf("Maximum occupancy (%d vehicles): ", max);
    for (f = 0; f < FLOORS; f++) {
        if (count[f] == max)
            printf("Floor %d  ", f + 1);
    }
    printf("\n");
}

// recursion: counts free slots next to each other from 'start'
int countFreeFrom(int *row, int start)
{
    if (start >= SLOTS || row[start] == 1)
        return 0;
    return 1 + countFreeFrom(row, start + 1);
}

void findConsecutiveSlots(int (*p)[SLOTS])
{
    int need, f, s, len, found = 0;

    printf("How many slots needed together (1-%d): ", SLOTS);
    need = readInt(1, SLOTS);

    for (f = 0; f < FLOORS; f++) {
        s = 0;
        while (s < SLOTS) {
            if (p[f][s] == 0) {
                len = countFreeFrom(p[f], s);
                if (len >= need) {
                    printf("Floor %d: Slot %d to Slot %d are free\n", f + 1, s + 1, s + len);
                    found = 1;
                }
                s = s + len;
            } else {
                s++;
            }
        }
    }

    if (found == 0)
        printf("No %d free slots together.\n", need);
}

void searchVehicle()
{
    int f, s;
    char vno[15];

    readVehicleNo(vno);

    if (findVehicle(vno, &f, &s) == 1) {
        printf("%s is parked at Floor %d, Slot %d (", vno, f + 1, s + 1);
        printType(typeName, vehicleType[f][s]);
        printf(")\n");
        printf("Entry time: %02d:%02d\n", entryTime[f][s] / 60, entryTime[f][s] % 60);
    } else {
        printf("Vehicle not found!\n");
    }
}

void showRecords()
{
    int i;

    if (recCount == 0) {
        printf("No vehicle has exited yet.\n");
        return;
    }

    printf("\nNo  Vehicle       Slot    In     Out    Amount\n");
    for (i = 0; i < recCount; i++) {
        printf("%-3d %-13s F%d-S%d   %02d:%02d  %02d:%02d  Rs. %d\n",
               i + 1, recVehicle[i], recFloor[i] + 1, recSlot[i] + 1,
               recIn[i] / 60, recIn[i] % 60, recOut[i] / 60, recOut[i] % 60,
               recCharge[i]);
    }
}

void printType(char **names, int type)
{
    printf("%s", *(names + type));
}

// action: 1 = entry, 2 = exit, 0 = show
void updateStats(int action, int amount)
{
    static int parked = 0;
    static int served = 0;
    static int revenue = 0;

    if (action == 1) {
        parked++;
    } else if (action == 2) {
        parked--;
        served++;
        revenue = revenue + amount;
    } else {
        printf("\n----- STATISTICS -----\n");
        printf("Vehicles parked now  : %d\n", parked);
        printf("Total vehicles served: %d\n", served);
        printf("Total money collected: Rs. %d\n", revenue);
    }
}
