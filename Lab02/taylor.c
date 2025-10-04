#include <stdio.h>
#include <stdint.h>

double t_sigmoid(double in);



double t_tanh(double in);




void main(void){
//declare vars

double in[3] = 0,sig_out[3] = {0},tanh_out[3] = {0};



//taylor sigmoid
for(uint8_t i = 0;i < ){
 sig_out[i] = t_sigmoid(in[i]);
 tanh_out[i] = t_tanh(in[i]);
 printf("Input: %d, Sigmoid: %d, Tanh: %d\r\n",in[i],sig_out[i],tanh_out[i]);
}

//taylor tanh







}