#include "Core/mainManager.h"

int main()
{
	Core::MainManager manager(1280, 720);

	manager.render();

	return 0;
}
//
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//
//int main()
//{
//	_CrtMemState start;
//	_CrtMemCheckpoint(&start);
//
//	int returnValue = appMain();
//
//	_CrtMemState end;
//	_CrtMemCheckpoint(&end);
//
//	_CrtMemState difference;
//	if (_CrtMemDifference(&difference, &start, &end))
//	{
//		printf("---------- _CrtMemDumpStatistics ----------\n\n");
//		_CrtMemDumpStatistics(&difference);
//		printf("\n---------- _CrtMemDumpAllObjectsSince ----------\n\n");
//		_CrtMemDumpAllObjectsSince(&end);
//		printf("\n---------- _CrtMemDumpMemoryLeaks ----------\n\n");
//		_CrtDumpMemoryLeaks();
//
//		if (!returnValue)
//		{
//			return -1;
//		}
//	}
//
//	return returnValue;
//}