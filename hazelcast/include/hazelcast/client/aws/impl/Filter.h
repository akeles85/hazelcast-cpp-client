/*
 * Copyright (c) 2008-2023, Hazelcast, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <unordered_map>
#include <string>

#include "hazelcast/util/export.h"

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable : 4251) // for dll export
#endif

namespace hazelcast {
namespace client {
namespace aws {
namespace impl {
/**
 * Query filter to narrow down the scope of the queried EC2 instance set.
 */
class HAZELCAST_API Filter
{
public:
    Filter();

    /**
     *
     * Add a new filter with the given name and value to the query.
     *
     * @param name Filter name
     * @param value Filter value
     *
     */
    void add_filter(const std::string& name, const std::string& value);

    const std::unordered_map<std::string, std::string>& get_filters();

private:
    std::unordered_map<std::string, std::string> filters_;
};
}; // namespace impl
} // namespace aws
} // namespace client
} // namespace hazelcast

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#pragma warning(pop)
#endif
