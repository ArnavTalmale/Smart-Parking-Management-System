# Smart Parking Management System

Computer Programming project (Group 4) written in C.

A console program that manages a 3-floor parking lot with 8 slots per floor.

## Features
- Vehicle entry with nearest free slot allocation
- Vehicle exit with parking charges (bike / car)
- Parking grid display
- Floor with maximum occupancy
- Search for consecutive free slots
- Search vehicle by number
- Exit records and statistics

## Menu
```
1. Display parking
2. Vehicle entry
3. Vehicle exit
4. Floor with maximum occupancy
5. Find consecutive free slots
6. Search vehicle
7. Show exit records
8. Statistics
9. Exit
```

## Charges
| Vehicle | Base | Per hour |
|---------|------|----------|
| Bike    | Rs 10 | Rs 5    |
| Car     | Rs 30 | Rs 20   |

Time is rounded up to full hours, with a minimum of 1 hour.

## How to run
```
gcc parking.c -o parking
./parking
```
