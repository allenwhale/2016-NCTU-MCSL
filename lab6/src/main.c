/*
 * main.cpp
 *
 *  Created on: 2016¦~11¤ë26¤é
 *      Author: Administrator
 */
#include "stm32l476xx.h"
#define abs(x) (((x) < 0) ? (-(x)) : (x))
extern void GPIO_init();
extern void max7219_init();
extern void SegSend(int, int);
static const int keypad[][4] = {
		{1, 2, 3, 10},
		{4, 5, 6, 11},
		{7, 8, 9, 12},
		{15, 0, 14, 13}
};
static int col[] = {5, 6, 7, 8};
static int row[] = {3, 4, 5, 6};
static GPIO_TypeDef* COL_GPIO = GPIOA;
static GPIO_TypeDef* ROW_GPIO = GPIOB;
int display(int data, int num_digs){
	for(int i=1;i<=num_digs;i++){
		SegSend(i, data % 10);
		data /= 10;
	}
	for(int i=num_digs+1;i<=8;i++){
		data /= 10;
		SegSend(i, 15);
	}
	if(data != 0)
		return -1;
	return 0;
}
int read_gpio(GPIO_TypeDef* gpio, int pin){
	return (gpio->IDR >> pin) & 1;
}
void set_gpio(GPIO_TypeDef* gpio, int pin){
	gpio->BSRR |= (1 << pin);
}
void reset_gpio(GPIO_TypeDef* gpio, int pin){
	gpio->BRR |= (1 << pin);
}
void keypad_init(){
	// PB 3456 ROW 1234 INPUT
	// PA 5678 COL 1234 OUTPUT
	// SET keypad GPIO OUTPUT
	// PA 5678 - COL 1234 OUTPUT
	RCC->AHB2ENR = RCC->AHB2ENR | 0x1;
	//Set PA5 ~ PA8 as output mode
	GPIOA->MODER = GPIOA->MODER & 0xFFFC03FF;
	GPIOA->MODER = GPIOA->MODER | 0x15400;
	// SET keypad GPIO INPUT
	// PB 3456 - ROW 1234 INPUT
	RCC->AHB2ENR = RCC->AHB2ENR | 0x2;
	//Set PB3 ~ PB6 as INPUT mode
	GPIOB->MODER = GPIOB->MODER & 0xFFFFC03F;
	GPIOB->MODER = GPIOB->MODER | 0x0;
	//set PB3 ~ PB6 is Pull-Down
	GPIOB->PUPDR = GPIOB->PUPDR & 0xFFFFC03F;
	GPIOB->PUPDR = GPIOB->PUPDR | 0x2A80;
	//Set PA5 ~ PA8 as high speed mode
	GPIOA->OSPEEDR = GPIOA->OSPEEDR | 261120;

	GPIOA->OTYPER |= 0x1e0;
	for(int i=5;i<=8;i++)
		set_gpio(GPIOA, i);

	//GPIOA->MODER &= 0xFFFC03FF;
	//GPIOA->PUPDR &= ~(((1<<8)-1)<<5);
	//GPIOA->PUPDR |= 0x15400;
}
void lab60(){
	int student_id = 316320;
	display(student_id, 6);
}

int get_keypad_input(){
	int cnt[4][4];
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			cnt[i][j] = 0;
	for(int k=400;k--;){
		for(int j=0;j<4;j++){
			GPIOA->OTYPER &= ~(1 << col[j]);
			//GPIOA->MODER |= (1 << (col[j] << 1));
			//GPIOA->BSRR |= (1 << col[j]);
			for(int i=0;i<4;i++){
				cnt[i][j] += read_gpio(ROW_GPIO, row[i]);
			}
			GPIOA->OTYPER |= (1 << col[j]);
			//GPIOA->MODER &= 0xFFFC03FF;
		}
	}
	int res = 0;
	for(int i=4;i--;){
		for(int j=4;j--;){
			res = (res << 1) | (cnt[i][j] >= 300);
		}
	}
	return res;
}
int get_keypad_input_one(int x, int y){
	int cnt = 0;
	GPIOA->OTYPER &= ~(1 << col[y]);
	for(int i=400;i--;)
		cnt += read_gpio(ROW_GPIO, row[x]);
	GPIOA->OTYPER |= (1 << col[y]);
	return (cnt > 300);
}
int get_keypad_input_one_raw(int x, int y){
	GPIOA->OTYPER &= ~(1 << col[y]);
	int res = read_gpio(ROW_GPIO, row[x]);
	GPIOA->OTYPER |= (1 << col[y]);
	return res;
}
int num_digits(int x){
	if(x == 0)return 1;
	int res = 0;
	while(x){
		res++;
		x /= 10;
	}
	return res;
}
void lab61(){
	keypad_init();
	while(1){
		int input = 0;
		for(int i=0;i<4;i++){
			for(int j=0;j<4;j++){
				if(get_keypad_input_one_raw(i, j)){
					input = 1;
					display(keypad[i][j], num_digits(keypad[i][j]));
				}
			}
		}
		if(input == 0)
			display(0, 0);
	}
}
void lab62(){
	int last_status[4][4], cnt[4][4];
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			last_status[i][j] = cnt[i][j] = 0;
	keypad_init();
	int total_digits = 0;
	int show_nums[8], show_num_idx = 0;
	while(1){
		int input = 0, sum = 0, cancel = 0;
		int keypad_input = get_keypad_input();
		for(int i=0;i<4;i++){
			for(int j=0;j<4;j++){
				int x = (keypad_input >> (i * 4 + j)) & 1;
				if((last_status[i][j] == 0 && x) || cnt[i][j] >= 10){
					cnt[i][j] = 0;
					input = 1;
					sum += keypad[i][j];
					if(keypad[i][j] == 15 || keypad[i][j] == 14){
						cancel = 1;
					}
				}
				if(x){
					cnt[i][j]++;
				}else{
					cnt[i][j] = 0;
				}
				last_status[i][j] = x;
			}
		}
		if(cancel){
			total_digits = 0;
			show_num_idx = 0;
		}else if(input){
			int digits = num_digits(sum);
			if(digits + total_digits <= 8){
				total_digits += digits;
				show_nums[show_num_idx++] = sum;
			}
			for(int i=0;i<4;i++){
				for(int j=0;j<4;j++){
					cnt[i][j] = 0;
				}
			}
		}
		if(total_digits == 0){
			display(0, 0);
		}else{
			int data = 0;
			for(int i=0;i<show_num_idx;i++){
				for(int j=num_digits(show_nums[i]);j--;)
					data *= 10;
				data += show_nums[i];
			}
			display(data, total_digits);
		}
	}
}
struct fraction{
	int a, b;
};
int gcd(int a, int b){
	while((a %= b) && (b %= a));
	return a + b;
}
int lcm(int a, int b){
	return a / gcd(a, b) * b;
}
struct fraction add(struct fraction x, struct fraction y){
	struct fraction res;
	res.b = lcm(x.b, y.b);
	res.a = x.a * res.b / x.b + y.a * res.b / y.b;
	return res;
}
struct fraction sub(struct fraction x, struct fraction y){
	struct fraction res;
	res.b = lcm(x.b, y.b);
	res.a = x.a * res.b / x.b - y.a * res.b / y.b;
	return res;
}
struct fraction mul(struct fraction x, struct fraction y){
	struct fraction res;
	res.a = x.a * y.a;
	res.b = x.b * y.b;
	int d = gcd(res.a, res.b);
	res.a /= d, res.b /= d;
	return res;
}
struct fraction div(struct fraction x, struct fraction y){
	struct fraction tmp = {.a=y.b, .b=y.a};
	return mul(x, tmp);
}
struct fraction calc(int *data, int n){
	int stk[128], stk_idx = 0;
	int output[128], output_idx = 0;
	for(int i=0;i<n;i++){
		if(1010 <= data[i] && data[i] <= 1013){ //op
			while(stk_idx && stk[stk_idx - 1] >= data[i]){
				output[output_idx++] = stk[--stk_idx];
			}
			stk[stk_idx++] = data[i];
		}else{
			output[output_idx++] = data[i];
		}
	}
	while(stk_idx){
		output[output_idx++] = stk[--stk_idx];
	}
	struct fraction ans = {.a=0, .b=1}, op1, op2, invalid = {.a=-1, .b=1};
	struct fraction ans_stk[128];
	int ans_stk_idx = 0;
	for(int i=0;i<n;i++){
		if(output[i] == 1010){
			if(ans_stk_idx < 2) return invalid;
			op2 = ans_stk[--ans_stk_idx];
			op1 = ans_stk[--ans_stk_idx];
			ans_stk[ans_stk_idx++] = add(op1, op2);
		}else if(output[i] == 1011){
			if(ans_stk_idx < 2) return invalid;
			op2 = ans_stk[--ans_stk_idx];
			op1 = ans_stk[--ans_stk_idx];
			ans_stk[ans_stk_idx++] = sub(op1, op2);
		}else if(output[i] == 1012){
			if(ans_stk_idx < 2) return invalid;
			op2 = ans_stk[--ans_stk_idx];
			op1 = ans_stk[--ans_stk_idx];
			ans_stk[ans_stk_idx++] = mul(op1, op2);
		}else if(output[i] == 1013){
			if(ans_stk_idx < 2) return invalid;
			op2 = ans_stk[--ans_stk_idx];
			op1 = ans_stk[--ans_stk_idx];
			ans_stk[ans_stk_idx++] = div(op1, op2);
		}else{
			op1.a = output[i], op1.b = 1;
			ans_stk[ans_stk_idx++] = op1;
		}
	}
	if(ans_stk_idx != 1){
		return invalid;
	}
	return ans_stk[0];
}
void display_fraction(struct fraction x){
	int sign = (x.a * x.b) >= 0 ? 0 : 1;
	x.a = abs(x.a);
	x.b = abs(x.b);
	if(x.b == 1){
		int digits = num_digits(x.a);
		display(x.a, digits);
		if(sign){
			SegSend(digits + 1, 10);
		}
	}else{
		int a = x.a / x.b, b = 0, a_digits = num_digits(a), b_digits = 0;
		int digits = a_digits + sign;
		x.a %= x.b;
		for(;digits<8&&x.a;digits++,b_digits++){
			x.a *= 10;
			b = b * 10 + x.a / x.b;
			x.a %= x.b;
		}
		for(int i=0;i<b_digits;i++){
			SegSend(i + 1, b % 10);
			b /= 10;
		}
		for(int i=0;i<a_digits;i++){
			SegSend(i + b_digits + 1, (a % 10) | (i ? 0 : (1 << 7)));
			a /= 10;
		}
		for(int i=digits+1;i<=8;i++){
			SegSend(i, 15);
		}
	}
}
void lab63(){
	keypad_init();
	int last_status[4][4];
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			last_status[i][j] = 0;
	int input[128], input_idx = 0;
	int now_input = 0, now_digits = 0;
	while(1){
		int answer = 0;
		for(int i=0;i<4;i++){
			for(int j=0;j<4;j++){
				int x = get_keypad_input_one(i, j), key = keypad[i][j];
				if(last_status[i][j] == 0 && x){
					if(0 <= key && key <= 9){
						if(now_digits < 3){
							now_digits++;
							now_input = now_input * 10 + key;
						}
						display(now_input, now_digits);
					}else{
						if(now_digits){
							input[input_idx++] = now_input;
							now_input = now_digits = 0;
						}
						if(key == 15){
							answer = 1;
						}else if(key == 14){
							input_idx = 0;
							now_input = 0;
							now_digits = 0;
							display(0, 0);
						}else{
							input[input_idx++] = key + 1000;
							display(0, 0);
						}
					}
				}
				last_status[i][j] = x;
			}
		}
		if(answer){
			struct fraction ans = calc(input, input_idx);
			display_fraction(ans);
			input_idx = 0;
		}
	}
}
int main(){
	GPIO_init();
	max7219_init();
	display(0, 0);
	lab62();
	return 0;
}

