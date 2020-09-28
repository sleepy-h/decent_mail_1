#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>

#define RAND_SEEK 0xDEADC0DE

// struct for parameters 
typedef struct{
	uint64_t number_of_bilets;
	int64_t parameter_for_generator;
	FILE* file_with_students;
} parameters;

//how much now whitespaced strings in file
uint64_t fcheck_not_whitespace_str(FILE* fileptr){
	uint64_t result = 0;
	while(!feof(fileptr)){
		while(isspace(fgetc(fileptr)) && !feof(fileptr));
		++result;
		while(((fgetc(fileptr)) != '\n')  && !feof(fileptr));
	}
	fseek(fileptr, 0L, SEEK_SET);
	return result;
}

//Initializator for parameters structure and processing parameters from command line
parameters* create_parameters(int argc, char const *argv[]){
	if (argc == 1){
        //Error: program without arguments
		return NULL;
	}
	parameters* parameter = (parameters *)malloc(sizeof(parameters));
	parameter->number_of_bilets = (uint64_t)0;
    parameter->parameter_for_generator = 0;
	for (int i = 1; i < (argc - 1); i+=2){
		if (argv[i][0] != '-'){
			// Error: Wrong command
			return NULL;
		} else {
			switch(argv[i][1]){
				case 'n':
					//if (strlen(argv[i]) == 2){
					if (argv[i][2] == '\0'){
						parameter->number_of_bilets = atoi(argv[i+1]);
					} else {
						// Error: Wrong command
						return NULL;
					}
					break;
				case 'g':
					//if (strlen(argv[i]) == 2){
					if (argv[i][2] == '\0'){
						parameter->parameter_for_generator = atoi(argv[i+1]);
					} else {
						// Error: Wrong command
						return NULL;
					}
					break;
				default:
					// Error: Wrong command
					return NULL;
					break;
			}
		}
	}
	parameter->file_with_students = fopen(argv[argc-1],"r");
	if (parameter->file_with_students == NULL)
		// Error: File is not created
		return NULL;
	if (parameter->number_of_bilets == 0){
		parameter->number_of_bilets = fcheck_not_whitespace_str(parameter->file_with_students);
    }
	if (parameter->number_of_bilets == 0)
		// Error: File is empty or whitespaced
		return NULL;
	return parameter;
}

// clear all dynamitic memory  in parameters
void destruct_parameters(parameters* parameters){
	fclose(parameters->file_with_students);
	free(parameters);
}

// function for write help
void help(char const* name_of_programm){
	printf("Usage: %s [options] file...\n", name_of_programm);
    printf("Options:\n");
    printf("  -g                       Parameters for pseudo generation random.\n");
    printf("  -n                       Quantity of bilets.\n");
}

int main(int argc, char const *argv[]){
	//Processing parameters
	parameters* parameters = create_parameters(argc,argv);
	if (parameters == NULL){
		help(argv[0]);
		return 0;
	}

	//Create list numbers status
	bool* numbers_status = (bool*)calloc(parameters->number_of_bilets, sizeof(bool));

	//Initialization random
	srand(RAND_SEEK + parameters->parameter_for_generator);
	
	// bool for optimization search not used number of bilets
	bool status_now = true;

	while(!feof(parameters->file_with_students)){
		for(uint64_t i = 0; i < parameters->number_of_bilets && !feof(parameters->file_with_students); ++i){
			char current_symbol;
			bool previous_is_space = false;
			while(isspace(current_symbol = fgetc(parameters->file_with_students)) && current_symbol != EOF);
			putchar(current_symbol);
			bool has_str = false;
			while(((current_symbol = fgetc(parameters->file_with_students)) != '\n')  && current_symbol != EOF){
				if (isspace(current_symbol)){
					previous_is_space = true;
				} else { 
					if (previous_is_space) {
						putchar(' ');
						previous_is_space = false;
					}
					putchar(current_symbol);
					has_str = true;
				}
			}
			if(has_str){
				uint64_t j = rand() % parameters->number_of_bilets;
				for(;numbers_status[j] == status_now; j = (j + 1) % parameters->number_of_bilets);
				numbers_status[j] = status_now;
				printf(": %" PRIu64 "\n", j + 1);
			}
		}
		status_now = !status_now;
	}
	destruct_parameters(parameters);
	return 0;
}