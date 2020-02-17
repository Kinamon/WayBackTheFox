#include <stdio.h>
#include <stdlib.h>

#include "CXInput.h"


void main()
{
	clsXInput *pClsXInput = NULL;
	pClsXInput = new clsXInput;

	while (1) {
		pClsXInput->UpdateStatus();//これ入れないと動かない.

		//十字.
		printf("Pad[");
		if (pClsXInput->IsPress(XINPUT_GAMEPAD_DPAD_UP)) {
			printf("8,");
		}
		if (pClsXInput->IsPress(XINPUT_GAMEPAD_DPAD_DOWN)) {
			printf("2,");
		}
		if (pClsXInput->IsPress(XINPUT_GAMEPAD_DPAD_LEFT)) {
			printf("4,");
		}
		if (pClsXInput->IsPress(XINPUT_GAMEPAD_DPAD_RIGHT)) {
			printf("6,");
		}
		//printf("], Button[");
		//if (pClsXInput->IsPress(XINPUT_GAMEPAD_START)) {
		//	printf("Start,");
		//}
		//if (pClsXInput->IsPress(XINPUT_GAMEPAD_BACK)) {
		//	printf("Back,");
		//}
		//if (pClsXInput->IsPress(XINPUT_GAMEPAD_LEFT_THUMB)) {
		//	printf("LT,");
		//}
		//if (pClsXInput->IsPress(XINPUT_GAMEPAD_RIGHT_THUMB)) {
		//	printf("RT,");
		//}
		//if (pClsXInput->IsPress(XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		//	printf("LS,");
		//}
		//if (pClsXInput->IsPress(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		//	printf("RS,");
		//}
		//ボタン取得.
		if (pClsXInput->IsPress(XINPUT_GAMEPAD_A)) {
			printf("A,");
			pClsXInput->SetVibration(INPUT_VIBRATION_MAX, 0);
		}
		if (pClsXInput->IsPress(XINPUT_GAMEPAD_B)) {
			printf("B,");
			pClsXInput->SetVibration(0, INPUT_VIBRATION_MAX);
		}
		if (pClsXInput->IsPress(XINPUT_GAMEPAD_X)) {
			printf("X,");
			pClsXInput->SetVibration(0, 0);
		}
		if (pClsXInput->IsPress(XINPUT_GAMEPAD_Y)) {
			printf("Y,");
			pClsXInput->SetVibration(INPUT_VIBRATION_MAX, INPUT_VIBRATION_MAX);
		}
		printf("], ");
		printf("Trg[L:%d,R:%d], ",
			pClsXInput->GetLTrigger(),
			pClsXInput->GetRTrigger());
		printf("StL[X:%d,Y:%d], ",
			pClsXInput->GetLThumbX(),
			pClsXInput->GetLThumbY());
		printf("StR[X:%d,Y:%d]\n",
			pClsXInput->GetRThumbX(),
			pClsXInput->GetRThumbY());

		if (GetAsyncKeyState(VK_F1) & 0x0001) {
			if (IDOK == MessageBox(0, "終了する？", "警告", MB_OKCANCEL)) {
				break;
			}
		}
	}
	pClsXInput->EndProc();
	XInputEnable( false );


	system("pause");
}