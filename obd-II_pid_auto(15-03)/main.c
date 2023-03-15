#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<dos.h>
void receive(char[]);
HANDLE hComm;                          // Handle to the Serial port
char  ComPortName[] = "\\\\.\\COM7";  // Name of the Serial port(May Change) to be opened,
BOOL  Status;                          // Status of the various operations
DWORD dwEventMask;                     // Event mask to trigger
char  TempChar;                        // Temperory Character
static DWORD  dNoOFBytestoWrite = 0;          // No of bytes to write into the port
static DWORD  dNoOfBytesWritten = 0;          // No of bytes written to the port
static DWORD NoBytesRead = 0 ;                     // Bytes read by ReadFile()
static char  SerialBuffer[65536];   // Buffer Containing Rxed Data
static char  SerialBuffer1[65536];
char    var = '\r';
static int i,temp=0;

char lpBuffer[15]= "atz";
char lpBuffer2[15]= "atsp6";
char hex[15]= "";
void main(void)
{
    printf("\n\n +==========================================+");
    printf("\n |    Serial Port  Reception (Win32 API)    |");
    printf("\n +==========================================+\n");
    /*---------------------------------- Opening the Serial Port -------------------------------------------*/

    hComm = CreateFile( ComPortName,                  // Name of the Port to be Opened
                        GENERIC_READ | GENERIC_WRITE, // Read/Write Access
                        0,                            // No Sharing, ports cant be shared
                        NULL,                         // No Security
                        OPEN_EXISTING,                // Open existing port only
                        0,                            // Non Overlapped I/O
                        NULL);                        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE)
        printf("\n    Error! - Port %s can't be opened\n", ComPortName);
    else
        printf("\n    Port %s Opened\n ", ComPortName);

    /*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/

    DCB dcbSerialParams = { 0 };                         // Initializing DCB structure
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    Status = GetCommState(hComm, &dcbSerialParams);      //retreives  the current settings

    if (Status == FALSE)
        printf("\n    Error! in GetCommState()");


    /*------------------------------------ Setting Timeouts --------------------------------------------------*/


    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout         = 10;
    timeouts.ReadTotalTimeoutConstant    = 220;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.WriteTotalTimeoutConstant   = 100;
    timeouts.WriteTotalTimeoutMultiplier = 1;

    if (SetCommTimeouts(hComm, &timeouts) == FALSE)
        printf("\n\n    Error! in Setting Time Outs");
    else
        printf("\n\n    Setting Serial Port Timeouts Successful");
    printf_s("\n>");

    while(1)
    {
        memset(SerialBuffer,0,sizeof(SerialBuffer));
        receive(lpBuffer);
        send1();
        Sleep(1);
        receive(lpBuffer2);
        send1();
        Sleep(1);
        hex[0]='0';

        for(int dec=256;dec<392;dec++){
            convert(dec);
            hex[0]='0';
            receive(hex);
            send1();
            //Sleep(1);
            memset(hex,0,sizeof(hex));
        }
        print1();

    }
    CloseHandle(hComm);//Closing the Serial Port
    printf("\n +==========================================+\n");
    _getch();
}//End of Main()


void receive(char lpBuffer1[])
{
    strncat(lpBuffer1, &var, 1);
    dNoOFBytestoWrite = 0;          // No of bytes to write into the port
    dNoOfBytesWritten = 0;          // No of bytes written to the port
    NoBytesRead = 0 ;
    dNoOFBytestoWrite = (strlen(lpBuffer1));
    Status = WriteFile(hComm,               // Handle to the Serialport
                       lpBuffer1,            // Data to be written to the port
                       dNoOFBytestoWrite,   // No of bytes to write into the port
                       &dNoOfBytesWritten,  // No of bytes written to the port
                       NULL);

    if(lpBuffer1==lpBuffer)
    {
        send1();
    }
    if (Status == FALSE)
    {
         printf("\n\n   Error %d in Writing to Serial Port",GetLastError());
    }
}

void send1()
{
    Status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception
    if (Status == FALSE)
    {
        printf("\n\n    Error! in Setting CommMask");
    }
    /*------------------------------------ Setting WaitComm() Event   ----------------------------------------*/

    Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received

    /*-------------------------- Program will Wait here till a Character is received ------------------------*/

    if (Status == FALSE)
    {
        printf("\n    Error! in Setting WaitCommEvent()");
    }
    else
    {
        do
        {
            Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
            SerialBuffer[i] = TempChar;
            i++;
        }
        while ((NoBytesRead > 0));


        if(SerialBuffer[6]=='4' && SerialBuffer[7]=='1')
        {
            temp++;
            for(int j=0;j<4;j++){
            SerialBuffer1[temp]=SerialBuffer[j];
            temp++;
            }
            SerialBuffer1[temp]='=';
            temp++;
            SerialBuffer1[temp]='"';
            temp++;
            for(int j=12;j<strlen(SerialBuffer)-7;j++){

                        SerialBuffer1[temp]  = SerialBuffer[j];
                        temp++;
            }

            SerialBuffer1[temp]='"';
            temp++;
            SerialBuffer1[temp]=',';
            temp++;
            //printf("ok\n");
        }
        else if(SerialBuffer[6]=='N' && SerialBuffer[7]=='O')
        {

        }
        else
        {
            for (int j = 0; j < i-1; j++)
            {
                printf_s("%c", SerialBuffer[j]);
            }
        }
        memset(SerialBuffer,0,sizeof(SerialBuffer));
        i=0;
    }
}


void print1 ()
{
    printf("{P:");
    for(int k=0;k<temp-1;++k)
    {
            if(SerialBuffer1[k]==' '||SerialBuffer1[k]=='>')
            {
                continue;
            }
            else{
             printf("%c",SerialBuffer1[k]);
            }

    }
    memset(SerialBuffer1,0,sizeof(SerialBuffer1));
    printf("}\n\n>");
}


void convert(int decimalnum)
{
    int quotient, remainder;
    char hexadecimalnum[4];
    quotient = decimalnum;
    int i, j = 0;
    //hex[0]='0';
    while (quotient != 0)
    {
        remainder = quotient % 16;
        if (remainder < 10)
            hexadecimalnum[j++] = 48 + remainder;
        else
            hexadecimalnum[j++] = 55 + remainder;
        quotient = quotient / 16;
    }
    for (int k=0,i = j; i >= 0; i--,k++){
        hex[k]=hexadecimalnum[i];

    }
}

