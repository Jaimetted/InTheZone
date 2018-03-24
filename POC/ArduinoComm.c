#pragma config(UART_Usage, UART1, uartUserControl, baudRate9600, IOPins, None, None)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define CHARS_PER_MESS 24
#define CHARS_PER_VAL 8

int rcvChar;
int startChar = 120; // Delimeter for messages char value = 'x'
task main()
{
  setBaudRate(uartOne, baudRate9600);

  while (getChar(uartOne) != -1) // Purge existing chars from buffer
  {}

	while (true)
  {
    rcvChar = getChar(uartOne);

    if (rcvChar == -1)
    {
      // No character available

      wait1Msec(2); // Don't want to consume too much CPU time. Waiting eliminates CPU consumption for this task.
      continue;
    }
    if (rcvChar == startChar){
    	int charCounter = 0;
    	while(charCounter < CHARS_PER_MESS){
	    	rcvChar = getChar(uartOne);
		    if (rcvChar == -1)
		    {
		      // No character available

		      wait1Msec(2); // Don't want to consume too much CPU time. Waiting eliminates CPU consumption for this task.
		      continue;
		    }
	    	writeDebugStream("%c", rcvChar);
	    	charCounter++;
    	}
    	writeDebugStream("\n");
    }
  }
}
