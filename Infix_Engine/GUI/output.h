// print logo
void printlogo()
{
	//***************************************************************************************************
	// |=====| ||\   ||  |==== |=====|  \\  //        ||==== ||\   ||  ||=====|| |=====| ||\   ||  ||==== 
	//   ||    ||\\  ||  |===    ||      \\//         ||___  ||\\  ||  ||          ||    ||\\  ||  ||__   
	//   ||    || \  ||  ||      ||      //\\         ||     || \  ||  ||   ==||   ||    || \  ||  ||     
	// |=====| ||  \\||  ||    |=====|  //  \\        ||==== ||  \\||  ||=====|| |=====| ||  \\||  ||==== 
	//***************************************************************************************************
	printf("\x1B[35m***************************************************************************************************\n");
	printf("|====| ||\\   || |==== |====| \\\\  //        ||==== ||\\   ||  ||=====|| |====| ||\\   ||  ||====\n");
	printf("  ||   ||\\\\  || |===    ||    \\\\//         ||___  ||\\\\  ||  ||          ||   ||\\\\  ||  ||__  \n");
	printf("  ||   || \\\\ || ||      ||    //\\\\         ||     || \\\\ ||  ||   ==||   ||   || \\\\ ||  ||    \n");
	printf("|====| ||  \\\\|| ||    |====| //  \\\\        ||==== ||  \\\\||  ||=====|| |====| ||  \\\\||  ||====\n");
	printf("***************************************************************************************************\033[0m\n");
}

// error handling
void printerror(const char* error_str) { printf("\x1B[31mERROR: %s \033[0m\n", error_str); }
void printerror(const char* error_str, int intager) { printf("\x1B[31mERROR: %s: %i \033[0m\n", error_str, intager); }
void printerror(const char* error_str, float floating) { printf("\x1B[31mERROR: %s: %f \033[0m\n", error_str, floating); }
void printerror(const char* error_str, const char* one_str) { printf("\x1B[31mERROR: %s: %s \033[0m\n", error_str, one_str); }

//warning handling
void printwarning(const char* warning_str) { printf("\x1B[33mWARNING: %s \033[0m\t\t", warning_str); }
void printwarning(const char* warning_str, int arg_one) { printf("\x1B[33mWARNING: %s: %i \033[0m\t\t", warning_str, arg_one); }
void printwarning(const char* warning_str, float arg_one) { printf("\x1B[33mWARNING: %s: %f \033[0m\t\t", warning_str, arg_one); }
void printwarning(const char* warning_str, const char* arg_one) { printf("\x1B[33mWARNING: %s: %s \033[0m\t\t", warning_str, arg_one); }

//loading
void updateloading(float percentage)
{
	printf("\r\x1B[32mLOADING [");

	for (float i = 0; i < 20; i++)
	{
		if (percentage > (i / 20.0))
		{
			printf("#");
		}
		else
		{
			printf("-");
		}
	}
	printf("]\033[0m");
}