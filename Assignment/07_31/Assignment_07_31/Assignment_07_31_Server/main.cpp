#include "Network.h"
#include "Contents.h"

using namespace NetworkLibrary;

int wmain()
{
	Contents* content = new Contents();
	Network* network = new Network(content);
}