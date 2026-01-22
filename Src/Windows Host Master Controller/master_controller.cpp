/*****************************************************************************

                              Master Controller UI for the Missle-Bot

                                  By RCL9. April 25 to May 12, 2010

                                  (Rob's Retro Computing Archive) 
                               RetroComputingArchive@gmail.com
                                                  github.com/rcl9

*****************************************************************************/

// !! 
// !! NOTE: You cannot have the "IC4" compiler running at the same time when this 
// !!       master controller is running since they both hog the COM1 serial port.
// !!

#include	<stdio.h>
#include 	<windows.h>
#include 	<windowsx.h>
#include	"resource.h"

// http://www.codeguru.com/cpp/i-n/network/serialcommunications/article.php/c2503
#include 	"serial.h"

// Wii Remote Controller access library from "http://wiiyourself.gl.tter.org"
#include "..\wii_yourself_c++_library\wiimote.h"

static INT_PTR CALLBACK 	MainDlgProc(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam);
static BOOL 			InitApplication(HINSTANCE hInstance);
static void			NI_SendCommandToHandyBoard(char *command);
static void 			Wii_Controller__On_State_Change(wiimote &remote, state_change_flags  changed, const wiimote_state &new_state);
static void			dialog_printf(const char *fmt, ...);
static void			Wii_Controller_Print_Info(HWND hDlg, short control_id, wiimote *remote, bool connected );

static	CSerial serial;

#define APPNAME "MasterController"
#define	BUFFER_INC_VALUE	65536

// ---------------------------------------------------------------------------------------------

	int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdline, int nCmdShow)
{
	HWND 	main_dialog_hwnd;
	FARPROC	fpMainDlgProc;

	InitApplication(hInstance);

	// Show our modeless dialog box
	fpMainDlgProc = (FARPROC) MakeProcInstance( MainDlgProc, hInstance );
	main_dialog_hwnd = CreateDialog( hInstance, "MAINDLGBOX", GetFocus(), (DLGPROC) fpMainDlgProc );
	SetWindowPos( main_dialog_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

	// We have set up the HandyBoard program to receive at 1200 baud
	// 1200, 2400, 4800, 9600, 19200, 38400 and 76800.
	if (!serial.Open( 1 /* ported number, 1 to 4 */, 1200)) { 
		MessageBox( GetFocus(), "Could not open COM port # 1 at 1200 baud to communicate with the HandyBoard.\n\nExiting...", "Serial Connection Error", MB_OK | MB_APPLMODAL);
		return FALSE;
	}

	// Create a wiimote object
	wiimote remote1;
	wiimote remote2;
	bool remote1_connected = FALSE;
	bool remote2_connected = FALSE;
	
	// In this demo we use a state-change callback to get notified of
	//  extension-related events, and polling for everything else
	// (note you don't have to use both, use whatever suits your app):
	remote1.ChangedCallback	= Wii_Controller__On_State_Change;
	remote2.ChangedCallback	= Wii_Controller__On_State_Change;
	//  notify us only when the wiimote connected sucessfully, or something
	//   related to extensions changes
	remote1.CallbackTriggerFlags = (state_change_flags) ( CONNECTED | EXTENSION_CHANGED | MOTIONPLUS_CHANGED );
	remote2.CallbackTriggerFlags = (state_change_flags) ( CONNECTED | EXTENSION_CHANGED | MOTIONPLUS_CHANGED );

	MSG msg;
	while (TRUE) {
		if (!PeekMessage(&msg, (HWND) NULL, (unsigned int) NULL, (unsigned int) NULL, PM_REMOVE)) {
			Sleep(100);
		}

		if (msg.message == WM_QUIT)
			goto exit;

		if (!remote1_connected) {
			if (remote1.Connect( wiimote::FIRST_AVAILABLE )) {
                		remote1_connected = TRUE;

				// Connected - light all LEDs
				remote1.SetLEDs(0x0f);
			}
		}
		if (!remote2_connected) {
			if (remote2.Connect( wiimote::FIRST_AVAILABLE )) {
                		remote2_connected = TRUE;

				// Connected - light all LEDs
				remote2.SetLEDs(0x0f);
			}
		}

		// Go poll the Wii remotes for any state changes
		// IMPORTANT: the wiimote state needs to be refreshed each pass
		if (remote1.RefreshState() != NO_CHANGE)
			Wii_Controller_Print_Info( main_dialog_hwnd, IDD_WII_INFO1, &remote1, remote1_connected );
		if (remote2.RefreshState() != NO_CHANGE)
			Wii_Controller_Print_Info( main_dialog_hwnd, IDD_WII_INFO2, &remote2, remote2_connected );

		if ( remote1.ConnectionLost() )
			remote1_connected = FALSE;
		if ( remote2.ConnectionLost() )
			remote2_connected = FALSE;

		// If the "A" Wii remote button is pressed then we'll allow the second Wii to track the Infra-red light source
		if ( remote1_connected && remote1.Button.A() && remote2_connected ) {
			// Let's track infra-red dot # 0
			wiimote_state::ir::dot &dot = remote2.IR.Dot[0];
			
			if (dot.bVisible) {
                		if (dot.X < 0.4) {
					NI_SendCommandToHandyBoard("1");	// Start turret rotation right 
					Sleep(200);
					NI_SendCommandToHandyBoard("S");	// Stop motor
				} else if (dot.X > 0.6) {
					NI_SendCommandToHandyBoard("2");	// Start turret rotation left 
					Sleep(200);
					NI_SendCommandToHandyBoard("S");	// Stop motor
				} else {
					// Beep(100, 1000);
					remote1.PlaySquareWave(FREQ_3130HZ, 0x10); 
					Sleep(200);
					remote1.EnableSpeaker(false);

					if ( remote1.Button.B() ) {	
						// If we are tracking a dot, and the user has pressed the trigger then go fire a missile
						NI_SendCommandToHandyBoard("4");	// Fire gattling gun
						// Wait for key up
						while ( remote1.Button.B() && !remote1.ConnectionLost() )
							remote1.RefreshState();
					}
				} 
			}
		}		

		// Check the up/down/left/right buttons on the Wii remote
		if (remote1_connected) {
			// Move robot to the left
			if ( remote1.Button.Left() ) {
				NI_SendCommandToHandyBoard("L");	// Start movement left
				while ( remote1.Button.Left() && !remote1.ConnectionLost() ) 
					remote1.RefreshState();
				NI_SendCommandToHandyBoard("S");	// Stop the robot
			}

			// Move robot to the right
			if ( remote1.Button.Right() ) {
				NI_SendCommandToHandyBoard("R");	// Start movement right
				while ( remote1.Button.Right() && !remote1.ConnectionLost() ) 
					remote1.RefreshState();
				NI_SendCommandToHandyBoard("S");	// Stop the robot
			}

			// Move robot forward
			if ( remote1.Button.Up() ) {
				NI_SendCommandToHandyBoard("F");	// Start movement forward
				while ( remote1.Button.Up() && !remote1.ConnectionLost() ) 
					remote1.RefreshState();
				NI_SendCommandToHandyBoard("S");	// Stop the robot
			}


			// Move robot backwards
			if ( remote1.Button.Down() ) {
				NI_SendCommandToHandyBoard("B");	// Start movement backwards
				while ( remote1.Button.Down() && !remote1.ConnectionLost() ) 
					remote1.RefreshState();
				NI_SendCommandToHandyBoard("S");	// Stop the robot
			}

			// Gattling gun reset
			if ( remote1.Button.Minus() ) {
				NI_SendCommandToHandyBoard("5");
				// Wait for key up
				while ( remote1.Button.Minus() && !remote1.ConnectionLost() )
					remote1.RefreshState();
			}

			// Gattling rotate and fire
			if ( remote1.Button.Plus() ) {
				if ( remote2_connected ) {
					remote2.PlaySquareWave(FREQ_2470HZ, 0x30); 
					Sleep(200);
					remote2.EnableSpeaker(false);
				}

				NI_SendCommandToHandyBoard("4");
				// Wait for key up
				while ( remote1.Button.Plus() && !remote1.ConnectionLost() )
					remote1.RefreshState();
			}

			if ( remote1.ExtensionType == wiimote_state::NUNCHUK ) {
				// Gattling gun reset
				if ( remote1.Nunchuk.C ) {
					NI_SendCommandToHandyBoard("5");
					// Wait for key up
					while ( remote1.Nunchuk.C && !remote1.ConnectionLost() )
						remote1.RefreshState();
				}
        	
				// Gattling rotate and fire
				if ( remote1.Nunchuk.Z ) {
					NI_SendCommandToHandyBoard("4");
					// Wait for key up
					while ( remote1.Nunchuk.Z && !remote1.ConnectionLost() )
						remote1.RefreshState();
				}

				if ( remote1.Nunchuk.Joystick.X < -0.2) {
					NI_SendCommandToHandyBoard("2");	// Start turret rotation left 
					while ( remote1.Nunchuk.Joystick.X < -0.2 && !remote1.ConnectionLost() ) 
						remote1.RefreshState();
					NI_SendCommandToHandyBoard("S");	// Stop motor
				}
				if ( remote1.Nunchuk.Joystick.X > 0.2) {
					NI_SendCommandToHandyBoard("1");	// Start turret rotation right 
					while ( remote1.Nunchuk.Joystick.X > 0.2 && !remote1.ConnectionLost() ) 
						remote1.RefreshState();
					NI_SendCommandToHandyBoard("S");	// Stop motor
				}
			}
		}

		if (!IsDialogMessage( main_dialog_hwnd, &msg )) { 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
exit:	remote1.Disconnect();
	remote2.Disconnect();

	DestroyWindow( main_dialog_hwnd );
	FreeProcInstance( fpMainDlgProc );
}

	static INT_PTR CALLBACK 
MainDlgProc(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam)
{

	lparam = lparam;		/* Turn off warning */

	switch (msg) {
	case WM_INITDIALOG:
		return (TRUE);
	case WM_DESTROY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wparam)) {
			case IDOK:
				PostQuitMessage(0);
				return (TRUE);

			case IDCANCEL:
				PostQuitMessage(0);
				return (TRUE);

			case IDD_STOP_ALL_MOTION:
				// Stop the motion of all motors and servos
				NI_SendCommandToHandyBoard("S");
				break;

			// --------------- Robot Base Motion

			case IDD_STOP:
				NI_SendCommandToHandyBoard("S");
				break;
			case IDD_FORWARD:
				NI_SendCommandToHandyBoard("F");
				break;
			case IDD_REVERSE:
				NI_SendCommandToHandyBoard("B");
				break;
			case IDD_LEFT:
				NI_SendCommandToHandyBoard("L");
				break;
			case IDD_RIGHT:
				NI_SendCommandToHandyBoard("R");
				break;

			// --------------- Gattlign Gun Motion

			case IDD_GUN_ROT_LEFT:
				NI_SendCommandToHandyBoard("3");
				break;
			case IDD_GUN_ROT_RIGHT:
				NI_SendCommandToHandyBoard("4");
				break;
			case IDD_GUN_RESET:
				NI_SendCommandToHandyBoard("5");
				break;

			// --------------- Turret Circular Motion

			case IDD_TURRET_STOP:
				NI_SendCommandToHandyBoard("S");
				break;
			case IDD_TURRET_RIGHT:
				NI_SendCommandToHandyBoard("1");
				break;
			case IDD_TURRET_LEFT:
				NI_SendCommandToHandyBoard("2");
				break;
		}
		break;
	}
	return (FALSE);
}

// Send a single-byte ASCII command to the HandyBoard

	static void
NI_SendCommandToHandyBoard(char *command)
{
	serial.SendData( command, 1 );
}

// -------------------------------------------------------------------------

	static ATOM 
MyRegisterClass(CONST WNDCLASS *lpwc)
{
   HANDLE  hMod;
   WNDCLASSEX wcex;

   hMod = GetModuleHandle ("USER32");
   if (hMod != NULL) {

         wcex.style         = lpwc->style;
         wcex.lpfnWndProc   = lpwc->lpfnWndProc;
         wcex.cbClsExtra    = lpwc->cbClsExtra;
         wcex.cbWndExtra    = lpwc->cbWndExtra;
         wcex.hInstance     = lpwc->hInstance;
         wcex.hIcon         = lpwc->hIcon;
         wcex.hCursor       = lpwc->hCursor;
         wcex.hbrBackground = lpwc->hbrBackground;
         wcex.lpszMenuName  = lpwc->lpszMenuName;
         wcex.lpszClassName = lpwc->lpszClassName;
         wcex.cbSize = sizeof(WNDCLASSEX);
         wcex.hIconSm = LoadIcon(wcex.hInstance, "SMALL");
         return RegisterClassEx(&wcex);
   }
   return (RegisterClass(lpwc));
}

	static LRESULT CALLBACK 
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_COMMAND:
			break;
		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (0);
}

	static BOOL 
InitApplication(HINSTANCE hInstance)
{
	WNDCLASS  wc;

        // Fill in window class structure with parameters that describe
        // the main window.
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = (WNDPROC)WndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = hInstance;
        wc.hIcon         = LoadIcon ( hInstance, APPNAME );
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

	wc.lpszMenuName  = APPNAME;
	wc.lpszClassName = APPNAME;

        // Register the window class and return success/failure code.
	return RegisterClass(&wc);
}

// --------------------------- Wii Remote Controller -------------------------------


// ------------------------------------------------------------------------------------
//  state-change callback example (we use polling for everything else):
// ------------------------------------------------------------------------------------

// We use this callback to set report types etc. to respond to key events
// (like the wiimote connecting or extensions (dis)connecting).
	
// NOTE: don't access the public state from the 'remote' object here, as it will
//		  be out-of-date (it's only updated via RefreshState() calls, and these
//		  are reserved for the main application so it can be sure the values
//		  stay consistent between calls).  Instead query 'new_state' only.

	static void 
Wii_Controller__On_State_Change(wiimote &remote,
	state_change_flags  changed,
	const wiimote_state &new_state)
{
	// the wiimote just connected
	if (changed & CONNECTED) {
		// ask the wiimote to report everything (using the 'non-continous updates'
		//  default mode - updates will be frequent anyway due to the acceleration/IR
		//  values changing):

		// note1: you don't need to set a report type for Balance Boards - the
		//		   library does it automatically.
		
		// note2: for wiimotes, the report mode that includes the extension data
		//		   unfortunately only reports the 'BASIC' IR info (ie. no dot sizes),
		//		   so let's choose the best mode based on the extension status:
		if(new_state.ExtensionType != wiimote::BALANCE_BOARD) {
			if(new_state.bExtension)
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
			else
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);		//    IR dots
		}
	}

	// a MotionPlus was detected
	if (changed & MOTIONPLUS_DETECTED) {
		// enable it if there isn't a normal extension plugged into it
		// (MotionPlus devices don't report like normal extensions until
		//  enabled - and then, other extensions attached to it will no longer be
		//  reported (so disable the M+ when you want to access them again).
		if(remote.ExtensionType == wiimote_state::NONE) {
			bool res = remote.EnableMotionPlus();
			_ASSERT(res);
		}

	// an extension is connected to the MotionPlus
	} else if(changed & MOTIONPLUS_EXTENSION_CONNECTED) {
		// We can't read it if the MotionPlus is currently enabled, so disable it:
		if(remote.MotionPlusEnabled())
			remote.DisableMotionPlus();

	// an extension disconnected from the MotionPlus
	} else if(changed & MOTIONPLUS_EXTENSION_DISCONNECTED)
		{
		// enable the MotionPlus data again:
		if(remote.MotionPlusConnected())
			remote.EnableMotionPlus();

	// another extension was just connected:
	} else if(changed & EXTENSION_CONNECTED) {
		// switch to a report mode that includes the extension data (we will
		//  loose the IR dot sizes)
		// note: there is no need to set report types for a Balance Board.
		if(!remote.IsBalanceBoard())
			remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);

	// extension was just disconnected:
	} else if(changed & EXTENSION_DISCONNECTED) {
		// use a non-extension report mode (this gives us back the IR dot sizes)
		remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
	}
}

static char	*text_display_buffer;
static INT_PTR text_display_buffer_length, curr_text_display_buffer_length;

	static void
Wii_Controller_Print_Info(HWND hDlg, short control_id, wiimote *remote, bool connected )
{
	text_display_buffer = (char *) NULL;
	text_display_buffer_length = 0;
	curr_text_display_buffer_length = 0;

	if (!connected) {
		dialog_printf("Not connected\n");
		goto exit;
	}

	dialog_printf("Connected, ID = 0x%x\n", remote->UniqueID);

	// Battery level:
	dialog_printf( "Battery: %3u%%\n", remote->BatteryPercent);

	// Buttons:
	dialog_printf( "Buttons: " ); 
	dialog_printf( "[" );
	for (unsigned bit=0; bit<16; bit++) {
		WORD mask = (WORD)(1 << bit);
		// skip unused bits
		if((wiimote_state::buttons::ALL & mask) == 0)
			continue;

		const TCHAR* button_name = wiimote::ButtonNameFromBit[bit];
		bool	pressed	 = ((remote->Button.Bits & mask) != 0);

		// if (bit > 0) 
		//	dialog_printf( "|" ); // seperator

		if (pressed) 
			dialog_printf( "%s ", button_name);
		//else
		//	dialog_printf( "%*s", strlen(button_name), "" );
	}
	dialog_printf( "]\n\n" );

	// Acceleration:
	dialog_printf( "Accel: X: %+2.3f, Y: %+2.3f, Z: %+2.3f\n\n",
		remote->Acceleration.X, remote->Acceleration.Y, remote->Acceleration.Z);

	// Orientation estimate (shown red if last valid update is aging):
	dialog_printf( "Orientation: " );
	dialog_printf( " (update age = %3u)\n", remote->Acceleration.Orientation.UpdateAge);
		
	//  show if the last orientation update is considered out-of-date
	//   (using an arbitrary threshold)
	// if(remote.Acceleration.Orientation.UpdateAge > 10)
	//	RED;
			
	dialog_printf( "  Pitch: %4ddeg\n  Roll: %4ddeg\n  X: %+.3f, Y: %+.3f, Z: %+.3f\n\n",
		(int) remote->Acceleration.Orientation.Pitch,
		(int) remote->Acceleration.Orientation.Roll ,
		remote->Acceleration.Orientation.X,
		remote->Acceleration.Orientation.Y,
  		remote->Acceleration.Orientation.Z);
				
	// -- Extensions --
	switch( remote->ExtensionType ) {
		// -- Nunchuk --
		case wiimote_state::NUNCHUK:
			dialog_printf( "Nunchuk info:\n");

			// Buttons:
			dialog_printf( "  Buttons: " ); 
			dialog_printf( "[" );
			dialog_printf( remote->Nunchuk.C ? "C" : " " );
			dialog_printf( "|" );
			dialog_printf( remote->Nunchuk.Z ? "Z" : " " );
			dialog_printf( "]\n" );

			// Joystick:
			dialog_printf( "  Joystick: X: %+2.3f, Y: %+2.3f\n",
				remote->Nunchuk.Joystick.X, remote->Nunchuk.Joystick.Y);

			// Acceleration:
			dialog_printf( "  Accel: X: %+2.3f, Y %+2.3f, Z: %+2.3f\n",
				remote->Nunchuk.Acceleration.X,
				remote->Nunchuk.Acceleration.Y,
				remote->Nunchuk.Acceleration.Z);
				
			// Orientation estimate (shown red if last valid update is aging):
			dialog_printf( "  Orientation (update age = %3u)\n", 
				remote->Nunchuk.Acceleration.Orientation.UpdateAge);

			//  show if the last orientation update is aging
			// if(remote.Nunchuk.Acceleration.Orientation.UpdateAge > 10)
			//	RED;

			dialog_printf( "  Pitch: %4ddeg\n  Roll: %4ddeg\n  (X: %+.2f, Y: %+.2f, Z: %+.2f)\n",
				(int) remote->Nunchuk.Acceleration.Orientation.Pitch,
				(int) remote->Nunchuk.Acceleration.Orientation.Roll ,
				remote->Nunchuk.Acceleration.Orientation.X,
				remote->Nunchuk.Acceleration.Orientation.Y,
 				remote->Nunchuk.Acceleration.Orientation.Z );
			break;
  	}

	// IR:
	dialog_printf( "\nIR: Mode %s ",
		((remote->IR.Mode == wiimote_state::ir::OFF     )? "OFF  " :
		 (remote->IR.Mode == wiimote_state::ir::BASIC   )? "BASIC" :
		 (remote->IR.Mode == wiimote_state::ir::EXTENDED)? "EXT. " :
		"FULL " ));

	// IR dot sizes are only reported in EXTENDED IR mode (FULL isn't supported yet)
	bool dot_sizes = (remote->IR.Mode == wiimote_state::ir::EXTENDED);

	dialog_printf( "\n");
	for(unsigned index=0; index<4; index++) {
		wiimote_state::ir::dot &dot = remote->IR.Dot[index];
			
		dialog_printf( "%u: ", index);

		if(dot.bVisible) {
			dialog_printf( "Seen, ");
		} else {
			dialog_printf( "Not seen, ");
		}

		dialog_printf( "Size: " );
		if(dot_sizes)
			 dialog_printf( "%3d, ", dot.Size);
		else {
			dialog_printf( "n/a, " );
			// if(dot.bVisible) WHITE;
		}

		dialog_printf( " X: %.3f, Y: %.3f\n", dot.X, dot.Y );
	}


        
exit:	text_display_buffer[curr_text_display_buffer_length++] = '\0';
	SendMessage( GetDlgItem(hDlg, control_id), WM_SETTEXT, 0, (LPARAM) text_display_buffer);
	if (text_display_buffer != (char *) NULL)
		free( (char *) text_display_buffer );
}

// Format then append a line of text to the text display buffer 

	static void
dialog_printf(const char *fmt, ...)
{
	static short curr_offset;
	va_list	marker;
	char	text[1024];
	short	len, i;

	va_start(marker, fmt);
	if (!(len = vsprintf(text, fmt, marker)))
		return;

	if (!text_display_buffer_length) {
		text_display_buffer = (char *) malloc(BUFFER_INC_VALUE);
		text_display_buffer[0] = '\0';
		curr_offset = 0;
	}

	for (i=0; i < len; ++i) {
		if (16 + curr_text_display_buffer_length >= text_display_buffer_length) {
			text_display_buffer_length += BUFFER_INC_VALUE;
			text_display_buffer = (char *) realloc(text_display_buffer, text_display_buffer_length);
		}

		if (text[i] == '\n') {
			text_display_buffer[curr_text_display_buffer_length++] = 0xd;
			text_display_buffer[curr_text_display_buffer_length++] = 0xa;
			curr_offset = 0;
		} else if (text[i] == '\t') {
			do {
				text_display_buffer[curr_text_display_buffer_length++] = ' ';
				++curr_offset;
			} while ((curr_offset % 8) != 0);
		} else {
			text_display_buffer[curr_text_display_buffer_length++] = text[i];
			++curr_offset;
		}
	}
}

