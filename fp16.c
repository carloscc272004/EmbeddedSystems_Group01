
#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

//typedef
typedef uint16_t fp16_t;
//DEFINES HERE 

 
#define FP32_BIAS 127
#define FP16_BIAS 15

//23 BITS AND 8 BITS
#define FP32_MANT_BITMASK 0x7FFFFF
#define FP32_EXP_BITMASK 0XFF

#define FP32_SIZE_OF_MANT 23
#define FP32_EXP_OFFSET FP32_SIZE_OF_MANT
#define FP32_SIGN_OFFSET 31

//10 BITS AND 5 BITS
#define FP16_MANT_BITMASK 0x3FF
#define FP16_EXP_BITMASK 0X1F

#define FP16_SIZE_OF_MANT 10
#define FP16_EXP_OFFSET FP16_SIZE_OF_MANT
#define FP16_SIGN_OFFSET 15



#define SINGLE_BIT_MASK 0x01


//GLLOBAL FUNCTION PROTOTYPES-> COMMENTS AT FUNCTION DECLARATION

fp16_t FP16(const float * const restrict x);
float float_from_fp16(const fp16_t  * const restrict x);
 void print_fp16(const fp16_t * const restrict x);
 fp16_t FP16_Mul(fp16_t x, fp16_t y);
 fp16_t fp16_add(const fp16_t x, const fp16_t y);
 //STATIC HELPER FUNCTION  PROTOTYPES -> COMMENTS AT FUNCTION DECLARATION

static inline void fp16_decompose(const fp16_t  * const restrict x, int * const restrict sign, int * const restrict exponent, uint16_t * const restrict mant);
static inline double fp16_cal_msum(const uint16_t mantissa);
static inline float fp16_parts_tofloat(int sign, int exponent, float mant_sum );

static inline void fp32_decompose(const uint32_t * const restrict x,uint8_t * const restrict sign_bit, uint8_t * const restrict exp_byte, uint32_t * const restrict mant);
static inline fp16_t fp32_parts_tofp16(const uint8_t * const restrict sign_bit, uint8_t * const restrict exp_byte, uint32_t * const restrict mant);



int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT

    // Configure GPIO
    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
    P1DIR |= BIT0;                          // Set P1.0 to output direction

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

        float a[5];
    float b[5];

    a[0] = 5;
    b[0] = 1;
    a[1] = 7.259;
    b[1] = -2.141;
    a[2] = 100.237;
    b[2] = -69.37;
    a[3] = -1.58;
    b[3] = -200.568;
    a[4] = 1000.548;
    b[4] = 55.478;

  
    //  printf("float list is  is: \r\n");
    // for(int i = 0;i < 5;i++){
    //     printf("%f and %f",a[i],b[i]);
    // }
    fp16_t half_a[5];
    fp16_t half_b[5];
    uint8_t i = 0;
     for( i = 0;i < 5;i++){
         half_a[i] = FP16(&(a[i]));
    }
     for( i = 0;i < 5;i++){
      half_b[i] = FP16(&(b[i]));

    }

    // printf("fp16 list is:\r\n");
   
    // for(int i = 0;i < 5;i++){
    //     // printf("pair %i:\r\n",i);
    //     print_fp16(&(half_a[i]));
    //      print_fp16(&(half_b[i]));

    // }

   float result_converted_back[5] = {0};
    float result[5] = {0};
    fp16_t result_fp16[5] = {0};
    // printf("multiplications:\r\n");
    for( i = 0;i < 5;i++){
        
        result[i] = a[i] * b[i];
        // printf("float mul is %f, fp16 mul is:",result[i]);
        result_fp16[i] = FP16_Mul(half_a[i],half_b[i]);
        result_converted_back[i] = float_from_fp16(&(result_fp16[i]));
        // print_fp16(&(result_fp16[i]));
    }

  for( i = 0;i < 5;i++){
        
        result[i] = a[i] + b[i];
        // printf("float add is %f, fp16 add is:",result[i]);
        result_fp16[i] = fp16_add(half_a[i],half_b[i]);
           result_converted_back[i] = float_from_fp16(&(result_fp16[i]));
        // print_fp16(&(result_fp16[i]));
    }


    while(1)
    {
        P1OUT ^= BIT0;                      // Toggle LED
        __delay_cycles(100000);
    }
}




/***************8
 * CONVERT FLOAT TO FP16
 */
fp16_t FP16(const float * const restrict x){

  
    uint32_t bare_x = 0;
    //we want the raw unprotected bytes of the float.  memcpy works for this
    memcpy(&bare_x,x,sizeof(bare_x));

        uint8_t sign = 0;
    uint32_t mantissa = 0;
    uint8_t exponent = 0;

    fp32_decompose(&bare_x,&sign,&exponent,&mantissa);
  
        
   
    fp16_t half_prec_float = fp32_parts_tofp16(&sign, &exponent, &mantissa);

    //optional debug messages
    // printf("float bytes are %x and bare bytes are  %x\r\n",*x,bare_x);
    // printf("sign bit is %x, mantissa is %x, exponent is %x\r\n ", sign,mantissa,exponent);
    //  printf("half_prec_float bytes are %x",half_prec_float);

    return half_prec_float;

}


/*******************8
 * CONVERT FP16 TO FLOAT
 */
float float_from_fp16(const fp16_t  * const restrict x){

int sign = 0;
int exponent = 0;
uint16_t mantissa = 0;

fp16_decompose(x,&sign,&exponent,&mantissa);

float mant_sum = fp16_cal_msum(mantissa);

// optional debug statements
//  printf("p:exponent = %x ",exponent);
// printf("p:mantissa = %x ",mantissa);
// printf("p:mant_sum = %f \r\n",mant_sum);


return fp16_parts_tofloat(sign,exponent,mant_sum);


}


/***************
 * PRINT AN FP16 VALUE
 */
 void print_fp16(const fp16_t * const restrict x){

float converted = float_from_fp16(x);
 
//print value
// printf("%f\r\n",converted);
return;
//debug message
//  printf("\r\n END OF PRINT\r\n");
}

fp16_t fp16_add(const fp16_t x, const fp16_t y){



   float x_fl = float_from_fp16(&x);

   float y_fl = float_from_fp16(&y);

   float result =  x_fl + y_fl;

   fp16_t fp16_result = FP16(&result);

   return fp16_result;


}


fp16_t FP16_Mul(const fp16_t x, const fp16_t y)
{

    //grab parts of x float
    int x_sign;
    int x_exponent;
    uint16_t x_mant;
    fp16_decompose(&x, &x_sign, &x_exponent, &x_mant);

    //grab parts of y float
    int y_sign;
    int y_exponent;
    uint16_t y_mant;
    fp16_decompose(&y, &y_sign, &y_exponent, &y_mant);



    //determine sign bit
    uint8_t result_sign = (((x_sign) * (y_sign)) < 0) ? 1 : 0; 
    
    //grab real mantissa values
    double mantsum_x = 1.0 + fp16_cal_msum(x_mant);  //this gives value after implied 1 so we add 1
    double mantsum_y = 1.0 + fp16_cal_msum(y_mant);

    //multiply the mantissas
    float mant_mul = mantsum_x * mantsum_y;
    int of_exponent = 0;


    //ensure mantissa has just a 1 to fit our implied one requirement
    while(mant_mul >= 2){
        mant_mul /= 2.0;
        of_exponent++;

    }
    
    uint32_t rdy_mant_mul = 0;

    memcpy(&rdy_mant_mul,&mant_mul,sizeof(rdy_mant_mul));

     //determine result exponent
    uint8_t result_exponent = (x_exponent) + (y_exponent) + of_exponent;
    fp16_t result =  fp32_parts_tofp16(&result_sign,&result_exponent,&rdy_mant_mul);


     return result;




}

/**************
 * INLINE HELPER FUNCTIONS
 */


 /*****
  * DECOMPOSE FP32 TO ITS PARTS
  */
static inline void fp32_decompose(const uint32_t * const restrict x,uint8_t * const restrict sign_bit, uint8_t * const restrict exp_byte, uint32_t * const restrict mant){
    *sign_bit = ((*x >> FP32_SIGN_OFFSET) &  0x01);
    *exp_byte =  ((*x >> FP32_EXP_OFFSET) & 0xFF) - FP32_BIAS;
    *mant = (*x) & FP32_MANT_BITMASK;

    return;
}

/********
 * COMBINE FP32 PARTS INTO FP16
 * 
 */
static inline fp16_t fp32_parts_tofp16(const uint8_t * const restrict sign_bit, uint8_t * const restrict exp_byte, uint32_t * const restrict mant){
     //add 16 bit bias and truncate to 5 bits
    *exp_byte = (*exp_byte + FP16_BIAS) & FP16_EXP_BITMASK;
    //truncate to 10 bits
    *mant = (((*mant >> (FP32_SIZE_OF_MANT - FP16_SIZE_OF_MANT)) & FP16_MANT_BITMASK));

    
    return ((*sign_bit ) << FP16_SIGN_OFFSET) | ((*exp_byte) << FP16_EXP_OFFSET) | (*mant);
 
   
}


/***********8
 * DECOMPOSE FP16 TO ITS PARTS
 */
static inline void fp16_decompose(const fp16_t  * const restrict x, int * const restrict sign, int * const restrict exponent, uint16_t * const restrict mant){
    //return only single sign bit
    *sign = ((*x >> FP16_SIGN_OFFSET) == 1)?-1:1;
    *exponent = ((*x >> FP16_EXP_OFFSET) & FP16_EXP_BITMASK) - FP16_BIAS;
    *mant = (*x & FP16_MANT_BITMASK);
    return;
}



/**********8
 * CALCULATE THE VALUE OF FP16_MANTISSA
 */
static inline double fp16_cal_msum(const uint16_t mantissa){
double mantissa_sum = 0;
//calculate mantissa by summing each decimal bit position multiplied by its corresponding power of two
uint8_t i = 0;
for( i = 1;i <= FP16_SIZE_OF_MANT;i++){

    mantissa_sum += (((mantissa >> (FP16_SIZE_OF_MANT - i)) & SINGLE_BIT_MASK) * pow(2.0,(double)( -i )));

} 
return mantissa_sum;
}


/*************
 * TURN FP16 PARTS INTO A FLOAT
 */
static inline float fp16_parts_tofloat(const int sign, const int exponent, const float mant_sum ){
    //float should be 1.mantissa * sign * 2 to power exponent
    return sign * (1.0 + mant_sum) * pow(2,(double)exponent);

}
