#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DBG 1

struct divisorRegs
{
	int dll;
	int dlm;
	int dld;
};

#define PRESCALER_NUM 2
#define SAMPLERATE_NUM 3

int prescaler[PRESCALER_NUM] = {1, 4};
int samplerate[SAMPLERATE_NUM] = {16, 8, 4};

void print_usage(char* program_name)
{
	printf("-----------------------------------------------------------------\n"
		   "DivisorGen of XR16V698 UART Chip's BRG for Specified BaudRate\n\n"
		   "Usage:\n"
		   "\t%s <clock> <baudrate>\n\n"
		   "-----------------------------------------------------------------\n",
		   program_name);

}

double divisorGen(long baudrate, int prescaler, int samplerate, long clock)
{
	double first_round_divisor, second_round_divisor;

	first_round_divisor = (clock / prescaler) / ((double)baudrate * samplerate);

	second_round_divisor = round((first_round_divisor - floor(first_round_divisor)) * 16) / 16 + floor(first_round_divisor);

#if DBG

	printf("\t(original divisor, manipulated divisor, error rate) --> (%.4f, %.4f, %.2f)\n", first_round_divisor, second_round_divisor, fabs(second_round_divisor - first_round_divisor) * 100 / first_round_divisor);

#endif

	return second_round_divisor;

}


struct divisorRegs divisorRegsGen(double divisor)
{
	struct divisorRegs regs;

	regs.dll = (int)floor(divisor) & 0xff;
	regs.dlm = (int)floor(divisor) >> 8;
	regs.dld = (int)round((divisor - floor(divisor)) * 16);

	return regs;
}


int main(int argc, char* argv[])
{

	long baudrate, clock;
	double divisor;
	struct divisorRegs regs;

	if (argc != 3) {

		print_usage(argv[0]);
		exit(1);

	}

	clock = atoi(argv[1]);
	baudrate = atoi(argv[2]);

#if DBG
	printf("baudrate is %ld\n", baudrate);
#endif


	printf("\nReg values:\n\n");

	for (int i=0; i<PRESCALER_NUM; i++) {

		for (int j=0; j<SAMPLERATE_NUM; j++) {

			divisor = divisorGen(baudrate, prescaler[i], samplerate[j], clock);

			regs = divisorRegsGen(divisor);


			if (0 == i) {

				printf("\t(prescaler, samplerate) is (\033[31m\033[1m%d, %d\033[0m):\n\n"
					   "\t\t(DLL, DLM, DLD) --> (\033[32m0x%x, 0x%x, 0x%x\033[0m)\n\n",
					   prescaler[i], samplerate[j], regs.dll, regs.dlm, regs.dld);

			} else {

				printf("\t(prescaler, samplerate) is (\033[31m\033[1m%d, %d\033[0m):\n\n"
					   "\t\t(DLL, DLM, DLD) --> (\033[33m\033[1m0x%x, 0x%x, 0x%x\033[0m)\n\n",
					   prescaler[i], samplerate[j], regs.dll, regs.dlm, regs.dld);

			}

		}

	}

	printf("Done!\n\n");

	return 0;

}
