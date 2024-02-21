
#define BIT_VALUE(value,noBit) (value >> noBit) & 1

unsigned int counter = 0;


void delay(){
    int a,b;
    for(a = 0; a<=147483647; a++)
    {
        for(b=0;b<=512;b++){

        }
    }
}

int main()
{
    int IntToGray(unsigned char input){
        return (input >> 1) ^ input;
    }

    while(1)
    {
        printf("\r");
        for(int i=7;i>=0;i--)
        {
            printf("%d",BIT_VALUE(counter,i));
        }
        printf("    ");
        for(int i=7;i>=0;i--){
            printf("%d",BIT_VALUE(IntToGray(counter),i));
        }
        counter++;
        delay();
    }
}
