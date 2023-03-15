#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<dos.h>
#include <sys/time.h>


void receive(char[]);
HANDLE hComm;                          // Handle to the Serial port
char  ComPortName[] = "\\\\.\\COM7";  // Name of the Serial port(May Change) to be opened,
BOOL  Status;                          // Status of the various operations
DWORD dwEventMask;                     // Event mask to trigger
char  TempChar;                        // Temperory Character
static DWORD  dNoOFBytestoWrite = 0;          // No of bytes to write into the port
static DWORD  dNoOfBytesWritten = 0;          // No of bytes written to the port
static DWORD NoBytesRead = 0 ;                     // Bytes read by ReadFile()
static char  SerialBuffer[655366];   // Buffer Containing Rxed Data
char    var = '\r';
static int i;
static char packet[655366];
static char buffer[655366];
char lpBuffer[15]= "";
struct timeval stop, start;

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

			dcbSerialParams.BaudRate = CBR_38400;      // Setting BaudRate = 38400
			dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
			dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
			dcbSerialParams.Parity = NOPARITY;        // Setting Parity = None

			Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB

			if (Status == FALSE)
				{
					printf("\n    Error! in Setting DCB Structure");
				}
			else //If Successfull display the contents of the DCB Structure
				{
					printf("\n\n    Setting DCB Structure Successfull\n");
					printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
					printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
					printf("\n       StopBits = %d", dcbSerialParams.StopBits);
					printf("\n       Parity   = %d", dcbSerialParams.Parity);
				}

            /*------------------------------------ Setting Timeouts --------------------------------------------------*/


			COMMTIMEOUTS timeouts = { 0 };
			timeouts.ReadIntervalTimeout         = 50;
			timeouts.ReadTotalTimeoutConstant    = 1000;
			timeouts.ReadTotalTimeoutMultiplier  = 0   ;
			timeouts.WriteTotalTimeoutConstant   = 100;
			timeouts.WriteTotalTimeoutMultiplier = 1;

			if (SetCommTimeouts(hComm, &timeouts) == FALSE)
				printf("\n\n    Error! in Setting Time Outs");
			else
				printf("\n\n    Setting Serial Port Timeouts Successful");
            printf_s("\n>");

       while(1){
            memset(SerialBuffer,0,sizeof(SerialBuffer));
            memset(lpBuffer,0,sizeof(lpBuffer));
            scanf_s("%s", &lpBuffer, (unsigned)_countof(lpBuffer));
            strncat(lpBuffer, &var, 1);
            receive(lpBuffer);
            send1();
            Sleep(1);
       }
				CloseHandle(hComm);//Closing the Serial Port
				printf("\n +==========================================+\n");
				_getch();
}//End of Main()


void receive(char lpBuffer1[])
{
    dNoOFBytestoWrite = 0;          // No of bytes to write into the port
    dNoOfBytesWritten = 0;          // No of bytes written to the port
    NoBytesRead = 0 ;
    dNoOFBytestoWrite = (strlen(lpBuffer));
    Status = WriteFile(hComm,               // Handle to the Serialport
                    lpBuffer1,            // Data to be written to the port
                    dNoOFBytestoWrite,   // No of bytes to write into the port
                    &dNoOfBytesWritten,  // No of bytes written to the port
                    NULL);

    if (Status == FALSE){
       printf("\n\n   Error %d in Writing to Serial Port",GetLastError());
    }
}


void send1()
{
    Status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception
    if (Status == FALSE){
        printf("\n\n    Error! in Setting CommMask");
    }
    /*------------------------------------ Setting WaitComm() Event   ----------------------------------------*/

    Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received

    /*-------------------------- Program will Wait here till a Character is received ------------------------*/

    if (Status == FALSE)
    {
        printf("\n    Error! in Setting WaitCommEvent()");
    }
    else //If  WaitCommEvent()==True Read the RXed data using ReadFile();
    {
        int at=0;
        gettimeofday(&start, NULL);
        do
        {
            Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
            SerialBuffer[i] = TempChar;
            i++;
            at++;
            if(at>=763)
            {
                gettimeofday(&start, NULL);
                stop1();
                goto here;

            }
        }
        while ((NoBytesRead > 0) || (at>763));

        /*------------Printing the RXed String to Console----------------------*/
       here:
        if((SerialBuffer[6]== '1' && SerialBuffer[7]== '8') || (SerialBuffer[6]== '0' && SerialBuffer[7]== 'C'))
        {
            int m=0,j=0,z,y,a=9;
            printf("{'P':");
            for (m = 0; m < 6; m++)
            {
                if(SerialBuffer[a]==' ' || SerialBuffer[a]=='\n')
                {
                    a++;
                    continue;
                }
                else
                {
                    packet[m]=SerialBuffer[a];
                    a++;
                    printf("%c",packet[m]);
                }
            }
            printf("=");
            a=a+3;
            for ( j = 0; j < 24; j++)
            {
                if(SerialBuffer[a]==' ' || SerialBuffer[a]=='\n')
                {
                    a++;
                    continue;
                }
                else
                {
                    buffer[j]=SerialBuffer[a];
                    a++;
                    printf("%c",buffer[j]);
                }
            }

            int aa=0;
            while(aa<19)
            {
                if((SerialBuffer[a]== '1' && SerialBuffer[a+1]== '8') || (SerialBuffer[a]== '0' && SerialBuffer[a+1]== 'C'))
                {
                    aa++;
                    a=a+3;
                    printf(",");
                    z=m+6;
                    y=j+24;
                    for (;m < z;m++)
                    {
                        if(SerialBuffer[a]==' ' || SerialBuffer[a]=='\n')
                        {
                            a++;
                            continue;
                        }
                        else
                        {
                            packet[m]=SerialBuffer[a];
                            a++;
                            printf("%c",packet[m]);
                        }
                    }
                    printf("=");
                    a=a+3;
                    for (; j < y; j++)
                    {
                        if(SerialBuffer[a]==' ' || SerialBuffer[a]=='\n')
                        {
                            a++;
                            continue;
                        }
                        else
                        {
                            buffer[j]=SerialBuffer[a];
                            a++;
                            printf("%c",buffer[j]);
                        }
                    }
                }
                a++;
            }

            printf("}\n\n");
            printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);


        }
        else
        {
            for (int j = 0; j < i-1; j++)
            {
                printf_s("%c", SerialBuffer[j]);
            }
        }
    }
    memset(lpBuffer,0,sizeof(lpBuffer));
    i=0;

    }

void stop1()
{
    dNoOFBytestoWrite = 0;          // No of bytes to write into the port
    dNoOfBytesWritten = 0;          // No of bytes written to the port
    char lp[15]="atws\r";
    dNoOFBytestoWrite = (strlen(lp));
    Status = WriteFile(hComm,               // Handle to the Serialport
                    lp,            // Data to be written to the port
                    dNoOFBytestoWrite,   // No of bytes to write into the port
                    &dNoOfBytesWritten,  // No of bytes written to the port
                    NULL);
}
