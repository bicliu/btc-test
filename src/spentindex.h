// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SPENTINDEX_H
#define BITCOIN_SPENTINDEX_H

#include "uint256.h"
#include "amount.h"
#include "script/script.h"
#include <script/standard.h>

struct CSpentIndexKey {
    uint256 txid;
    unsigned int outputIndex;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action/*, int nType = SER_DISK, int nVersion = CLIENT_VERSION*/) {
        READWRITE(txid);
        READWRITE(outputIndex);
    }

    CSpentIndexKey(uint256 t, unsigned int i) {
        txid = t;
        outputIndex = i;
    }

    CSpentIndexKey() {
        SetNull();
    }

    void SetNull() {
        txid.SetNull();
        outputIndex = 0;
    }

};

struct CSpentIndexValue {
    uint256 txid;
    unsigned int inputIndex;
    int blockHeight;
    CAmount satoshis;
    int addressType;
    uint160 addressHash;
	uint256 vitnessHash;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action/*, int nType, int nVersion*/) {
        READWRITE(txid);
        READWRITE(inputIndex);
        READWRITE(blockHeight);
        READWRITE(satoshis);
        READWRITE(addressType);
		if(addressType == 4)
			READWRITE(vitnessHash);
		else
        	READWRITE(addressHash);
    }

    CSpentIndexValue(uint256 t, unsigned int i, int h, CAmount s, int type, uint160 a, uint256 va) {
        txid = t;
        inputIndex = i;
        blockHeight = h;
        satoshis = s;
        addressType = type;
        addressHash = a;
		vitnessHash = va;
    }

    CSpentIndexValue() {
        SetNull();
    }

    void SetNull() {
        txid.SetNull();
        inputIndex = 0;
        blockHeight = 0;
        satoshis = 0;
        addressType = 0;
        addressHash.SetNull();
		vitnessHash.SetNull();
    }

    bool IsNull() const {
        return txid.IsNull();
    }
};

struct CSpentIndexKeyCompare
{
    bool operator()(const CSpentIndexKey& a, const CSpentIndexKey& b) const {
        if (a.txid == b.txid) {
            return a.outputIndex < b.outputIndex;
        } else {
            return a.txid < b.txid;
        }
    }
};

struct CTimestampIndexIteratorKey {
    unsigned int timestamp;

    size_t GetSerializeSize(/*int nType, int nVersion*/) const {
        return 4;
    }
    template<typename Stream>
    void Serialize(Stream& s/*, int nType, int nVersion*/) const {
        ser_writedata32be(s, timestamp);
    }
    template<typename Stream>
    void Unserialize(Stream& s/*, int nType, int nVersion*/) {
        timestamp = ser_readdata32be(s);
    }

    CTimestampIndexIteratorKey(unsigned int time) {
        timestamp = time;
    }

    CTimestampIndexIteratorKey() {
        SetNull();
    }

    void SetNull() {
        timestamp = 0;
    }
};

struct CTimestampIndexKey {
    unsigned int timestamp;
    uint256 blockHash;

    size_t GetSerializeSize(/*int nType, int nVersion*/) const {
        return 36;
    }
    template<typename Stream>
    void Serialize(Stream& s/*, int nType, int nVersion*/) const {
        ser_writedata32be(s, timestamp);
        blockHash.Serialize(s/*, nType, nVersion*/);
    }
    template<typename Stream>
    void Unserialize(Stream& s/*, int nType, int nVersion*/) {
        timestamp = ser_readdata32be(s);
        blockHash.Unserialize(s/*, nType, nVersion*/);
    }

    CTimestampIndexKey(unsigned int time, uint256 hash) {
        timestamp = time;
        blockHash = hash;
    }

    CTimestampIndexKey() {
        SetNull();
    }

    void SetNull() {
        timestamp = 0;
        blockHash.SetNull();
    }
};

struct CAddressUnspentKey {
    unsigned int type;
    uint160 hashBytes;
	uint256 vithash;
    uint256 txhash;
    size_t index;

    size_t GetSerializeSize(/*int nType, int nVersion*/) const {
		if(type == 4)
		{
			return 69;
		}
        return 57;
    }
    template<typename Stream>
    void Serialize(Stream& s/*, int nType, int nVersion*/) const {
        ser_writedata8(s, type);
		if(type == 4)
			vithash.Serialize(s/*, nType, nVersion*/);
		else
        	hashBytes.Serialize(s/*, nType, nVersion*/);
        txhash.Serialize(s/*, nType, nVersion*/);
        ser_writedata32(s, index);
    }
    template<typename Stream>
    void Unserialize(Stream& s/*, int nType, int nVersion*/) {
        type = ser_readdata8(s);
		if(type == 4)
			vithash.Unserialize(s/*, nType, nVersion*/);
		else
        	hashBytes.Unserialize(s/*, nType, nVersion*/);
        txhash.Unserialize(s/*, nType, nVersion*/);
        index = ser_readdata32(s);
    }

    CAddressUnspentKey(unsigned int addressType, uint160 addressHash, uint256 vitness, uint256 txid, size_t indexValue) {
        type = addressType;
        hashBytes = addressHash;
		vithash = vitness;
        txhash = txid;
        index = indexValue;
    }

    CAddressUnspentKey() {
        SetNull();
    }

    void SetNull() {
        type = 0;
        hashBytes.SetNull();
		vithash.SetNull();
        txhash.SetNull();
        index = 0;
    }
};

struct CAddressUnspentValue {
    CAmount satoshis;
    CScript script;
    int blockHeight;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action/*, int nType, int nVersion*/) {
        READWRITE(satoshis);
        READWRITE(*(CScriptBase*)(&script));
        READWRITE(blockHeight);
    }

    CAddressUnspentValue(CAmount sats, CScript scriptPubKey, int height) {
        satoshis = sats;
        script = scriptPubKey;
        blockHeight = height;
    }

    CAddressUnspentValue() {
        SetNull();
    }

    void SetNull() {
        satoshis = -1;
        script.clear();
        blockHeight = 0;
    }

    bool IsNull() const {
        return (satoshis == -1);
    }
};

struct CAddressIndexKey {
    unsigned int type;
    uint160 hashBytes;
	uint256 vithash;
    int blockHeight;
    unsigned int txindex;
    uint256 txhash;
    size_t index;
    bool spending;

    size_t GetSerializeSize(/*int nType, int nVersion*/) const {
		if(type == 4)
			return 78;
        return 66;
    }
    template<typename Stream>
    void Serialize(Stream& s/*, int nType, int nVersion*/) const {
        ser_writedata8(s, type);
		if(type == 4)
			vithash.Serialize(s/*, nType, nVersion*/);
		else
        	hashBytes.Serialize(s/*, nType, nVersion*/);
        // Heights are stored big-endian for key sorting in LevelDB
        ser_writedata32be(s, blockHeight);
        ser_writedata32be(s, txindex);
        txhash.Serialize(s/*, nType, nVersion*/);
        ser_writedata32(s, index);
        char f = spending;
        ser_writedata8(s, f);
    }
    template<typename Stream>
    void Unserialize(Stream& s/*, int nType, int nVersion*/) {
        type = ser_readdata8(s);
		if(type == 4)
			vithash.Unserialize(s/*, nType, nVersion*/);
		else
        	hashBytes.Unserialize(s/*, nType, nVersion*/);
        blockHeight = ser_readdata32be(s);
        txindex = ser_readdata32be(s);
        txhash.Unserialize(s/*, nType, nVersion*/);
        index = ser_readdata32(s);
        char f = ser_readdata8(s);
        spending = f;
    }

    CAddressIndexKey(unsigned int addressType, uint160 addressHash, uint256 vitnessHash, int height, int blockindex,
                     uint256 txid, size_t indexValue, bool isSpending) {
        type = addressType;
        hashBytes = addressHash;
		vithash = vitnessHash;
        blockHeight = height;
        txindex = blockindex;
        txhash = txid;
        index = indexValue;
        spending = isSpending;
    }

    CAddressIndexKey() {
        SetNull();
    }

    void SetNull() {
        type = 0;
        hashBytes.SetNull();
		vithash.SetNull();
        blockHeight = 0;
        txindex = 0;
        txhash.SetNull();
        index = 0;
        spending = false;
    }

};

struct CAddressIndexIteratorKey {
    unsigned int type;
    uint160 hashBytes;
	uint256 vithash;

    size_t GetSerializeSize(/*int nType, int nVersion*/) const {
		if(type == 4)
			return 33;
        return 21;
    }
    template<typename Stream>
    void Serialize(Stream& s/*, int nType, int nVersion*/) const {
        ser_writedata8(s, type);
		if(type == 4)
			vithash.Serialize(s/*, nType, nVersion*/);
		else
        	hashBytes.Serialize(s/*, nType, nVersion*/);
    }
    template<typename Stream>
    void Unserialize(Stream& s/*, int nType, int nVersion*/) {
        type = ser_readdata8(s);
		if(type == 4)
			vithash.Unserialize(s/*, nType, nVersion*/);
		else
        	hashBytes.Unserialize(s/*, nType, nVersion*/);
    }

    CAddressIndexIteratorKey(unsigned int addressType, uint160 addressHash, uint256 vitnessHash) {
        type = addressType;
        hashBytes = addressHash;
		vithash = vitnessHash;
    }

    CAddressIndexIteratorKey() {
        SetNull();
    }

    void SetNull() {
        type = 0;
        hashBytes.SetNull();
		vithash.SetNull();
    }
};

struct CAddressIndexIteratorHeightKey {
    unsigned int type;
    uint160 hashBytes;
	uint256 vithash;
    int blockHeight;

    size_t GetSerializeSize(/*int nType, int nVersion*/) const {
		if(type == 4)
			return 37;
        return 25;
    }
    template<typename Stream>
    void Serialize(Stream& s/*, int nType, int nVersion*/) const {
        ser_writedata8(s, type);
		if(type == 4)
			vithash.Serialize(s/*, nType, nVersion*/);
		else
        	hashBytes.Serialize(s/*, nType, nVersion*/);
        ser_writedata32be(s, blockHeight);
    }
    template<typename Stream>
    void Unserialize(Stream& s/*, int nType, int nVersion*/) {
        type = ser_readdata8(s);
		if(type == 4)
			vithash.Unserialize(s/*, nType, nVersion*/);
		else
        	hashBytes.Unserialize(s/*, nType, nVersion*/);
        blockHeight = ser_readdata32be(s);
    }

    CAddressIndexIteratorHeightKey(unsigned int addressType, uint160 addressHash, uint256 vitnessHash, int height) {
        type = addressType;
        hashBytes = addressHash;
		vithash = vitnessHash;
        blockHeight = height;
    }

    CAddressIndexIteratorHeightKey() {
        SetNull();
    }

    void SetNull() {
        type = 0;
        hashBytes.SetNull();
		vithash.SetNull();
        blockHeight = 0;
    }
};


#endif // BITCOIN_SPENTINDEX_H
