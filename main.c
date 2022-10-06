#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;

#define CAR_IN_STOCK 7
#define MAX_SALES 30
#define MAX_CAR_NAME 13
#define MAX_NAME 37
#define FILE_OPENED 0
#define FILE_CLOSED 1
#define FILE_ERROR 2
#define CSV_FILE "data.csv"
#define bool unsigned char


int index[MAX_SALES], sortIndex[MAX_SALES], carIndex[CAR_IN_STOCK], sortCarIndex[CAR_IN_STOCK];
int carPrice[CAR_IN_STOCK] = { 500,1500,1000,2000,3000,4000,5000 }, salesStatistics[MAX_SALES], b[MAX_SALES], discountGiven[MAX_SALES];
int totalSales;
short carYears[CAR_IN_STOCK] = { 1990,1990,2000,2000,2010,2010,2010 }, carSaledYears[MAX_SALES], isDiscount[MAX_SALES];
short carHere[CAR_IN_STOCK] = { 1,2,1,5,1,2,3 };
short customerAge, numberOfCar, numberOfSales, carSaled, count = 1, isNewSales = 1;
char carNames[CAR_IN_STOCK][MAX_CAR_NAME + 1] = { "AUDI","Volkswagen","AUDI","NISSAN" ,"AUDI" ,"NISSAN" ,"Volkswagen" };
char userName[MAX_SALES][MAX_NAME + 1];// = { "Empty        ","Empty        ","Empty        ","Empty        " ,"Empty        " ,"Empty        " ,"Empty        " , "Empty        "  ,"Empty        "  ,"Empty        " };
char salesNames[MAX_SALES][MAX_CAR_NAME + 1];// = { "Empty        ","Empty        ","Empty        ","Empty        " ,"Empty        " ,"Empty        " ,"Empty        " , "Empty        "  ,"Empty        "  ,"Empty        " };
char poundSign = 156, userAnswer;

void clear_screen() {
	// this tells the Command Prompt (i.e. the System's output mechanisn) 
	// to do something to clear the screen/console
	// this is purely for the user's benefit, to keep the screen clear of clutter
	printf("\nPress Enter to return to the menu");
	getch();
	getch();
	system("cls");
	return;
}

FILE* file;
unsigned char fileStatus = FILE_CLOSED;

FILE* createFile(char fileName[201]) {
	// open the file for writing
	file = fopen(fileName, "w");
	// then close it immediately if successful
	if (file != NULL) {
		fclose(file);
	}
	// return the pointer so that the success may be checked
	return file;
}

void openFile(char fileName[201], char mode[4]) {
	// attempt to open the file
	file = fopen(fileName, mode);

	// if file's value is NULL, then something happened...
	if (file == NULL) {

		// attempt to create it first and then check if it's still NULL, meaning
		//		that something still failed
		if (createFile(fileName) == NULL) {
			fileStatus = FILE_ERROR;

			// will produce the output: Error opening D:\data.csv: No such file of directory
			printf("Error opening %s: %s", fileName, strerror(errno));
		}
		else {
			// we managed to create the file ... so now ...
			// execute this function again with the same parameters
			//		because it should now succeed
			openFile(fileName, mode);
		}

	}
	// otherwise, all OK
	else {
		fileStatus = FILE_OPENED;
	}
	return;
}

void closeFile() {
	// only attempt to close the file if it's already open
	if (fileStatus == FILE_OPENED) {
		fclose(file);
		fileStatus = FILE_CLOSED;
	}
	return;
}

void readDataFromFile() {
	// will keep track of how many lines were read from the file
	int lineCounter = 0;

	// this is an infinite loop, we'll manually stop it once we reach the end of the file 
	while (1) {

		unsigned int carPrices = 0, discountGivenPerSaleValue = 0;
		unsigned short carYear = 0, isDiscountGiven = 0;
		char together[MAX_CAR_NAME + MAX_NAME];
		char carName[MAX_CAR_NAME] = "m";
		char customerNameValue[MAX_NAME] = "m";///

		int scanResult = fscanf(
			file, // the file stream
			"%d,%hd,%d,%hd,%50[^\n]s", // the format of the line
			&carPrices, // the variables, one for each placeholder in the format above
			&carYear,
			&discountGivenPerSaleValue,
			&isDiscountGiven,
			&together
		);

		//printf("test %s\n", together);
		//printf("%d", scanResult);
		//getch();

		// if we reached the end of the file
		if (scanResult == EOF || carPrices < 450 || carPrices > 5000) {
			// then, stop the loop
			break;
		}

		int i = 0, i1 = 0, i2 = 0, wasComma = 0;
		while (together[i] != NULL) {
			if (together[i] == ',') {
				wasComma = 1;
			}
			else {
				if (wasComma == 0) {
					carName[i1] = together[i];
					i1++;
				}
				else {
					customerNameValue[i2] = together[i];
					i2++;
				}
			}
			i++;
		}

		carSaled++;
		totalSales += carPrices;

		// add the bits of data that were read above into the correct arrays
		salesStatistics[lineCounter] = carPrices;
		carSaledYears[lineCounter] = carYear;
		discountGiven[lineCounter] = discountGivenPerSaleValue;
		isDiscount[lineCounter] = (bool)isDiscountGiven;

		// also cast (convert) the discountGivenPerSaleValue from unsigned short to a bool type
		//		before putting it in the discountGivenPerSale array
		// need to use strcpy here because we're working with strings
		strcpy(salesNames[lineCounter], carName);
		//userName[lineCounter] = "Hidden       ";
		strcpy(userName[lineCounter], customerNameValue);
		//printf("Customer Name %s\n", userName[lineCounter]);

		// increment the lineCounter, ready for next line that might be read
		lineCounter++;
	}

	// make sure the numberOfSales variable is also aware of how many sales are available after the above operation
	numberOfSales = lineCounter;
	return;
}


void getDataFromFile() {
	// this function used to check if we can or can't take information from file
	// and to call reading function
	openFile(CSV_FILE, "r");

	if (fileStatus == FILE_OPENED) {
		readDataFromFile();
	}
	else if (fileStatus == FILE_ERROR) {
		printf("There was an error trying to read from the file %s.", CSV_FILE);
		// this function requires a char value, so we give it one 
		//		that'll tell it we're using it because of a file error
		//			see the function body, it's been updated to check for
		//			this underscore character
		clear_screen();
	}

	closeFile();
	return;
}

void writeDataToFile() {
	// loop through every sale
	for (int i = 0; i < numberOfSales; i++) {

		// this string will be built up bit by bit before being written to the opened file
		char line[201] = "";
		// this string will contain the converted int value
		char data[50] = "";

		// convert the unsigned short value into a string, put it in the data string variable; 
		//		the "10" refers to base 10, which is what regular numeric values are written in
		//		e.g. saying you're 22 years old means that 22, in this case, is written in base 10
		//			because that's how we people use numbers by default
		// also cast the unsigned short value into an int type before converting
		_itoa((int)salesStatistics[i], data, 10);
		//data = "";
		// add the amount of tickets to the line; first time we use strcpy, 
		//		then strcat to add more to the string
		strcpy(line, data);
		// add a comma to separate this value from the next on this line
		strcat(line, ",");

		// convert the value into a string
		_itoa((int)carSaledYears[index[i]], data, 10);
		// add the type of ticket to the line
		strcat(line, data);
		// add a comma to separate this value from the next on this line
		strcat(line, ",");

		// convert the value into a string
		_itoa((int)discountGiven[index[i]], data, 10);
		// add the discount given to the line
		strcat(line, data);
		// add a comma to separate this value from the next on this line
		strcat(line, ",");

		_itoa((int)isDiscount[index[i]], data, 10);
		// add the discount given to the line
		strcat(line, data);
		// add a comma to separate this value from the next on this line
		strcat(line, ",");

		// add the Car Name to the line
		strcat(line, salesNames[index[i]]);
		// add a comma to separate this value from the next on this line
		strcat(line, ",");

		// add the customer name to the line
		strcat(line, userName[index[i]]);
		//printf("%s\n", userName[index[i]]);
		//printf("%s\n", line);

		// write line to file
		fprintf(file, line);

		// only add a newline character if we're not yet writing the very last
		//		line in the file
		if (i < numberOfSales - 1) {
			fprintf(file, "\n");
		}

	}
	return;
}

void saveDataToFile() {
	// this function used to check that we can upload new information to file 
	// and call uploading function
	openFile(CSV_FILE, "w");

	if (fileStatus == FILE_OPENED) {
		writeDataToFile();
	}
	else if (fileStatus == FILE_ERROR) {
		printf("There was an error trying to write to the file %s.", CSV_FILE);
		// this function requires a char value, so we give it one 
		//		that'll tell it we're using it because of a file error
		//			see the function body, it's been updated to check for
		//			this underscore character
		clear_screen();
	}

	closeFile();
	return;
}

void merge_sort_sales(short leftPointer, short rightPointer) {
	// this function used to sort sales information
	if (rightPointer - leftPointer < 2) {
		if (rightPointer - leftPointer == 1) {
			if (salesStatistics[rightPointer] > salesStatistics[leftPointer]) {
				int x;
				x = salesStatistics[rightPointer];
				salesStatistics[rightPointer] = salesStatistics[leftPointer];
				salesStatistics[leftPointer] = x;
				x = index[rightPointer];
				index[rightPointer] = index[leftPointer];
				index[leftPointer] = x;
				//swap(salesStatistics[r],salesStatistics[l]);
			}
		}
		return;
	}
	int midPointer = (rightPointer + leftPointer) / 2;
	int newLeftPointer = leftPointer, newRightPointer = midPointer + 1, cnt = 0;
	merge_sort_sales(leftPointer, midPointer);
	merge_sort_sales(midPointer + 1, rightPointer);
	for (int i = 1; i <= (rightPointer - leftPointer + 1); i++) {
		if (newRightPointer == rightPointer + 1) {
			b[cnt] = salesStatistics[newLeftPointer];
			sortIndex[cnt] = index[newLeftPointer];
			cnt++;
			newLeftPointer++;
		}
		else if (newLeftPointer == midPointer + 1) {
			b[cnt] = salesStatistics[newRightPointer];
			sortIndex[cnt] = index[newRightPointer];
			cnt++;
			newRightPointer++;
		}
		else {
			if (salesStatistics[newLeftPointer] > salesStatistics[newRightPointer]) {
				b[cnt] = salesStatistics[newLeftPointer];
				sortIndex[cnt] = index[newLeftPointer];
				cnt++;
				newLeftPointer++;
			}
			else {
				b[cnt] = salesStatistics[newRightPointer];
				sortIndex[cnt] = index[newRightPointer];
				cnt++;
				newRightPointer++;
			}
		}
	}
	for (int i = 0; i < (rightPointer - leftPointer + 1); ++i) {
		salesStatistics[leftPointer + i] = b[i];
		index[leftPointer + i] = sortIndex[i];
	}
	return;
}

void merge_sort_cars(short leftPointer, short rightPointer) {

	// this fucntion is used to sort information about cars

	if (rightPointer - leftPointer < 2) {
		if (rightPointer - leftPointer == 1) {
			if (carHere[rightPointer] > carHere[leftPointer]) {
				//printf("%hd %hd %hd %hd ^^^\n", carHere[r], carHere[l], carIndex[r], carIndex[l]);
				int x;
				x = carHere[rightPointer];
				carHere[rightPointer] = carHere[leftPointer];
				carHere[leftPointer] = x;
				x = carIndex[rightPointer];
				carIndex[rightPointer] = carIndex[leftPointer];
				carIndex[leftPointer] = x;
				//printf("%hd %hd %hd %hd &&&\n", carHere[r], carHere[l], carIndex[r], carIndex[l]);
				//swap(carHere[r],carHere[l]);
			}
		}
		return;
	}
	int midPointer = (rightPointer + leftPointer) / 2;
	int newleftPointer = leftPointer, newrightPointer = midPointer + 1, cnt = 0;
	merge_sort_cars(leftPointer, midPointer);
	merge_sort_cars(midPointer + 1, rightPointer);
	for (int i = 0; i < (rightPointer - leftPointer + 1); i++) {
		if (newrightPointer == rightPointer + 1) {
			b[cnt] = carHere[newleftPointer];
			sortCarIndex[cnt] = carIndex[newleftPointer];
			cnt++;
			newleftPointer++;
		}
		else if (newleftPointer == midPointer + 1) {
			b[cnt] = carHere[newrightPointer];
			sortCarIndex[cnt] = carIndex[newrightPointer];
			cnt++;
			newrightPointer++;
		}
		else {
			if (carHere[newleftPointer] > carHere[newrightPointer]) {
				b[cnt] = carHere[newleftPointer];
				sortCarIndex[cnt] = carIndex[newleftPointer];
				newleftPointer++;
				cnt++;
			}
			else {
				b[cnt] = carHere[newrightPointer];
				sortCarIndex[cnt] = carIndex[newrightPointer];
				cnt++;
				newrightPointer++;
			}
		}
	}
	for (int i = 0; i < (rightPointer - leftPointer + 1); ++i) {
		carHere[leftPointer + i] = b[i];
		carIndex[leftPointer + i] = sortCarIndex[i];
	}
	return;
}

void greet_user_and_print_menu() {
	// First thing that user see, used to explain everything

	printf("Welcome, to our Car buying program.\n\nMenu:\na.Buy a car\nb.Look for sales statistics\nc.Exit\n");

	scanf("\n %c", &userAnswer);
	if (userAnswer != 'a' && userAnswer != 'b' && userAnswer != 'c') {
		count = 0;
	}
	else {
		count = 1;
	}
	return;
}

void print_cars() {
	// funtion for giving user information about cars that are in stock

	printf("We have 7 types of cars that you can buy. Now, I'll give you a menu and time to choose a car\n");

	for (int i = 0; i < CAR_IN_STOCK; i += 1) {
		if (carHere[i] == 0) {
			printf("%d .  %s   OUT OF STOCK\n", i + 1, carNames[carIndex[i]]);
		}
		else {
			if (carNames[carIndex[i]][0] == 'V') {
				printf("%d . %s    Year %hd - %d %c. %hd cars left\n", i + 1, carNames[carIndex[i]], carYears[carIndex[i]], carPrice[carIndex[i]], poundSign, carHere[i]);
			}
			else if (carNames[carIndex[i]][0] == 'A') {
				if (carPrice[carIndex[i]] == 500) {
					printf("%d . %s          Year %hd - %d %c.  %hd cars left\n", i + 1, carNames[carIndex[i]], carYears[carIndex[i]], carPrice[carIndex[i]], poundSign, carHere[i]);
				}
				else {
					printf("%d . %s          Year %hd - %d %c. %hd cars left\n", i + 1, carNames[carIndex[i]], carYears[carIndex[i]], carPrice[carIndex[i]], poundSign, carHere[i]);
				}
			}
			else if (carNames[carIndex[i]][0] == 'N') {
				printf("%d . %s        Year %hd - %d %c. %hd cars left\n", i + 1, carNames[carIndex[i]], carYears[carIndex[i]], carPrice[carIndex[i]], poundSign, carHere[i]);
			}
		}
	}

	printf("Also, I wanted to ask, how old are you? If you apply for our social program you can get a discount\n");
	scanf("\n %hd", &customerAge);

	printf("And what is your name?\n");
	scanf("\n %[^\n]s", &userName[numberOfSales]);

	return;
}

void buying_process() {
	// funstion for gettin user replies and checking if everything alright

	printf("\nWhen you will choose please give me the number of car that you want to buy from the menu above:");
	scanf("\n %hd", &numberOfCar);

	//system("cls");

	numberOfCar = numberOfCar - 1;
	if (numberOfCar > 6 || numberOfCar < 0 || carHere[numberOfCar] == 0) {
		printf("Error, wrong number. Try again later\n");
	}
	else {
		isNewSales = 1;
		carSaled++;
		carHere[numberOfCar] = carHere[numberOfCar] - 1;

		index[numberOfSales] = numberOfSales;

		strcpy(salesNames[numberOfSales], carNames[numberOfCar]);

		carSaledYears[numberOfSales] = carYears[numberOfCar];

		if (customerAge >= 25 && customerAge <= 60) {
			isDiscount[numberOfSales] = 0;
			printf("Thank you for buying in our car salon.\nYou bought a %s Year %hd .\nTotal cost %d %c .\nGood bye!\n", carNames[carIndex[numberOfCar]], carYears[carIndex[numberOfCar]], carPrice[carIndex[numberOfCar]], poundSign);
			totalSales += carPrice[carIndex[numberOfCar]];
			salesStatistics[numberOfSales] = carPrice[carIndex[numberOfCar]];
		}
		else {
			int totalPriceWithDiscount = carPrice[carIndex[numberOfCar]] - carPrice[carIndex[numberOfCar]] * 10 / 100;
			printf("Thank you for buying in our car salon.\nYou bought a %s Year %hd .\nTotal cost %d %c .\nGood bye!\n", carNames[carIndex[numberOfCar]], carYears[carIndex[numberOfCar]], totalPriceWithDiscount, poundSign);
			totalSales += totalPriceWithDiscount;
			salesStatistics[numberOfSales] = totalPriceWithDiscount;
			isDiscount[numberOfSales] = 1;
			discountGiven[numberOfSales] = carPrice[carIndex[numberOfCar]] / 100 * 10;
		}
		numberOfSales++;
	}
}

void print_statistics() {
	// function for giving user statistics

	printf("%hd Car saled for %d %c.\n", carSaled, totalSales, poundSign);
	printf("Want to get more information?\nPrint a if you want to or b If not\n");
	scanf("\n %c", &userAnswer);
	if (userAnswer == 'a') {
		for (int i = 0; i < numberOfSales; i++) {
			if (discountGiven[index[i]]) {
				printf("Sale Index: %d | Sale Amount: %d | Discount Amount: %d | Type of car: %s \nYear: %hd |Customer Name: %s\n\n", i + 1, salesStatistics[i], discountGiven[index[i]], salesNames[index[i]], carSaledYears[index[i]], userName[index[i]]);
			}
			else {
				printf("Sale Index: %d | Sale Amount: %d | Discount Amount: No Discount | Type of car: %s \nYear: %hd |Customer Name: %s\n\n", i + 1, salesStatistics[i], salesNames[index[i]], carSaledYears[index[i]], userName[index[i]]);
			}
		}
	}
	return;
}

void print_discount_applying() {
	// function for checking if there's a dicsount user can use

	if (customerAge >= 25 && customerAge <= 60) {
		printf("Sorry, You're not applaying for our social program\n");
	}
	else {
		printf("You're applaying for our social programm you will get 10 percent discount!!!\n\n");
	}
	return;
}

void sales_update() {
	// function that check if we need to do updates in data
	// if we need to then call function for update

	if (isNewSales == 1) {
		merge_sort_sales(0, numberOfSales - 1);
		merge_sort_cars(0, CAR_IN_STOCK - 1);
		isNewSales = 0;
	}
	return;
}

int main() {
	//read from file
	getDataFromFile();
	for (int i = 0; i < CAR_IN_STOCK; ++i) {
		carIndex[i] = i;
	}
	for (int i = 0; i < MAX_SALES; ++i) {
		index[i] = i;
	}
	sales_update();
	while (1) {
		//system("cls");
		greet_user_and_print_menu();

		if (!count) {
			printf("Error, please try again");
			clear_screen();
			continue;
		}

		if (userAnswer == 'a') {
			print_cars();
			//printf("%s", userName[numberOfSales]);

			//system("cls");

			if (customerAge < 16) {
				printf("You're not allowed to drive a car. Why you want to buy it? Come back with parents!\n\n");
			}
			else {
				print_discount_applying();

				buying_process();
			}
		}
		else if (userAnswer == 'b') {
			print_statistics();
		}
		else if (userAnswer == 'c') {
			printf("Have a nice day, Good bye\n");
			break;
		}
		else {
			printf("Error, try again later\n");
		}
		//sleep(2);
		clear_screen();
		sales_update();
	}
	//print to file
	saveDataToFile();
	closeFile();
	if (fileStatus != FILE_CLOSED) {
		printf("error");
	}
	//print("%c", fileStatus);
	return 0;
}
