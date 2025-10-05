#include <stdio.h>
#include <stdint.h>
#include <math.h> 

//exact versions so that we can compare in outputs:
double exact_sigmoid(double x) 
{
    return 1.0 / (1.0 + exp(-x));
}
double exact_tanh(double x) 
{
    return (exp(x)-exp(-x)) / (exp(x)+exp(-x));
}


//finding T_Sigmoid around 0 or more acc.
double t_sigmoid(double in)
{
    return 0.5 + 0.25 * in;
}

double t_tanh(double in)
{
    return in - ((1.0 / 3.0) * pow(in,3) );
}


int main(void)
{
    //declare vars

    double in[3] = {-1.0, 0.0, 1.0}; //imguessig u mean inputs??
    double sig_out[3] = {0}, tanh_out[3] = {0};
    double actual_sig_out[3] = {0}, actual_tanh_out[3] = {0};

    //taylor sigmoid
    for(uint8_t i = 0;i < 3; i++)
    {
        sig_out[i] = t_sigmoid(in[i]);
        tanh_out[i] = t_tanh(in[i]);
        actual_sig_out[i] = exact_sigmoid(in[i]);
        actual_tanh_out[i] = exact_tanh(in[i]);
        
        printf("Input: %f\n Sigmoid |  Actual: %f Approximation: %f \n Tanh |  Actual: %f Approximation: %f \n\n", in[i], actual_sig_out[i], sig_out[i], actual_tanh_out[i], tanh_out[i]);

        //
        //I want pretty outputs
        //printf("For Input: %f\n", something);
        //printf("Sigmoid:  Taylor: %f | Exact: %f\n", something, something);
        //printf("Tanh:     Taylor: %f | Exact: %f\n\n", honey, bunney);
        //
    }

//taylor tanh


}



