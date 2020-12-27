/*
 * Author: Evin Li
 * Date: 11/25/2020 12:12 PM
 *
 * Purpose: This program implements a simple counter using two user-defined integer
 *			values as the start and end points, and displays the counter value on an
 *			LED screen. The counter will count down from an initial value by increments
 *			of 4, stopping just before the end value is reached. To start/pause the
 *			counter, use output channel 1.
 */

#define _CRT_SECURE_NO_WARNINGS

 /* headers */
#include <stdio.h>
#include <stdlib.h>
#include <DAQlib.h>
#include <Windows.h>

/* status constants */
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0
#define NUM_DISPLAYS 8
#define ONE_SECOND 1000
#define COUNT_AMT 4

/* simulator configuration */
#define SIMULATOR 4

/* I/O channels */
#define SWITCH1 1

/* function prototypes */
void simpleCounter(void);
void writeDigit(int digit, int position);
void writeNumber(int number);

/* main function */
int main(void) {
	printf("Launching Program...\n");

	if (setupDAQ(SIMULATOR)) {
		simpleCounter();
	}
	else {
		printf("ERROR: Cannot initialize the DAQ...\n");
	}

	/* pause before exit */
	system("PAUSE");
	return 0;
}

/*
 * Purpose: Implements all the logic of the counter, continuing to loop
 *			until the user decides to quit
 */
void simpleCounter(void) {
	int startVal, stopVal;

	/* prompt the user for the start and stop values of the counter */
	printf("Enter the start value of the counter:\n");
	scanf("%i", &startVal);

	printf("Enter the stop value of the counter:\n");
	scanf("%i", &stopVal);

	/* check the statuses of input channel #1 and initialize time and count variables */
	int runSwitch = digitalRead(SWITCH1);
	int time = 0;
	int count = startVal;

	int firstIteration = TRUE; /* flag for updating time when run switch is pressed for the first time */
	time = millis();

	while (continueSuperLoop()) {
		writeNumber(count);

		runSwitch = digitalRead(SWITCH1);

		/* decrement count value if 4 seconds have elapsed and run switch is on */
		if (runSwitch) {
			if (firstIteration) {
				time = millis();
				firstIteration = FALSE;
			}

			if (millis() - time >= (COUNT_AMT * ONE_SECOND) && (count - COUNT_AMT) >= stopVal) {
				count -= COUNT_AMT; /* decrement count by 4 */
				time = millis(); /* update time */
			}
		}
	}
}


/*
 * Purpose: Writes a digit to a 7-segment display
 * Parameters: digit - digit to be displayed
 *			   position - position the digit needs to be displayed
 */
void writeDigit(int digit, int position) {
	static int digits[] = { 252, 96, 218, 242, 102, 182, 190, 224, 254, 246, 0 };

	displayWrite(digits[digit], position);
}

/*
 * Purpose: Writes a non-negative integer to the eight 7-segment LED
 *			displays of the DAQ module
 * Parameters: number - integer value to display
 */
void writeNumber(int number) {
	int digitVal;
	int position = 0;

	while (number > 0 && position < NUM_DISPLAYS) {
		digitVal = number % 10;
		writeDigit(digitVal, position);

		number /= 10;
		position++;
	}

	while (position < NUM_DISPLAYS) {
		writeDigit(10, position);
		position++;
	}
}

