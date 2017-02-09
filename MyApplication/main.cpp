// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MyApplication.h"

int main()
{
	MyApplication* app = new MyApplication();

	app->run("My Application", 1280, 720, false);

	delete app;

    return 0;
}

