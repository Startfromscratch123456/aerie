#ifndef __STAMNOS_CFS_SERVER_DIRECTORY_INODE_H
#define __STAMNOS_CFS_SERVER_DIRECTORY_INODE_H

#include "osd/containers/name/container.h"
#include "cfs/server/inode.h"

namespace server {

class Session;   // forward declaration
class FileInode; // forward declaration

class DirInode: public InodeT<DirInode> {
public:

	DirInode()
	{ }

	DirInode(InodeNumber ino)
		: InodeT<DirInode>(ino, kDirInode)
	{ 
		osd::common::ObjectId oid(ino);
		obj_ = osd::containers::server::NameContainer::Object::Load(oid);
	}

	static DirInode* Make(Session* session, DirInode* ip) {
		osd::common::ObjectId oid;
		int                   ret;

		if ((ret = session->salloc()->AllocateContainer(session, 0, osd::containers::T_NAME_CONTAINER, &oid)) < 0) {
			return NULL;
		}
		return new(ip) DirInode(oid.u64());
	}


	int Link(Session* session, const char* name, uint64_t ino);
	int Link(Session* session, const char* name, DirInode* child);
	int Link(Session* session, const char* name, FileInode* child);
	int Lookup(Session* session, const char* name, InodeNumber* ino);
	int Unlink(Session* session, const char* name);

	osd::containers::server::NameContainer::Object* obj() { return obj_; }
	
	osd::containers::server::NameContainer::Object* obj_;
};

} // namespace server

#endif // __STAMNOS_CFS_SERVER_DIRECTORY_INODE_H
