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

  
     printf("float list is  is: \r\n");
    for(int i = 0;i < 5;i++){
        printf("%f and %f",a[i],b[i]);
    }
    fp16_t half_a[5];
    fp16_t half_b[5];
     for(int i = 0;i < 5;i++){
         half_a[i] = FP16(&(a[i]));
    }
     for(int i = 0;i < 5;i++){
      half_b[i] = FP16(&(b[i]));

    }

    printf("fp16 list is:\r\n");
   
    for(int i = 0;i < 5;i++){
        printf("pair %i:\r\n",i);
        print_fp16(&(half_a[i]));
         print_fp16(&(half_b[i]));

    }


    float result[5];
    fp16_t result_fp16[5];
    printf("multiplications:\r\n");
    for(int i = 0;i < 5;i++){
        
        result[i] = a[i] * b[i];
        printf("float mul is %f, fp16 mul is:",result[i]);
        result_fp16[i] = FP16_Mul(half_a[i],half_b[i]);
        print_fp16(&(result_fp16[i]));
    }

  for(int i = 0;i < 5;i++){
        
        result[i] = a[i] + b[i];
        printf("float add is %f, fp16 add is:",result[i]);
        result_fp16[i] = fp16_add(half_a[i],half_b[i]);
        print_fp16(&(result_fp16[i]));
    }
