//
// Created by sancar koyunlu on 6/18/13.
// Copyright (c) 2013 hazelcast. All rights reserved.


#include "hazelcast/client/impl/MapKeySet.h"
#include "hazelcast/client/map/DataSerializableHook.h"
#include "ObjectDataOutput.h"
#include "ObjectDataInput.h"

namespace hazelcast {
    namespace client {
        namespace map {
            MapKeySet::MapKeySet() {

            };

            const std::vector<serialization::Data>& MapKeySet::getKeySet() const {
                return keySet;
            };

            int MapKeySet::getFactoryId() const {
                return DataSerializableHook::F_ID;
            }

            int MapKeySet::getClassId() const {
                return DataSerializableHook::KEY_SET;
            }

            void MapKeySet::writeData(serialization::ObjectDataOutput& writer) {
                writer.writeInt(keySet.size());
                for (int i = 0; i < keySet.size(); ++i) {
                    keySet[i].writeData(writer);
                }

            }

            void MapKeySet::readData(serialization::ObjectDataInput& reader) {
                int size = reader.readInt();
                keySet.resize(size);
                for (int i = 0; i < size; i++) {
                    keySet[i].readData(reader);
                }
            }


        }
    }
}