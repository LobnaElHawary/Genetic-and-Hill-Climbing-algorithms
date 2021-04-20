#include <iostream>
#include <Windows.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include <map>
#include <fstream>
#include <string>

using namespace std;

#define CityNum			9	//Num of cities to randomly pick from
#define FULLPATH		11  //from Amsterdam back to Amsterdam
#define MutationRate	35	//Later used for random mutation
#define PARENTCOUNT		2	//Num of parents


int maxIter;				//How many generations/iterations
int POP_SIZE;				//Population size

struct Member {
	int DNA[FULLPATH];
	int Fitness;
};

struct Population {
	//Population of size POP_SIZE
	vector<Member> Members = vector<Member>(POP_SIZE);
};


/* --------------- FUNCTIONS ------------------*/

string num_to_city(int num);							//converting from integer to city's name for fitness function
int fitness(int array[FULLPATH]);						//calculating the fitness

//For Genetic:
void checkRepeats(int DNA[CityNum + 2], Member &temp);				//to avoid and fix any repeats
void crossOver(Member& Child, Member Parent1, Member Parent2);		//Crossing over and random mutations
void genetic();

//For Hill-climbing:
void unique_random(int first, int last, vector<int>& numbers);
void random_array(vector<int>& test);
void create_state(int state[]);
void print_state(int state[]);
void next_state(int state[], int new_state[]);
void equate_states(int state_1[11], int state_2[11]);
void hillClimb();

/* --------------- FUNCTIONS ------------------*/



int main() {

	int choice;
	cout << "Enter the algorithim you'd like used\t 0) Genetic, 1) Hill-climbing: \n";
	cin >> choice;

	while (choice != 0 && choice != 1) {
		cout << "Incorrect input, try again...";
		cin >> choice;
	}



	if (choice == 0)
		genetic();

	else if (choice == 1)
		hillClimb();

	else
		;

	return 0;
}





//changing the integers to their city representations to be used in fitness function
string num_to_city(int num)
{
	if (num == 0)
	{
		return "Amsterdam";
	}
	if (num == 1)
	{
		return "Berlin";
	}
	if (num == 2)
	{
		return "Bern";
	}
	if (num == 3)
	{
		return "Budapest";
	}
	if (num == 4)
	{
		return "Madrid";
	}
	if (num == 5)
	{
		return "Paris";
	}
	if (num == 6)
	{
		return "Prague";
	}
	if (num == 7)
	{
		return "Rome";
	}
	if (num == 8)
	{
		return "Vienna";
	}
	if (num == 9)
	{
		return "Warsaw";
	}

}

int fitness(int array[FULLPATH])
{
	string file_name;
	//cout << "enter filename you would like to be used" << endl;
	//cin >> file_name;
	file_name = "distances.txt";

	int position_1, position_2;
	int cost = 0;
	string line, city_1, city_2, cost_str;

	// traverse through array
	for (int i = 0; i < 10; i = i + 1)
	{
		string city_1, city_2;
		string file_city_1, file_city_2, cost_str;
		int file_cost;

		city_1 = num_to_city(array[i]);
		city_2 = num_to_city(array[i + 1]);

		//open file and look for path from city_1 to city_2 and add that to cost
		ifstream temp_file(file_name);
		if (temp_file.is_open())
		{
			while (getline(temp_file, line))
			{
				position_1 = line.find_first_of(" ");
				position_2 = line.find_last_of(" ");

				file_city_1 = line.substr(1, position_1 - 1); //city_1's name
				file_city_2 = line.substr(position_1 + 1, (position_2 - position_1) - 1);

				//
				if ((file_city_1 == city_1) && (file_city_2 == city_2) || (file_city_1 == city_2) && (file_city_2 == city_1))
				{

					cost_str = line.substr(position_2 + 1, line.length() - 1);
					file_cost = stoi(cost_str);
					cost = file_cost + cost;
				}
			}
		}

		else
			cout << "File did not open." << endl;

		temp_file.close();
	}

	return cost;

}


//mutates further if there are any repeats
void checkRepeats(int DNA[CityNum + 2], Member &temp) {
	bool checker[CityNum + 1];
	int check[CityNum + 1];

	//initiliazing both arrays
	for (int i = 1; i <= CityNum; i++) {
		checker[i] = FALSE;
		check[i] = 0;
	}


	//putting all the cities into a boolean array as TRUE when used
	//increment based on how many times visited
	for (int i = 1; i <= CityNum; i++) {
		checker[DNA[i]] = TRUE;
		check[DNA[i]]++;
	}


	//to start checking and mutating
	for (int i = 1; i <= CityNum; i++) {

		//if it was already visited many times
		if (checker[DNA[i]] == TRUE && check[DNA[i]] > 1) {

			//then we have to mutate it based on what's FALSE
			for (int j = 1; j <= CityNum; j++) {
				if (checker[j] == FALSE) {
					check[DNA[i]]--;

					DNA[i] = j;
					temp.DNA[i] = j;
					checker[j] = TRUE;
					check[j]++;

					break;
				}
			}

		}
	}
}


void crossOver(Member& Child, Member Parent1, Member Parent2)
{
	//to see how much of the parent the child will take
	int p = rand() % CityNum + 1;

	//to see which parent will go first
	int choose = rand() % 2;
	Member First;
	Member Second;

	if (choose == 1) {
		First = Parent1;
		Second = Parent2;
	}

	else { //choose is 0
		First = Parent2;
		Second = Parent1;
	}

	//selecting the DNA from parents to put into children
	//involves random mutation
	for (int i = 1; i < p; i++) {
		Child.DNA[i] = First.DNA[i];

		//Random mutation, with 35% chance
		if (rand() % 100 < MutationRate)
			Child.DNA[i] = rand() % CityNum + 1;
	}

	for (int i = p; i < CityNum + 1; i++) {
		Child.DNA[i] = Second.DNA[i];

		//Random mutation
		if (rand() % 100 < MutationRate)
			Child.DNA[i] = rand() % CityNum + 1;
	}

}

void genetic()
{
	cout << "Enter the population size: \n";
	cin >> POP_SIZE;
	cout << "Enter the number of iterations: \n";
	cin >> maxIter;

	int DNA[FULLPATH];		//stores the sequence of cities
	int Generation = 0;		//Generations' count

	srand(time(NULL));		//for better randomness


	//create a population and initialize it with random DNA, and set their fitness to 0
	//also makes sure that the random DNA is valid (by checking for repeats)
	Population Pop;
	for (int i = 0; i < Pop.Members.size(); i++) {
		for (int j = 1; j <= CityNum; j++)
			Pop.Members.at(i).DNA[j] = rand() % CityNum + 1;
		checkRepeats(Pop.Members.at(i).DNA, Pop.Members.at(i));
		Pop.Members.at(i).Fitness = 0;
	}

	//create a parents vector and make space of size PARENTCOUNT for it
	vector<Member> Parents;
	for (int z = 0; z < PARENTCOUNT; z++)
		Parents.push_back(Pop.Members.at(z));



	//while we haven't finished iterating
	while (Generation < maxIter) {
		Generation++;

		//calculating the fitness for each member
		for (int i = 0; i < Pop.Members.size(); i++)
			Pop.Members.at(i).Fitness = fitness(Pop.Members.at(i).DNA);

		//sorting based on Fitness
		sort(Pop.Members.begin(), Pop.Members.end(), [](Member const& a, Member& b) {
			return a.Fitness < b.Fitness;
			});

		//printing current generation
		cout << "\n ------------------ Generation " << Generation << " ------------------\n";
		for (int i = 0; i < Pop.Members.size(); i++) {
			cout << "Sequence: ";
			for (int j = 0; j <= CityNum + 1; j++)
				cout << Pop.Members.at(i).DNA[j] << " ";
			cout << "\nFitness: " << Pop.Members.at(i).Fitness << endl;
		}

		//select top z to be parents to pair/mate
		for (int z = 0; z < PARENTCOUNT; z++)
			Parents.at(z) = Pop.Members.at(z);

		//also the top s% move over to the next generation: Elitism
		int s = (5 * POP_SIZE) / 100;

		//Making a new generation
		for (int i = s; i < Pop.Members.size(); i++) {

			//crossing over and mutating
			crossOver(Pop.Members.at(i), Parents.at(0), Parents.at(1));

			//to fix the path if the operations invalidated the path by including repeats
			checkRepeats(Pop.Members.at(i).DNA, Pop.Members.at(i));
		}

		cout << "\nGeneration: " << Generation << "\nHighest Fitness: " << Parents.at(0).Fitness << "\nWith Sequence: ";
		for (int i = 0; i <= 10; i++)
			cout << Parents.at(0).DNA[i] << " ";
		cout << endl;

	}

	cout << "\n\n ---------- Finished the " << maxIter << " Generations ----------\n\n";

	//Printing the final output
	cout << "Final Path: \n";

	for (int i = 0; i < FULLPATH; i++)
		cout << " " << num_to_city(Parents.at(0).DNA[i]);
	cout << ". \tFitness: " << Parents.at(0).Fitness << endl;
	Sleep(2000);
}


//Old Crossover, just in case.
/*for (int j = 1; j <= CityNum; j++) {

	//Random Crossover
	//randomizing which of the parents to be used
	int TempSelection = rand() % Parents.size();
	Pop.Members.at(i).DNA[j] = Parents.at(TempSelection).DNA[j];

}*/



void unique_random(int first, int last, vector<int>& numbers)
{
	int swapIndex;
	for (int i = first; i <= last; i++)
	{
		numbers.push_back(i);
	}

	for (int i = 0; i < numbers.size(); i++)
	{
		swapIndex = rand() % numbers.size();
		swap(numbers[i], numbers[swapIndex]);
	}
}

void random_array(vector<int>& test)
{
	srand((unsigned int)time(NULL));
	unique_random(1, 9, test);
}


void create_state(int state[])
{


	vector<int>test;
	random_array(test);

	state[0] = 0;
	for (int i = 1; i < 10; i++)
	{
		state[i] = test[i - 1];
	}
	state[10] = 0;


}

void print_state(int state[])
{

	for (int i = 0; i < 11; i++)
	{
		cout << state[i] << " ";
	}
}

//done by swapping any random 2 cities (excluding the starting and ending city)
void next_state(int state[], int new_state[])
{
	for (int i = 0; i < 11; i++)
	{
		new_state[i] = state[i];
	}

	int temp;
	int random_1 = rand() % 9 + 1;
	int random_2 = rand() % 9 + 1;

	//generate 2 distinct random values from 1 - 9
	while (random_2 == random_1)
	{
		random_2 = rand() % 9 + 1;
	}

	//swap
	temp = new_state[random_1];
	new_state[random_1] = new_state[random_2];
	new_state[random_2] = temp;

}

//state1 = state2
void equate_states(int state_1[11], int state_2[11])
{
	for (int i = 0; i < 11; i++)
	{
		state_1[i] = state_2[i];
	}
}

void hillClimb() {
	int fitness_current, fitness_new, iterations;

	int initial_state[FULLPATH];
	int current_state[FULLPATH];
	int new_state[FULLPATH];

	string file_name = "distances.txt";
	//cout << "Enter the name of the file where the paths and the distances are" << endl;
	//cin >> file_name;
	cout << "Enter the number of iterations you would like to run: " << endl;
	cin >> iterations;

	//create first state and find its fitness
	create_state(initial_state);
	cout << endl;
	cout << "The initial state is: " << endl;
	print_state(initial_state);
	// initial_state = current_state
	equate_states(current_state, initial_state);
	cout << " " << fitness(current_state) << endl;
	cout << endl;
	fitness_current = fitness(current_state);


	for (int i = 0; i < iterations; i++)
	{
		cout << "--------------------------- \nIteration number: " << i + 1 << endl;
		next_state(initial_state, new_state);
		cout << "New state: " << endl;
		print_state(new_state);
		cout << " " << fitness(new_state) << endl << endl;
		fitness_new = fitness(new_state);

		//if new_state has better fitness than current_state's fitness make it the new current
		if (fitness_new < fitness_current)
		{
			//current_state = new_state
			equate_states(current_state, new_state);
			fitness_current = fitness_new;
		}

		cout << "Current state: " << endl;
		print_state(current_state);
		cout << " " << fitness_current << "\n";
		cout << endl;
	}

	//Printing the final output
	cout << "Final Path: \n";

	for (int i = 0; i < FULLPATH; i++)
		cout << " " << num_to_city(current_state[i]);
	cout << ". \tFitness: " << fitness_current << endl;

}