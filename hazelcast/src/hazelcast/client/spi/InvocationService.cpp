//
// Created by sancar koyunlu on 5/23/13.
// Copyright (c) 2013 hazelcast. All rights reserved.


#include "hazelcast/client/spi/InvocationService.h"
#include "hazelcast/client/spi/ClusterService.h"
#include "hazelcast/client/spi/PartitionService.h"
#include "hazelcast/client/impl/PortableRequest.h"

namespace hazelcast {
    namespace client {
        namespace spi {
            InvocationService::InvocationService(ClusterService &clusterService, PartitionService &partitionService)
            : clusterService(clusterService)
            , partitionService(partitionService) {

            }

            boost::shared_future<serialization::Data> InvocationService::invokeOnRandomTarget(const impl::PortableRequest &request) {
                return clusterService.send(request);
            };

            boost::shared_future<serialization::Data> InvocationService::invokeOnKeyOwner(const impl::PortableRequest &request, serialization::Data &key) {
                Address *owner = partitionService.getPartitionOwner(partitionService.getPartitionId(key));
                if (owner != NULL) {
                    return invokeOnTarget(request, *owner);
                }
                return invokeOnRandomTarget(request);
            };

            boost::shared_future<serialization::Data> InvocationService::invokeOnTarget(const impl::PortableRequest &request, const Address &target) {
                return clusterService.send(request, target);
            };

            boost::shared_future<serialization::Data> InvocationService::invokeOnRandomTarget(const impl::PortableRequest &request, impl::EventHandlerWrapper *handler) {
                return clusterService.send(request, handler);
            }

            boost::shared_future<serialization::Data> InvocationService::invokeOnTarget(const impl::PortableRequest &request, impl::EventHandlerWrapper *handler, const Address &target) {
                return clusterService.send(request, handler, target);
            }

            boost::shared_future<serialization::Data> InvocationService::invokeOnKeyOwner(const impl::PortableRequest &request, impl::EventHandlerWrapper *handler, const serialization::Data &key) {
                Address *owner = partitionService.getPartitionOwner(partitionService.getPartitionId(key));
                if (owner != NULL) {
                    return invokeOnTarget(request, handler, *owner);
                }
                return invokeOnRandomTarget(request, handler);
            }
        }
    }
}