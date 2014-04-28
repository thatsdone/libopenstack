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
#ifndef _OPENSTACK_SWIFT_H
#define _OPENSTACK_SWIFT_H

#include <json/json.h>
#include <openstack/keystone.h>
/*
struct swift_account {
	json_object *account;
};

struct swift_container {
	json_object *container;
};

struct swift_object {
	json_object *object;
};

*/
void swift_stat(struct keystone *);

#endif

