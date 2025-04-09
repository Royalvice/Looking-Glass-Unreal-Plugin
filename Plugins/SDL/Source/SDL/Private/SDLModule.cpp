#include "ISDLModule.h"

class FSDLModule: public ISDLModule
{
	void StartupModule() override
	{
	}

	void ShutdownModule() override
	{
	}
};

IMPLEMENT_MODULE(FSDLModule, SDL)
