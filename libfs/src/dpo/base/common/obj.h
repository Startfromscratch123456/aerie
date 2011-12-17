/**
 * \file obj.h
 *
 * \brief This is the object header of a public persistent object 
 */

#ifndef __STAMNOS_DPO_COMMON_PERSISTENT_OBJECT_H
#define __STAMNOS_DPO_COMMON_PERSISTENT_OBJECT_H

#include <stdint.h>
#include <boost/functional/hash.hpp>
#include "common/types.h"

namespace dpo {

namespace common {

// 0 is invalid value for type ObjectType
typedef uint16_t ObjectType;

struct ObjectIdHashFcn;

class ObjectId {
friend class ObjectIdHashFcn;
	enum {
		OBJECT_NUMBER_LEN_LOG2 = 48
	};
public:
	ObjectId(uint64_t u64 = 0)
		: u64_(u64)
	{ }

	ObjectId(const ObjectId& oid)
		: u64_(oid.u64_)
	{ }

	ObjectId(ObjectType type, void* addr) {
		Create(type, reinterpret_cast<uint64_t>(addr));
	}

	ObjectId(ObjectType type, uint64_t num) {
		Create(type, num);
	}

	ObjectType type() {
		return u16_[3];
	}

	uint64_t num() {
		return u64_ & ((1LLU << OBJECT_NUMBER_LEN_LOG2) - 1);
	}

	void* addr() {
		return reinterpret_cast<void*>(num());
	}

	bool operator==(const ObjectId& other) const {
		return (u64_ == other.u64_);
	}

	bool operator!=(const ObjectId& other) const {
		return !(*this == other);
	}
	
	uint64_t u64() {
		return u64_;
	}

private:
	void Create(ObjectType type, uint64_t num) {
		u64_ = type;
		u64_ = u64_ << OBJECT_NUMBER_LEN_LOG2;
		u64_ = u64_ | num;
	}
	union {
		uint64_t u64_;
		uint16_t u16_[4];
	};
};


struct ObjectIdHashFcn {
	std::size_t operator()(const ObjectId& oid) const {
		boost::hash<uint64_t> hasher;
		return hasher(oid.u64_);
	}
};


class Object {
public:
	ObjectId oid() {
		return ObjectId(type_, this);
	}

	ObjectType type() {
		return type_;
	}

	void set_type(ObjectType type) {
		type_ = type;
	}

protected:
	ObjectType type_; //!< Magic number identifying object type
};


} // namespace common


namespace cc {

/**
 * We support concurrency control along two (orthogonal) dimensions:
 * 1) hierarchy: flat or hierarchical
 *    - these two are mutually exclusive: an object cannot be controlled by
 *      both a flat and hierarchical lock. 
 * 2) synchronization type: locks or optimistic transactions
 *    - currently we support read-only optimistic transactions
 *    - these two modes are mutually exclusive
 */

namespace common {

typedef uint64_t VersionNumber;

/**
 * Base object for any object that is synchronized under distributed 
 * concurrency control
 */
class Object: public dpo::common::Object {
public:
	Object()
		: status_(0),
		  version_(0)
	{ }

	VersionNumber ccVersion() { return version_; }
	VersionNumber ccSetVersion(VersionNumber version) { version_ = version; }

protected:
	//! bit flags that indicate the state of the object
	//! - concurrency control mode
	uint32_t      status_ __attribute__ ((aligned (4)));
	VersionNumber version_ __attribute__ ((aligned (8)));
};


} // namespace common
} // namespace cc
} // namespace dpo


#endif // __STAMNOS_DPO_COMMON_OBJECT_H