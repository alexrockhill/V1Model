// Alex Rockhill 3.10.2019

#include <stdio.h>
#include "plot.h" 
#include "network.h"
#include "random.h"

int main()
{
	int seed = 11;
	//plot_line();
	struct v1n n;
	n.x = 0;
	n.y = 1;
	n.ori = my_rand(&seed);
	printf("%f\n", n.ori);
}