//
// Created by sancar koyunlu on 8/5/13.
// Copyright (c) 2013 hazelcast. All rights reserved.




#ifndef HAZELCAST_TransactionContext
#define HAZELCAST_TransactionContext

#include "hazelcast/client/TransactionOptions.h"
#include "hazelcast/client/txn/TransactionProxy.h"
#include "hazelcast/client/TransactionalMap.h"
#include "hazelcast/client/exception/IllegalStateException.h"
#include "hazelcast/client/TransactionalQueue.h"
#include "hazelcast/client/TransactionalMultiMap.h"
#include "hazelcast/client/TransactionalList.h"
#include "hazelcast/client/TransactionalSet.h"

#if  defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4251) //for dll export
#endif

namespace hazelcast {
    namespace client {

        namespace spi {
            class ClientContext;
        }

        namespace connection {
            class ConnectionManager;
        }

        /**
         * Provides a context to do transactional operations; so beginning/committing transactions, but also retrieving
         * transactional data-structures like the TransactionalMap.
         *
         * @see HazelcastClient::newTransactionContext
         */
        class HAZELCAST_API TransactionContext {
        public:
            /**
             *  Constructor to be used internally. Not public API.
             *
             */
            TransactionContext(spi::ClientContext &clientContext, const TransactionOptions &);
            /**
             *  @return txn id.
             */
            std::string getTxnId() const;

            /**
             * Begins a transaction.
             *
             * @throws IllegalStateException if a transaction already is active.
             */
            void beginTransaction();

            /**
             * Commits a transaction.
             *
             * @throws TransactionException if no transaction is active or the transaction could not be committed.
             */
            void commitTransaction();

            /**
             * Begins a transaction.
             *
             * @throws IllegalStateException if a transaction already is active.
             */
            void rollbackTransaction();

            /**
             * Returns the transactional distributed map instance with the specified name.
             *
             *
             * @param name name of the distributed map
             * @return transactional distributed map instance with the specified name
            */

            template<typename K, typename V>
            TransactionalMap<K, V> getMap(const std::string &name) {
                return getTransactionalObject< TransactionalMap<K, V> >(name);
            }

            /**
             * Returns the transactional queue instance with the specified name.
             *
             *
             * @param name name of the queue
             * @return transactional queue instance with the specified name
             */
            template<typename E>
            TransactionalQueue< E > getQueue(const std::string &name) {
                return getTransactionalObject< TransactionalQueue< E > >(name);
            }

            /**
             * Returns the transactional multimap instance with the specified name.
             *
             *
             * @param name name of the multimap
             * @return transactional multimap instance with the specified name
             */
            template<typename K, typename V>
            TransactionalMultiMap<K, V> getMultiMap(const std::string &name) {
                return getTransactionalObject< TransactionalMultiMap<K, V> >(name);
            }

            /**
             * Returns the transactional list instance with the specified name.
             *
             *
             * @param name name of the list
             * @return transactional list instance with the specified name
             */
            template<typename E>
            TransactionalList< E > getList(const std::string &name) {
                return getTransactionalObject< TransactionalList< E > >(name);
            }

            /**
             * Returns the transactional set instance with the specified name.
             *
             *
             * @param name name of the set
             * @return transactional set instance with the specified name
             */
            template<typename E>
            TransactionalSet< E > getSet(const std::string &name) {
                return getTransactionalObject< TransactionalSet< E > >(name);
            }

            /**
             * get any transactional object with template T.
             *
             * Mostly to be used by spi implementers of Hazelcast.
             *
             * @return transactionalObject.
             */
            template<typename T>
            T getTransactionalObject(const std::string &name) {
                if (transaction.getState() != txn::TxnState::ACTIVE) {
                    std::string message = "No transaction is found while accessing ";
                    message += "transactional object -> [" + name + "]!";
                    throw exception::IllegalStateException("TransactionContext::getMap(const std::string& name)", message);
                }
                T txnObject(name, &transaction);
                return txnObject;
            }

        private :
            const int CONNECTION_TRY_COUNT;
            spi::ClientContext &clientContext;
            TransactionOptions options;
            boost::shared_ptr<connection::Connection> txnConnection;
            txn::TransactionProxy transaction;

            boost::shared_ptr<connection::Connection> connect();

        };

    }
}

#if  defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#pragma warning(pop)
#endif

#endif //HAZELCAST_TransactionContext

