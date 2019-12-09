/*
This example program sounds the bell when driver is on seat AND haven't closed the doors. Use the general framework and the function shells, replace the code inside  the control_action() function with your own code.

Note: This code is designed to run in an infinite loop to mimic a real control system. 
If you prefer to read the inputs from a file, modify the code appropriately to terminate the loop when all the inputs have been processed.

run this file as : gcc filename.c -o executableName

Created By WilliamHe(2018053277) & RTsui(2018058445)
*/
#include <stdio.h>  //This is useful to do i/o to test the code
#include <stdlib.h> //exit()
#include <unistd.h> //sleep() for UNIX/mingw-w64
#include <string.h> //strlen()

#define DEBUG 1
#define MAX 64

unsigned int driver_on_seat;            //DOS
unsigned int driver_seat_belt_fastened; //DSBF
unsigned int engine_running;            //ER
unsigned int doors_closed;              //DC
unsigned int key_in_car;                //KIC
unsigned int door_lock_lever;           //DLC
unsigned int brake_pedal;               //BP
unsigned int car_moving;                //CM

unsigned int bell;                      //BELL
unsigned int door_lock_actuator;        //DLA
unsigned int brake_actuator;            //BA

char first_run = 1;
FILE *sensors;
FILE *actuators;

void read_inputs_from_ip_if();
void write_output_to_op_if();

void read_inputs_from_ip_if() {
	//place your input code here
	//to read the current state of the available sensors
    char temp[MAX];
	char *ptr;
	
	if (first_run) {
		sensors = fopen("sensors.txt", "w");//initiate the file
		if (sensors == NULL) {
			puts("e: File create failed.");
            fclose(sensors);
			exit(2);
		} //still NULL

		fprintf(sensors, "DOS=\n");  //driver on seat
		fprintf(sensors, "DSBF=\n"); //seatbelt fastened
		fprintf(sensors, "ER=\n");   //Engine running
		fprintf(sensors, "DC=\n");   //door closed
		fprintf(sensors, "KIC=\n");  //key in car
		fprintf(sensors, "DLC=\n");  //door lock lever
		fprintf(sensors, "BP=\n");   //brake pedal
		fprintf(sensors, "CM=\n");   //car moving
		puts("\nWarming up: wait for sensors...\n");
		fclose(sensors);
		first_run = 0;
		sleep(5);
		read_inputs_from_ip_if();
		return;
	} else sensors = fopen("sensors.txt", "r");
	
	fgets(temp, MAX, sensors); //the last character is '\n', so we need to get the second last character
    driver_on_seat = strtol(&temp[strlen(temp) - 2], &ptr, 10);
    printf("DOS=%u\t", driver_on_seat);

    fgets(temp, MAX, sensors);
    driver_seat_belt_fastened = strtol(&temp[strlen(temp) - 2], &ptr, 10);
    printf("DSBF=%u\t", driver_seat_belt_fastened);

    fgets(temp, MAX, sensors);
    engine_running = strtol(&temp[strlen(temp) - 2], &ptr, 10);
    printf("ER=%u\t", engine_running);

    fgets(temp, MAX, sensors);
    doors_closed = strtol(&temp[strlen(temp) - 2], &ptr, 10);
    printf("DC=%u\t", doors_closed);

    fgets(temp, MAX, sensors);
    key_in_car = strtol(&temp[strlen(temp) - 2], &ptr, 10);
    printf("KIC=%u\t", key_in_car);

    fgets(temp, MAX, sensors);
    door_lock_lever = strtol(&temp[strlen(temp) - 2], &ptr, 10);
    printf("DLC=%u\t", door_lock_lever);

    fgets(temp, MAX, sensors);
    brake_pedal = strtol(&temp[strlen(temp) - 2], &ptr, 10);
    printf("BP=%u\t", brake_pedal);

    fgets(temp, MAX, sensors);
    car_moving = strtol(&temp[strlen(temp) - 2], &ptr, 10);
    printf("CM=%u\n", car_moving);
    fclose(sensors);
}

void write_output_to_op_if() {
    //place your output code here
    //to display/print the state of the 3 actuators (DLA/BELL/BA)
	actuators = fopen("actuators.txt", "w");//initiate the file
	if (actuators == NULL) {
		puts("e: File create failed.");
		exit(2);
	} //still NULL
			
    printf("BELL=%u\t", bell);
    printf("DLA=%u\t", door_lock_actuator);
    printf("BA=%u\n", brake_actuator);

    fprintf(actuators, "BELL=%u\n", bell);  //bell alert
    fprintf(actuators, "DLA=%u\n", door_lock_actuator);   //door lock actuator
    fprintf(actuators, "BA=%u\n", brake_actuator);    //brake actuator
	
	fclose(actuators);//if not closed, files are not written
}

//The code segment which implements the decision logic
void control_action() {
	/*
	The code given here sounds the bell when driver is on seat 
	AND hasn't closed the doors. (Requirement-2)
 	Replace this code segment with your own code.
	*/
    if (driver_on_seat && !driver_seat_belt_fastened && engine_running) {// requirement 1
        bell = 1;
    } else if (driver_on_seat && engine_running && !doors_closed) {// requirement 2
        bell = 1;
    } else {// requirement 3
        bell = 0;
    }

    if ((driver_on_seat && door_lock_lever) || (!driver_on_seat && !key_in_car && door_lock_lever)) { // requirement 4
        door_lock_actuator = 1;
    } else if ((!driver_on_seat && key_in_car && door_lock_lever) || (!door_lock_lever)) {
        door_lock_actuator = 0;
    }

    if (brake_pedal && car_moving) { // requirement 5
        brake_actuator = 1;
    } else if (!brake_pedal || !car_moving) {
        brake_actuator = 0;
    }
}

/* ---     You should not have to modify anything below this line ---------*/

int main(int argc, char *argv[]) {
	/*The main control loop which keeps the system alive and responsive for ever, 
	until the system is powered off */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
	for (;;) {
		read_inputs_from_ip_if();
		control_action();
		write_output_to_op_if();
		DEBUG ? sleep(2) : 0;
	}
#pragma clang diagnostic pop
}
