//Nooa Jutila, toivottavasti 5p
#include<iostream>
#include<cstdlib>
#include<ctime>
#include<cmath>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<random>
using namespace std;

int RoomAmount();
vector<double> Discount(double Cost);
vector<int> RoomsAvailable(int RoomAmount, vector<vector<int>> RoomInfo);
vector<int> Booking(int PeopleAmount, vector<int> RoomsAvailable);
int ErrorChecker(int answer);
void RoomInfoToFile(vector<vector<int>> RoomInfo, int CustomerNumber, vector<string> ReservationName, int RoomsTotal);
static bool HotelHasSpace(int Room1Amount, int Room2Amount, vector<int> RoomsAvailable);
vector<vector<int>> InitializeRoomInfo(int RoomsTotal);

//Struct for RoomInfoToFile
struct RoomInfoStruct {
	vector<vector<int>> RoomInfo_s;  //2D vector to store room information
	vector<string> CustomerNames_s;  //1D vector to store customer names
	int RoomsTotal_s = 0;            //Integer to store number of rooms
};

//Reads from file named "RoomInfo.txt" all the data regarding the occupation of the rooms
static RoomInfoStruct RoomInfoFromFile() {
	vector<string> CustomerNames;  // To store customer names
	vector<vector<int>> RoomInfo;  // 2D vector to store room information
	int lineNumber = 0;			   // Saves current line being read
	string data;
	RoomInfoStruct result2;
	vector<int> RoomData;

	//Opening the file
	ifstream inFile("RoomInfo.txt");

	//Reading the customer names from the file (every 4th line)
	while (getline(inFile, data)) {
		lineNumber++;

		// Process every 3rd line as room data (1 integer per line)
		if (lineNumber % 4 != 0) {
			// Convert the string data to an integer
			stringstream ss(data);
			int roomValue;
			ss >> roomValue;

			// Store the room value in RoomData
			RoomData.push_back(roomValue);

			// If we have 3 values in RoomData, push it to RoomInfo
			if (RoomData.size() == 3) {
				RoomInfo.push_back(RoomData);
				RoomData.clear();  // Clear for the next room's data
			}
		}
		else {
			// Every 4th line is a customer name
			CustomerNames.push_back(data);
		}
	}

	const int RoomsTotal = CustomerNames.size();

	// Closing the file
	inFile.close();

	// Returning the read values
	result2.RoomInfo_s = RoomInfo;
	result2.CustomerNames_s = CustomerNames;
	result2.RoomsTotal_s = RoomsTotal;

	return result2;
}

struct Results {
	string Name;
	vector<int> AmountRoomSizes;
};

//Contains a majority of the UI of the hotel and returns the amount of rooms that have been booked and the name of the booker
static Results HotelSystem(int RoomsTotal, vector<int> RoomsAvailable_main) {
	string Name;
	int answer = 2;
	int PeopleAmount;
	int cost = 0;
	int AmountRoomSize1 = 0;
	int AmountRoomSize2 = 0;

	//Begin menu
	std::cout << "Hello. Currently, there are a total of " << RoomsAvailable_main[0] << " rooms available for reservation.\nOf these rooms, " << RoomsAvailable_main[1] << " can house one person, and " << RoomsAvailable_main[2] << " can house two." << endl;
	std::cout << "What name will these rooms be booked under? ";
	std::getline(std::cin, Name);

	//Making sure name is right
	std::cout << "Is this the right name: " << Name << "\nYes [1]\nNo, return [2]\n";
	std::cin >> answer;
	while (std::cin.fail() || (answer != 1 && answer != 2)) {
		std::cin.clear();
		std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
		std::cout << "Please answer with either 1 or 2.";
		std::cout << "\nIs this the right name: " << Name << "\nYes [1]\nNo, return [2]\n";
		std::cin >> answer;
	}

	//Asking to re-enter the name
	while (answer == 2) {
		std::cout << "Please re-enter the name you want to book under: ";
		std::cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');	//Clears the stream because funny business deletes Name for this loop otherwise
		std::getline(cin, Name);
		std::cout << "Is this the right name: " << Name << "\nYes [1]\nNo, return [2]\n";
		std::cin >> answer;

		//Asking again if number was incorrect
		while (std::cin.fail() || (answer != 1 && answer != 2)) {
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
			std::cout << "Please answer with either 1 or 2.";
			std::cout << "\nIs this the right name: " << Name << "\nYes [1]\nNo, return [2]\n";
			std::cin >> answer;
		}
	}

	//Asking for amount of people staying
	std::cout << "How many people will be staying? ";
	std::cin >> PeopleAmount;
	while (std::cin.fail() || PeopleAmount < 1) {
		std::cin.clear();
		std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); //Ignores previous input/leftovers
		std::cout << "Please answer with a number.\n";
		std::cin >> PeopleAmount;
	}

	//Calculating if hotel has capacity for these people
	int CurrentCapacity = RoomsAvailable_main[1] + ((RoomsAvailable_main[2] * 2));	//1 person in [1] and 2 people in [2].
	if (CurrentCapacity < PeopleAmount) {
		std::cout << "We currently have room for " << CurrentCapacity << ". We cannot book " << PeopleAmount << " visitors.";
		exit(0);
	}

	//Calling Booking() to start the booking and
	//Saving the returned amount of rooms that were booked
	vector<int> BookingVector = Booking(PeopleAmount, RoomsAvailable_main);
	cost = BookingVector[0];

	//indx 0 = amount of room size 1 booked, indx 1 = amount of room size 2 booked
	vector<int> AmountRoomSizes{ BookingVector[1], BookingVector[2] };
	Results result;
	result.Name = Name;
	result.AmountRoomSizes = AmountRoomSizes;

	return result;
}

//Main is for handling data saving and calling functions
int main() {
	bool restart = true;
	int answer;
	int DataAnswer;
	int RoomsTotal = 0;
	vector<vector<int>> RoomInfo;
	vector<string> ReservationName;
	int CustomerNumber;

	cout << "Would you like to [1] continue with old data or [2] reset the hotel system? ";
	cin >> DataAnswer;
	DataAnswer = ErrorChecker(DataAnswer);

	//Call RoomInfoFromFile() to use previous variables
	if (DataAnswer == 1) {
		RoomInfoStruct OldData = RoomInfoFromFile();
		ReservationName = OldData.CustomerNames_s;
		RoomInfo = OldData.RoomInfo_s;
		RoomsTotal = OldData.RoomsTotal_s;
	}

	//Set RoomsTotal to a random value from RoomAmount(), initialize RoomInfo and ReservationName
	else if (DataAnswer == 2) {
		RoomsTotal = RoomAmount();	//Amount of rooms in hotel
		RoomInfo = InitializeRoomInfo(RoomsTotal);
		for (int i = 0; i < RoomsTotal; i++) {
			ReservationName.push_back("Unreserved");
		}
		RoomInfoToFile(RoomInfo, 0, ReservationName, RoomsTotal);
	}

	
	//indx 0 = Free total, indx 1 = Free size 1, indx 2 = Free size 2
	vector<int> RoomsAvailable_main = RoomsAvailable(RoomsTotal, RoomInfo);

	while (restart == true) {
		//Here is the start of the main program
		std::cout << "\n[1] Search for current residents or [2] Book new residents? ";
		std::cin >> answer;
		answer = ErrorChecker(answer);

		
		//Searching the data for residents
		bool restartlookup = true;
		while (restartlookup == true) {

			if (answer == 1) {
				std::cin.clear();
				std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
				std::cout << "Would you like to search [1] by the name or [2] by the booking number?\n";
				std::cin >> answer;
				answer = ErrorChecker(answer);
				std::cout << endl;

				//Searching with a name
				if (answer == 1) {
					int RoomsUnderThisName = 0;
					std::cin.clear();
					std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
					string NameSearch;

					//Asking for the name of the resident
					std::cout << "Please give the name you want to search: ";
					std::getline(std::cin, NameSearch);

					//Calculating how many rooms are under that name
					for (int i = 0; i < RoomsTotal; i++) {
						if (ReservationName[i] == NameSearch)
							RoomsUnderThisName++;
					}

					//Answering with the room numbers
					std::cout << "\nThere are " << RoomsUnderThisName << " rooms reserved under that name.\n";
					if (RoomsUnderThisName > 0) {
						std::cout << "These rooms are:\n";
						for (int i = 0; i < RoomsTotal; i++) {
							if (ReservationName[i] == NameSearch) {
								std::cout << "Room " << i + 1 << ". " << endl;
							}
						}
					}
				}

				//Searching with a number
				if (answer == 2) {
					std::cin.clear();
					std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
					int CustomerNumber;

					//Asking for the Customer Number
					std::cout << "Please give the number you want to search: ";
					std::cin >> CustomerNumber;

					//Validating input
					while (std::cin.fail() || (CustomerNumber > 99999) || (CustomerNumber < 10000)) {
						std::cin.clear();
						std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
						std::cout << "Please enter a valid customer number between 10 000 - 99 999. ";
						std::cin >> CustomerNumber;
					}

					//Answering with rooms
					int RoomsUnderThisNumber = 0;

					//Calculating how many rooms are with that number
					for (int i = 0; i < RoomsTotal; i++) {
						if (RoomInfo[i][2] == CustomerNumber)
							RoomsUnderThisNumber++;
					}

					//Reporting the rooms with that number
					std::cout << "\nThere are " << RoomsUnderThisNumber << " rooms under that number.\n";
					if (RoomsUnderThisNumber > 0) {
						std::cout << "These rooms are:\n";
						for (int i = 0; i < RoomsTotal; i++) {
							if (RoomInfo[i][2] == CustomerNumber) {
								std::cout << "Room " << i + 1 << ". " << endl;
							}
						}
					}
				}
			}
			std::cout << "Look up more residents? \n[1] Yes\n[2] No\n";
			cin >> answer;
			answer = ErrorChecker(answer);
			if (answer == 1)
				continue;
			if (answer == 2)
				restartlookup = false;
			answer = 1;		//Sends the program over the next section, over to its end where the program normally ends after asking if they want to book more rooms, only if the while loop ends because user answered "2".
		}

		//This is where the main program begins
		//Booking begins
		if (answer == 2) {
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');

			//Calling HotelSystem() which calls Booking() etc. and storing the returned values using a struct
			Results Roomstaken = HotelSystem(RoomsTotal, RoomsAvailable_main);
			string Name = Roomstaken.Name;

			//indx 0 = amount of room size 1 booked, indx 1 = amount of room size 2 booked
			vector<int> RoomsBooked = Roomstaken.AmountRoomSizes;

			//Making a customer number

			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<> distrib(10000, 99999);

			CustomerNumber = distrib(gen);
			std::cout << "\nThe customer number for this booking is " << CustomerNumber << endl;

			//Storing information

			int Room1booked = 0;
			int Room2booked = 0;

			//Rooms of size 1
			for (int i = 0; i < RoomsTotal; i++) {
				if (RoomsBooked[0] > 0) {
					//If we haven't booked the necessary amount of rooms, true
					while (Room1booked < RoomsBooked[0]) {
						//If room is unoccupied and size 1, true
						if (RoomInfo[i][1] == 0 && RoomInfo[i][0] == 1) {
							//Marking room number i as taken
							RoomInfo[i][1] = 1;
							//Giving it the customer number
							RoomInfo[i][2] = CustomerNumber;
							//At the same index, meaning for the same room, store the name in a separate vector for strings.
							ReservationName[i] = Name;
							++Room1booked;
						}
						// If no more available rooms of size 1, break the loop
						if (RoomInfo[i][0] != 1 || RoomInfo[i][1] != 0) {
							break;
						}
					}
				}
			}
			//Rooms of size 2
			for (int i = RoomsTotal/2; i < RoomsTotal; i++) {
				if (RoomsBooked[1] > 0) {
					//If we haven't booked the necessary amount of rooms, true
					while (Room2booked < RoomsBooked[1]) {
						//If room is unoccupied and size 2, true
						if (RoomInfo[i][1] == 0 && RoomInfo[i][0] == 2) {
							//Marking room number i as taken
							RoomInfo[i][1] = 1;
							//Giving it the customer number
							RoomInfo[i][2] = CustomerNumber;
							//At the same index, meaning for the same room, store the name in a separate vector for strings.
							ReservationName[i] = Name;
							++Room2booked;
						}
						if (RoomInfo[i][0] != 2 || RoomInfo[i][1] != 0) {
							break;
						}
						// Optionally: Check if all rooms have been iterated over
						if (i == RoomsTotal - 1) {
							break; // Stop looping if all rooms are checked
						}
					}
				}
			}
			//Sending data to RoomInfoToFile to be written
			RoomInfoToFile(RoomInfo, CustomerNumber, ReservationName, RoomsTotal);
		}
		//This is where it loops or ends
		int UserRestart;
		std::cout << "\nWould you like to book more rooms?\nYes [1]\nNo[2]\n";
		std::cin >> UserRestart;
		UserRestart = ErrorChecker(UserRestart);
		if (UserRestart == 1) {
			restart = true;
			RoomInfoStruct OldData = RoomInfoFromFile();
			ReservationName = OldData.CustomerNames_s;
			RoomInfo = OldData.RoomInfo_s;
			RoomsTotal = OldData.RoomsTotal_s;
		}
		else if (UserRestart == 2)
			restart = false;
		std::cin.clear();
		std::cin.ignore(numeric_limits<streamsize>::max(), '\n');

	}
}

//Calculates random amount of rooms
int RoomAmount() {
	//srand initialization
	srand(time(NULL));
	//Value starts at 0
	static int RandRooms = 0;
	//So that this program only returns one random number per time it's called.
	if (RandRooms == 0) {
		//random number between 40 and 300
		RandRooms = 40 + rand() % 261;
		//checking if it's odd, and if so, add 1.
	//299 is odd, add one, 300. 298 is fine and so is 300. it won't leave my range
		if (RandRooms % 2 != 0)
			RandRooms++;
	}
	return RandRooms;
}

//Returns the randomly discounted price, and the discount
vector<double> Discount(double price) {
	//Discount, formatted as in 0.0-0.3
	double randomnumber = ((rand() % 3) / (10.0));
	//Discount, formatted as in price*0.9
	double disc = (1 - randomnumber);
	//Discount, formatted for display, like: 10
	double dispdisc = randomnumber * 100;
	vector<double> Discounted = { price * disc, dispdisc };
	return Discounted;
}

//Calculates amount of rooms available and returns a vector {FreeTotal, FreeSize1, FreeSize2}
vector<int> RoomsAvailable(int RoomAmount, vector<vector<int>> RoomInfo) {
	int FreeTotal;
	int FreeSize1 = 0;
	int FreeSize2 = 0;
	//Count all free rooms
	for (int i = 0; i < RoomAmount; i++) {
		if (RoomInfo[i].size() >= 2) {
			//RoomInfo(i,1) == 0 if free and (i,0) == 1 if size is 1
			if (RoomInfo[i][1] == 0 && RoomInfo[i][0] == 1)
				FreeSize1++;
			//RoomInfo(i,1) == 0 if free and (i,0) == 2 if size is 2
			if (RoomInfo[i][1] == 0 && RoomInfo[i][0] == 2)
				FreeSize2++;
		}
		else {
			cout << "Warning: Room " << i << " data is incomplete.";
		}
	}
	FreeTotal = FreeSize1 + FreeSize2;
	vector<int> FreeRooms{ FreeTotal, FreeSize1, FreeSize2 };
	return FreeRooms;
}

//Checks if answer is a valid input of either 1 or 2.
int ErrorChecker(int answer) {
	while (std::cin.fail() || answer != 1 && answer != 2) {
		std::cin.clear();
		std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); //Ignores input
		std::cout << "Please answer with 1 or 2.\n";
		std::cin >> answer;
	}
	return answer;
}

//Gives some booking options and returns the cost of the booking. || Indexes: (0=Cost of booking, 1=amount of room size 1 booked, 2=amount of room size 2 booked)
vector<int> Booking(int PeopleAmount, vector<int> RoomsAvailable) {
	int cost = 0;
	int answer = 2;
	int RoomSize1Amount = 0;
	int RoomSize2Amount = 0;
	//Making the vector that Booking() will return
	vector<int> BookingVector{ cost, RoomSize1Amount, RoomSize2Amount };	//Indexes: (0=Cost of booking, 1=amount of room size 1 booked, 2=amount of room size 2 booked)
	//Giving some booking options.
	
	//If there is an even amount of guests
	if ((PeopleAmount % 2 == 0) && (PeopleAmount > 2)) {
		std::cout << "Would you rather reserve [1] " << PeopleAmount / 2 << " rooms for two or [2] " << PeopleAmount << " single person rooms?\n";
		std::cin >> answer;
		answer = ErrorChecker(answer);

		//Calculating cost of booking
		if (answer == 1) {
			cost = (PeopleAmount / 2) * 150;
			RoomSize2Amount += (PeopleAmount / 2);
		}
		else if (answer == 2){
			cost = PeopleAmount * 100;
			RoomSize1Amount += PeopleAmount;
		}
	}

	//If there is an odd amount of guests
	else if ((PeopleAmount % 2 == 1) && (PeopleAmount > 2)) {
		std::cout << "Would you rather reserve [1] " << (PeopleAmount - 1) / 2 << " room(s) for two and 1 single person room or [2] " << PeopleAmount << " single person rooms?\n";
		std::cin >> answer;
		answer = ErrorChecker(answer);

		//Calculating cost of booking
		if (answer == 1) {
			cost = (((PeopleAmount - 1) / 2) * 150) + 100;
			RoomSize1Amount += 1;
			RoomSize2Amount += ((PeopleAmount - 1) / 2);
		}
		else if (answer == 2) {
			cost = PeopleAmount * 100;
			RoomSize1Amount += PeopleAmount;
		}
	}

	//If there are two guests
	else if (PeopleAmount == 2) {
		std::cout << "Would you rather reserve [1] one room for two or [2] two single person rooms?\n";
		std::cin >> answer;
		answer = ErrorChecker(answer);

		//Calculating cost of booking
		if (answer == 1) {
			cost = (PeopleAmount / 2) * 150;
			RoomSize2Amount += (PeopleAmount / 2);
		}
		else if (answer == 2) {
			cost = PeopleAmount * 100;
			RoomSize1Amount += PeopleAmount;
		}
	}

	//If there is one guest
	else if (PeopleAmount == 1) {
		std::cout << "Would you like to reserve one single person room?\nYes [1]\nNo, return [2]\n";
		std::cin >> answer;
		answer = ErrorChecker(answer);

		//Calculating cost of booking
		if (answer == 1) {
			cost = PeopleAmount * 100;
			RoomSize1Amount += 1;
		}
		else if (answer == 2) {
			std::cout << "Then why did you even come here? You can sleep outside.";
			exit(0);
		}
	}
	
	//Making sure there is enough space.
	if (HotelHasSpace(RoomSize1Amount, RoomSize2Amount, RoomsAvailable)) {

		//Asking how many nights they want to stay (not actually stored in the RoomInfo, just here to modify the final cost.
		//Apparently was a requirement for full points even though I asked if it was, and got told no.

		int nights = 1;
		std::cout << "For how many nights do you want to book the rooms? (1-31)\n";
		std::cin >> nights;
		while (nights < 1 || nights > 31 || cin.fail()) {
			std::cin.clear();
			std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
			std::cout << "Please answer with a number between 1 and 31. ";
			std::cin >> nights;
		}

		//Calling Discount() and assigning the returned vector into costvector
		vector<double> costvector = Discount(cost*nights);

		//Confirming booking
		std::cout << "The booking comes to " << cost*nights << " euros. You will receive a " << costvector[1] << " percent discount,\nmaking your total " << costvector[0] << " euros." << endl;
		std::cout << "Would you like to confirm the booking ? \nYes[1]\nNo, return[2]\n";
		std::cin >> answer;
		answer = ErrorChecker(answer);

		if (answer == 1) {
			std::cout << "Booking succesfully made.";

			//Updating all values before returning the vector
			BookingVector[0] = cost;
			BookingVector[1] = RoomSize1Amount;
			BookingVector[2] = RoomSize2Amount;
			return BookingVector;
		}

		else if (answer == 2) {
			RoomSize1Amount = 0;
			RoomSize2Amount = 0;
			std::cout << "Would you like to [1] select another booking option or [2] book under a new name? ";
			std::cin >> answer;
			answer = ErrorChecker(answer);
			if (answer == 1) {
				Booking(PeopleAmount, RoomsAvailable);
			}
			else;
		}
	}
	
	//No space, offering return to start
	else {
		cout << "There isn't enough space for your booking. ";
		RoomSize1Amount = 0;
		RoomSize2Amount = 0;
		std::cout << "Would you like to [1] select another booking option or [2] book under a new name? ";
		std::cin >> answer;
		answer = ErrorChecker(answer);
		if (answer == 1) {
			Booking(PeopleAmount, RoomsAvailable);
		}
		else;
	}
}

//Writes to file named "RoomInfo.txt" all the data inside RoomInfo and as a "third index" in the file RoomInfo, it writes the name of the booked room.
//File syntax:
// Room size (1 or 2)
// Room occupation (1=occupied, 0=unoccupied)
// Customer number
// Customer name
void RoomInfoToFile(vector<vector<int>> RoomInfo, int CustomerNumber, vector<string> ReservationName, int RoomsTotal) {
	std::ofstream outFile("RoomInfo.txt");
	for (int i = 0; i < RoomsTotal; i++) {
		for (int j = 0; j < 3; j++) {
			outFile << RoomInfo[i][j];
			outFile << endl;
		}
		outFile << ReservationName[i];
		outFile << endl;
	}
	outFile.close();
}

//Returns true if there is enough room. RoomXAmount wants the amount of rooms about to be booked.
static bool HotelHasSpace(int Room1Amount, int Room2Amount, vector<int> RoomsAvailable) {
	if ((Room1Amount > RoomsAvailable[1]) || (Room2Amount > RoomsAvailable[2]))
		return false;
	else
		return true;
}

vector<vector<int>> InitializeRoomInfo(int RoomsTotal) {
	vector<vector<int>> RoomInfo(RoomsTotal, vector<int>(3));//[size(1 or 2),taken(1 yes, 0 no), customer number
	//Initializing matrix
	for (int i = 0; i < RoomsTotal; ++i) {
		if (i < (RoomsTotal / 2))	//First half of rooms fit 1 person
			RoomInfo[i][0] = 1;
		else if (!(i < (RoomsTotal / 2)))
			RoomInfo[i][0] = 2;	//Second half of rooms fit 2 people
		RoomInfo[i][1] = 0;
		RoomInfo[i][2] = 0;
	}
	return RoomInfo;
}