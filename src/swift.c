/*
 * Copyright (C) 2014, Masanori Itoh.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Masanori Itoh <masanori.itoh@gmail.com>
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <json/json.h>
#include <curl/curl.h>

#include <openstack/keystone.h>
#include <openstack/swift.h>

/*
struct swift_callback_data {
	int command;
};
*/

size_t swift_header_func(void *buffer, size_t size, size_t nmemb, void *userp)
{
  //printf("swift_header_func called: %p %d %d %p\n", buffer, size, nmemb, userp);
	//	json_object *jobj;
	//printf("DEBUG: '%s'", buffer);
	write(1, buffer, nmemb);
	return nmemb;
}


static void swift_read_common(struct keystone *keystone)
{
	;
}

void swift_capabilities(struct keystone *keystone)
{
	return;
}

void swift_list_container(struct keystone *keystone, char *container)
{
	return;
}

void swift_list(struct keystone *keystone)
{
	return;
}

void swift_stat_container(struct keystone *keystone, char *container)
{
	return;
}
/*
 * swift_stat
 *
 */
void swift_stat(struct keystone *keystone)
{
	CURL *handle;
	CURLcode ret;
	char *swift_url, *token;
	struct curl_slist *header = NULL; 
	char buf[8192];

	//printf("DEBUG swift_stat called.\n");
	swift_url = keystone_get_endpoint(keystone, "object-store", "publicURL");
	//printf("DEBUG: publicURL: %s\n", swift_url);

	curl_global_init(CURL_GLOBAL_ALL);
	handle = curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_URL, swift_url);
	//NOTE(thatsdone): the below generates stdout messages...
	//curl_easy_setopt(handle, CURLOPT_HEADER, 1);
	//NOTE(thatsdone): to use HTTP HEAD, only CURLOPT_NOBODY is necessory.
	curl_easy_setopt(handle, CURLOPT_NOBODY, 1);
	curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, swift_header_func);
	//TODO(thatsdone): allocate a structure and set results.
	curl_easy_setopt(handle, CURLOPT_HEADERDATA, NULL);
	/*
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, swift_header_func);
	curl_easy_setopt(handle, CURLOPT_WRITEHEADER, NULL);
	*/
	//header = curl_slist_append(header, "Content-Type: application/json");
	token = keystone_get_token(keystone);
	//
	memset(buf, 0x0, sizeof(token));
	snprintf(buf, sizeof(buf), "X-Auth-Token: %s", token);
	header = curl_slist_append(header, buf);
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header);

	curl_easy_perform(handle);

	curl_slist_free_all(header);
	curl_easy_cleanup(handle);
	
	return;
}
