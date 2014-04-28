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
#include <getopt.h>
#include <stdlib.h>
//#include <sys/types.h>
//#include <string.h>
//#include <json/json.h>
//#include <curl/curl.h>

#include <openstack/keystone.h>
#include <openstack/swift.h>


int debug = 0;

int main (int argc, char **argv)
{
	char *username   = getenv("OS_USERNAME");
	char *tenantname = getenv("OS_TENANT_NAME");
	char *password   = getenv("OS_PASSWORD");
	char *auth_url   = getenv("OS_AUTH_URL");
	
	struct keystone *keystone;
	int this_option_optind = optind ? optind : 1;
	int opt_idx = 0;
	static struct option long_options[] = {
		{"os-username", required_argument, NULL, 0},
		{"os-password", required_argument, NULL, 1},
		{"os-tenant-name", required_argument, NULL, 2},
		{"os-auth-url", required_argument, NULL, 3},
		{"debug", required_argument, NULL, 4},
		{0, 0, 0, 0}
	};
	int c;
	while ((c = getopt_long(argc, argv, "u:p:t:a:d", long_options, &opt_idx)) != -1) {
		switch (c) {
		case '?':
			exit(1);
			break;
		case 0:
		case 'u':
			username = optarg;
			break;
		case 1:
		case 'p':
			password = optarg;
			break;
		case 2:
		case 't':
			tenantname = optarg;
			break;
		case 3:
		case 'a':
			auth_url = optarg;
			break;
		case 4:
		case 'd':
			debug = 1;
		}
	}

	if (username == NULL || tenantname == NULL || password == NULL ||
		auth_url == NULL) {
		printf("Specify username, tenantname, password and auth_url");
		exit(1);
	}

	keystone = keystone_authenticate(username, tenantname, password,
					 auth_url);

	swift_stat(keystone);

	return 0;
}
