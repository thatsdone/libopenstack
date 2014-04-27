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


//#include <json/json.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <string.h>
//#include <curl/curl.h>
#include <stdlib.h>

#include <openstack/keystone.h>
#include <openstack/swift.h>


extern void swift_stat(struct keystone *);

/*
OS_AUTH_URL=http://10.10.1.77:5000/v2.0/
OS_PASSWORD=13a3912e3b424c96
OS_TENANT_NAME=admin
OS_USERNAME=admin
 */
int main (int argc, char **argv)
{
	char *username   = getenv("OS_USERNAME");
	char *tenantname = getenv("OS_TENANT_NAME");
	char *password   = getenv("OS_PASSWORD");
	char *auth_url   = getenv("OS_AUTH_URL");
	
	struct keystone *keystone;

	keystone = keystone_authenticate(username, tenantname, password,
					 auth_url);

	swift_stat(keystone);

	return 0;
}
