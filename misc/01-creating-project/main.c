

/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */
volatile int a, b, add, sub, mul, div, rem, result;


int main(void){
	a = 27;
	b = 9;
	add = a + b;
	sub = a - b;
    mul = a * b;
	div = a / b;
	rem = a % b;

	result = add + sub + mul + div + rem;


	while (1) {
	}

	return 0;
}


