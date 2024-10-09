


#pragma once

#ifndef __AFXWIN_H__
#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.


// Cipc2023App:
// 이 클래스의 구현에 대해서는 ipc2023.cpp을(를) 참조하세요.
//

class Cipc2023App : public CWinApp
{
public:
	Cipc2023App();

	// 재정의입니다.
public:
	virtual BOOL InitInstance();

	// 구현입니다.

	DECLARE_MESSAGE_MAP()
};

extern Cipc2023App theApp;
