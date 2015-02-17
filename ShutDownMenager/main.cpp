#include <windows.h>
#include "resource.h" //needed for the accelerators table
//Win proc
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
//Starts the timer
void ActivateTimer(HWND hwnd);
//Shut down windows
bool MySystemShutdown();


HINSTANCE				hGlobalInstance;
HWND					hEditControl, hButtonControl; //button handles
static unsigned int		iShutDownTime_inSeconds; //Timer value

char szBuffer[50]; //Temp buffer
BOOL bTemp; //Temp boolean	  

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     static TCHAR	szAppName[] = TEXT ("ShutDown") ;
     HWND         	hwnd ;
     MSG          	msg ;
     WNDCLASS     	wndclass ;
	 HACCEL			hAccel ; //handle for the accelerators table

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
     hwnd = CreateWindow (szAppName,                  	// window class name
                          TEXT ("ShutDown Manager"),	// window caption
                          WS_OVERLAPPEDWINDOW,        	// window style
                          CW_USEDEFAULT,              	// initial x position
                          CW_USEDEFAULT,              	// initial y position
                          200,              			// initial x size
                          100,              			// initial y size
                          NULL,             			// parent window handle
                          NULL,                       	// window menu handle
                          hInstance,                  	// program instance handle
                          NULL) ;                     	// creation parameters
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
	 //Load the accelerators table
	 hAccel = LoadAccelerators (hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1) ) ;

     while (GetMessage (&msg, NULL, 0, 0))
	 {
		if (!TranslateAccelerator (hwnd, hAccel, &msg))
    	{
    	     TranslateMessage (&msg) ;
    	     DispatchMessage (&msg) ;
    	}
}


}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	 HDC					 hdc ;
     PAINTSTRUCT			 ps ;
     RECT					 rect ;
     
     switch (message)
     {
     case WM_CREATE:
          hEditControl=CreateWindow(TEXT("edit"), NULL, 
                          WS_CHILD | WS_VISIBLE | WS_BORDER , //window style
                          75,              // initial x position
                          20,              // initial y position
                          50,              // initial x size
                          20,              // initial y size
                          hwnd,            // parent window handle
                          (HMENU)1,        // window menu handle
                          hGlobalInstance, // program instance handle
                          NULL) ;          // creation para
		  
		  CreateWindow(TEXT("button"), TEXT("Activate timer!"), 
                          WS_CHILD | WS_VISIBLE | WS_BORDER , //window style
                          5,               // initial x position
                          50,              // initial y position
                          120,             // initial x size
                          20,              // initial y size
                          hwnd,            // parent window handle
                          (HMENU)2,        // window menu handle
                          hGlobalInstance, // program instance handle
                          NULL) ;          // creation para
		   hButtonControl=CreateWindow(TEXT("button"), TEXT("left"), 
                          WS_CHILD | WS_VISIBLE | WS_BORDER , //window style
                          130,             // initial x position
                          20,              // initial y position
                          60,              // initial x size
                          20,              // initial y size
                          hwnd,            // parent window handle
                          (HMENU)3,        // window menu handle
                          hGlobalInstance, // program instance handle
                          NULL) ;          // creation para

          return 0 ;

     case WM_PAINT:
          hdc = BeginPaint (hwnd, &ps) ;    
         
		  GetClientRect (hwnd, &rect) ;
		  TextOut(hdc, 0,0,TEXT("Set the ShutDown Timer."),strlen("Set the ShutDown Timer."));
		  TextOut(hdc, 0,18,TEXT("in minutes:"),strlen("in minutes:"));
          
		  EndPaint (hwnd, &ps) ;
          return 0 ;
	 case WM_COMMAND: 

		  switch (LOWORD (wParam))
          {
		  case 2:	//Button "Activate timer!" was pressed		
			  ActivateTimer(hwnd);
			  break;
		  case ID_ACCELERATOR40002:	//Enter was pressed (look in the accelerators table)
			  ActivateTimer(hwnd);
			  break;
		  }

		  return 0;

	 case WM_KEYDOWN:
		 //Enter was pressed
		  if( wParam == VK_RETURN )
		  {
			 ActivateTimer(hwnd);
		  }

		  return 0;

	 case WM_TIMER:	//Timer ticks every second

		  iShutDownTime_inSeconds--;
		  wsprintf((LPWSTR)szBuffer,TEXT("%d"), iShutDownTime_inSeconds);		
		  SetWindowText(hButtonControl,(LPWSTR)szBuffer);
		  if(iShutDownTime_inSeconds<1){
			 MySystemShutdown();
			 //MessageBox(hwnd,TEXT("shutdown now"), TEXT("note"),MB_OK);// For testing purpose
		  }

		  return 0;
	 case WM_SETFOCUS:// If the window acquires the focus 
		 //set the focus to the edit box
		 SetFocus(hEditControl);
		 return 0;

     case WM_DESTROY:
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}

//Starts the timer
void ActivateTimer(HWND hwnd)
{
	InvalidateRect(hwnd,NULL,TRUE);
		
	KillTimer(hwnd, 10); //If the timer is running stop it
	
	//Read the value from the edit box
	iShutDownTime_inSeconds=GetDlgItemInt(hwnd,1,&bTemp,FALSE);
	if(!bTemp) //Check for valid input
		MessageBox(hwnd, TEXT("Incorrect value!"),TEXT("Error"),MB_OK);

	iShutDownTime_inSeconds *= 60; //Convert to seconds
			
	if(iShutDownTime_inSeconds>0)
	{	
		if(!SetTimer(hwnd, 10,1000,NULL)) //Start WM_TIMER to tick every second
			MessageBox(hwnd, TEXT("Couldn't start the timer"),TEXT("Couldn't start the timer"),MB_OK);
		
	}
}

//Shut down windows
bool MySystemShutdown()
{
   HANDLE hToken; 
   TOKEN_PRIVILEGES tkp; 
 
   // Get a token for this process. 
 
   if (!OpenProcessToken(GetCurrentProcess(), 
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
      return( FALSE ); 
 
   // Get the LUID for the shutdown privilege. 
 
   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
        &tkp.Privileges[0].Luid); 
 
   tkp.PrivilegeCount = 1;  // one privilege to set    
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
   // Get the shutdown privilege for this process. 
 
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
        (PTOKEN_PRIVILEGES)NULL, 0); 
 
   if (GetLastError() != ERROR_SUCCESS) 
      return FALSE; 
 
   // Shut down the system and force all applications to close. 
 
   if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_FLAG_PLANNED)) 
      return FALSE; 

   return TRUE;
}
