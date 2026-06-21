/*
  The Forgotten Client
  Copyright (C) 2020 Saiyans King

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef __FILE_HTTP_h_
#define __FILE_HTTP_h_

#include "defines.h"

enum HttpResult
{
	HTTP_RESULT_FAILED = 0,
	HTTP_RESULT_SUCCEEDED = 1
};

struct HttpRequest
{
	void (*eventHandlerFunction)(Uint32,Sint32);
	void* easy_handle;
	void* mime_handle;
	SDL_RWops* file_handle;
	char* json_data;
	HttpResult result;
	Uint32 internalId;
	Uint32 evtParam;
};

class Http
{
	public:
		Http();
		~Http();

		Uint32 addRequest(const std::string& urlRequest, const std::string& fileName, const std::string& jsonData, void (*eventHandlerFunction)(Uint32,Sint32), Uint32 mEvent);
		Uint32 addDiscordWebhook(const std::string& urlRequest, const std::string& content, const std::string& attachmentFile,
			void (*eventHandlerFunction)(Uint32,Sint32), Uint32 mEvent);
		HttpRequest* getRequest(Uint32 internalId);
		void removeRequest(Uint32 internalId, bool cleanup = true);
		void updateHttp();

	protected:
		void handleResult(void* handle, Sint32 result);

	private:
		std::vector<HttpRequest> m_httpRequests;
		void* m_curlJsonHeader;
		void* m_curlHandle;
		Sint32 m_remainingHandles;
};

#endif /* __FILE_HTTP_h_ */
