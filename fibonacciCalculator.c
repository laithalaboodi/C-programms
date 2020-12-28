/******************************************************************************

By: Laith Alaboodi
Simple Fibonacci Calculator

*******************************************************************************/

#include <stdio.h>

//method to see if its square
  int isSquare(int x)
    {
        float temp; 
        float squareR;
         squareR = x / 2;
         temp = 0;
         while(squareR != temp){
    
        temp = squareR;
        
        squareR = ( x/temp + temp) / 2;
    }
        
       int s = ( x/temp + temp) / 2; 
       return (s*s == x);
       
     }  
     //method to return the calulation 
   int isFib(int i)
      {
         return isSquare(5*i*i + 4) ||
           isSquare(5*i*i - 4);
     }
    int main(void)
    {   
    
	 int userInput;
	 scanf("%d", &userInput);
	  if (userInput>0)
		{
		 printf("%d is",userInput);
		  if(isFib(userInput)==1){
		        printf(" fib");
		    }
		    if(isFib(userInput)==0){
		        printf(" not fib");
		    }
		} 
		return 0;        
   }
