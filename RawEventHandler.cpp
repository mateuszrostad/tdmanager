#include <cstring> // strlen
#include <cstdlib> // atoi
#include "RawEventHandler.hpp"


RemoteEventData* getRemoteEventData(const char* data)
{
	RemoteEventData* eventData = new RemoteEventData;

	char datatokens[strlen(data)+1];
	strcpy(datatokens, data);
	
	char* tokens_ptr = strtok(datatokens, ";");
	char* value_ptr;
	while (tokens_ptr != NULL)
	{
		if ( (value_ptr = strchr(tokens_ptr, ':')) )
		{
			if      (strncmp(tokens_ptr, "class", 5) == 0)
			{
				if (strncmp(value_ptr+1, "command", 7) == 0)
					eventData->class_   = command;
			}
			else if (strncmp(tokens_ptr, "protocol", 8) == 0)
			{
				if (strncmp(value_ptr+1, "arctech", 7) == 0)
					eventData->protocol = arctech;
			}
			else if (strncmp(tokens_ptr, "model", 5) == 0)
			{
				if (strncmp(value_ptr+1, "selflearning", 12) == 0)
					eventData->model    = selflearning;
			}
			else if (strncmp(tokens_ptr, "house", 5) == 0)
				eventData->house = atoi(value_ptr+1);

			else if (strncmp(tokens_ptr, "unit", 4) == 0)
				eventData->unit  = atoi(value_ptr+1);

			else if (strncmp(tokens_ptr, "group", 5) == 0)
				eventData->group = atoi(value_ptr+1);

			else if (strncmp(tokens_ptr, "code", 4) == 0)
				eventData->code  = atoi(value_ptr+1);

			else if (strncmp(tokens_ptr, "method", 6) == 0)
			{
				if      (strncmp(value_ptr+1, "turnon", 6) == 0)
					eventData->method        = turnon;
				else if (strncmp(value_ptr+1, "turnoff", 7) == 0)
					eventData->method        = turnoff;
			}
		}
		tokens_ptr = strtok(NULL, ";");
	}
	
	return eventData;
}

