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
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <json/json.h>
#include <curl/curl.h>

#include <openstack/keystone.h>

static const char *key_user = "username";
static const char *key_pass = "password";
static const char *key_tenant = "tenantName";
static const char *key_pw_cred = "passwordCredentials";
static const char *key_auth = "auth";

static json_object *keystone_build_credential(char *user, char *pass, char *tenant)
{
	json_object *jo_auth = json_object_new_object();
	json_object *jo_tenant = json_object_new_object();
	json_object *jo_cred = json_object_new_object();

	json_object_object_add(jo_cred, key_user, json_object_new_string(user));
	json_object_object_add(jo_cred, key_pass, json_object_new_string(pass));

	json_object_object_add(jo_tenant, key_tenant, json_object_new_string(tenant));
	json_object_object_add(jo_tenant, key_pw_cred, jo_cred);

	json_object_object_add(jo_auth, key_auth, jo_tenant);
	return jo_auth;
}

/**
 * keystone_get_token
 *
 * Extracts a token from the given keystone structure.
 * Users must pass keystone structure derived by keystone_authenticate().
 */
char *keystone_get_token(struct keystone *keystone)
{
	json_object *jobj = keystone->access;
	if (jobj == NULL) {
		return NULL;
	}
	jobj = json_object_object_get(jobj, "access");
	jobj = json_object_object_get(jobj, "token");
	jobj = json_object_object_get(jobj, "id");

	return (char *)json_object_get_string(jobj);
}

static size_t keystone_authenticate_handler(void *buffer, size_t size, size_t nmemb, void *userp)
{
	json_object *jobj;

	//printf("DEBUG: keystone_authenticate_handler called: %p %d %d %p\n", buffer, size, nmemb, userp);

	if (userp != NULL) {
		jobj = json_tokener_parse(buffer);
		//On failure, jobj is NULL.
		((struct keystone *)userp)->access = jobj;
	}
	//write(1, buffer, nmemb);
	return nmemb;
}

/**
 * keystone_authenticate
 *
 * Do authentication using the given credentials.
 */
struct keystone *keystone_authenticate(char *user, char *tenant, char *password, char *auth_url)
{
	CURL *handle;
	CURLcode ret;
	struct curl_slist *header = NULL; 
	const char *postdata;
	char  *url_buf = alloca(strlen(auth_url) + strlen("/tokens") + 1);
	json_object *jo_creds;

	struct keystone *keystone = malloc(sizeof(struct keystone));
	if (keystone == NULL) return NULL;
	memset(keystone, 0x0, sizeof(struct keystone));

	jo_creds = keystone_build_credential(user, password, tenant);
	if (jo_creds == NULL) {
		free(keystone);
		return NULL;
	}
	postdata = json_object_to_json_string(jo_creds);
	//printf("DEBUG: %s\n", postdata);

	curl_global_init(CURL_GLOBAL_ALL);
	handle = curl_easy_init();
	// append "/token" to auth_url.
	strcpy(url_buf, auth_url);
	if (auth_url[strlen(auth_url) - 1] != '/') {
		strcat(url_buf, "/");
	}
	strcat(url_buf, "tokens");
	curl_easy_setopt(handle, CURLOPT_URL, url_buf);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, keystone_authenticate_handler);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, keystone);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postdata);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, (long)strlen(postdata));
	header = curl_slist_append(header, "Content-Type: application/json");
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header);
	/*
	 * Issue an HTTP POST request.
	 * Results will be handled in keystone_authenticate_handler().
	 */
	long respcode;
	ret = curl_easy_perform(handle);
	if (ret != CURLE_OK) {
		respcode = -1;
	} else {
		curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &respcode);
	}
	//printf("DEBUG: authenticate: %s %ld\n", curl_easy_strerror(ret), respcode);
	curl_slist_free_all(header);
	curl_easy_cleanup(handle);
	json_object_put(jo_creds);
	if ((respcode != 200) && (respcode != 203)) {
		if (keystone->access != NULL) {
			json_object_put(keystone->access);
		}
		free(keystone);
		return NULL;
	}
	return keystone;
}


char *keystone_get_endpoint(struct keystone *keystone,
			    char *service, char *url_type)
{
	json_object *jobj = keystone->access;
	json_object *j1, *j2, *j3, *j4;
	const char *type_str;
	if (jobj == NULL) return NULL;

	j1 = json_object_object_get(jobj, "access");
	j1 = json_object_object_get(j1, "serviceCatalog");
	//	printf("DEBUG: foo: type = %d\n" , json_object_get_type(j1));
	for (int i = 0; i < json_object_array_length(j1); i++) {
		j2 = json_object_array_get_idx(j1, i);
		j3 = json_object_object_get(j2, "type");
		type_str = json_object_get_string(j3);
		// printf("DEBUG: %s\n", type_str);
		if (strcmp(type_str, service) == 0) {
			j3 = json_object_object_get(j2, "endpoints");
			// printf("DEBUG: type = %d\n" , json_object_get_type(j3));

			for (int j = 0; j < json_object_array_length(j3);  j++) {
				// printf("DEBUG: j = %d / %d\n", j, json_object_array_length(j3));
				j4 = json_object_array_get_idx(j3, j);
				j4 = json_object_object_get(j4, url_type);
				//printf("DEBUG: publicURL: %s\n", json_object_get_string(j4));
				return (char *)json_object_get_string(j4);
			}
			break;
		}
	}
	return NULL;
}

