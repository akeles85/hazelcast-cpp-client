//
// Created by sancar koyunlu on 5/21/13.
// Copyright (c) 2013 sancar koyunlu. All rights reserved.


#include "hazelcast/client/ClientConfig.h"
#include "hazelcast/client/connection/Connection.h"
#include "hazelcast/client/protocol/AuthenticationRequest.h"
#include "hazelcast/client/spi/ClusterService.h"

namespace hazelcast {
    namespace client {
        namespace connection {
            ConnectionManager::ConnectionManager(spi::ClusterService& clusterService, serialization::SerializationService& serializationService, ClientConfig& clientConfig)
            : clusterService(clusterService)
            , serializationService(serializationService)
            , clientConfig(clientConfig)
            , heartBeatChecker(clientConfig.getConnectionTimeout(), serializationService)
            , principal(NULL) {

            };


            ConnectionManager::~ConnectionManager() {
                if (principal != NULL) {
                    delete principal;
                }
            };

            Connection *ConnectionManager::newConnection(Address const & address) {
                Connection *connection = new Connection(address, serializationService);
                authenticate(*connection, true, true);
                return connection;
            };

            Connection *ConnectionManager::getRandomConnection() {
                const Address& address = clientConfig.getLoadBalancer()->next().getAddress();
                return getConnection(address);
            }

            Connection *ConnectionManager::getConnection(const Address& address) {
                util::AtomicPointer<ConnectionPool> pool = getConnectionPool(address);
                if (pool == NULL )
                    return NULL;
                Connection *connection = NULL;
                connection = pool->take();
                if (connection != NULL && !heartBeatChecker.checkHeartBeat(*connection)) {
                    std::cerr << "Closing connection : " << *connection << " reason => Member is not responding to heartbeat";
                    delete connection;
                    return NULL;
                }
                return connection;
            };

            void ConnectionManager::releaseConnection(Connection *connection) {
                util::AtomicPointer<ConnectionPool> pool = getConnectionPool(connection->getEndpoint());
                if (pool != NULL) {
                    pool->release(connection);
                } else {
                    std::cerr << "Closing connection : " << *connection << " reason => Member pool is removed";
                    delete connection;
                }
            };

            util::AtomicPointer <ConnectionPool> ConnectionManager::getConnectionPool(const Address& address) {
                util::AtomicPointer<ConnectionPool> pool = poolMap.get(address);
                if (pool == NULL) {
                    if (!clusterService.isMemberExists(address)) {
                        util::AtomicPointer<ConnectionPool> x;
                        return x;
                    }
                    pool.reset(new ConnectionPool(address, serializationService, *this));

                    util::AtomicPointer<ConnectionPool> previousPool = poolMap.putIfAbsent(address, pool);
                    return previousPool == NULL ? pool : previousPool;
                }
                return pool;
            };

            void ConnectionManager::removeConnectionPool(const Address &address) {
                poolMap.remove(address);
            };

            void ConnectionManager::authenticate(Connection& connection, bool reAuth, bool firstConnection) {
                connection.connect();
                connection.write(protocol::ProtocolConstants::PROTOCOL);
                protocol::AuthenticationRequest auth(clientConfig.getCredentials());
                auth.setPrincipal(principal);
                auth.setReAuth(reAuth);
                auth.setFirstConnection(firstConnection);

                serialization::Data toData = serializationService.toData<protocol::AuthenticationRequest>(&auth);
                connection.write(toData);
                serialization::Data data1 = connection.read(serializationService.getSerializationContext());
                Address address = serializationService.toObject<Address>(data1);
                connection.setEndpoint(address);
                serialization::Data data2 = connection.read(serializationService.getSerializationContext());

                this->principal = new protocol::Principal(serializationService.toObject<protocol::Principal>(data2));
            };


        }
    }
}