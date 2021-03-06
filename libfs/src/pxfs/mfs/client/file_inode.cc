#define  __CACHE_GUARD__

#include "pxfs/mfs/client/dir_inode.h"
#include "pxfs/mfs/client/inode_factory.h"
#include "pxfs/mfs/client/file_inode.h"

#include <stdint.h>
#include "common/util.h"
#include "pxfs/client/session.h"
#include "pxfs/client/const.h"
#include "pxfs/common/publisher.h"

namespace mfs {
namespace client {

// FIXME: Should Link/Unlink increment/decrement the link count as well? currently the caller 
// must do a separate call, which breaks encapsulation. 


int 
FileInode::Read(::client::Session* session, char* dst, uint64_t off, uint64_t n)
{
	return rw_ref()->proxy()->interface()->Read(session, dst, off, n);
}


int 
FileInode::Write(::client::Session* session, char* src, uint64_t off, uint64_t n)
{
	int ret;
        s_log("[%ld] FileInode::%s",s_tid, __func__);

	ret = rw_ref()->proxy()->interface()->Write(session, src, off, n);
	return ret;
}


int FileInode::nlink()
{
	DBG_LOG(DBG_INFO, DBG_MODULE(client_inode),
	        "In inode %lx, nlink = %d\n", ino(), rw_ref()->proxy()->interface()->nlink());
	
	return rw_ref()->proxy()->interface()->nlink();
}


int FileInode::set_nlink(int nlink)
{
	DBG_LOG(DBG_INFO, DBG_MODULE(client_inode),
	        "In inode %lx, set nlink = %d\n", ino(), nlink);
	
	return rw_ref()->proxy()->interface()->set_nlink(nlink);
}


int
FileInode::Lock(::client::Session* session, lock_protocol::Mode mode)
{
	#ifdef FAST_SPIDER_LOCK_PATH
		pthread_mutex_lock(&spider_lock);
	#else
		osd::containers::client::ByteContainer::Proxy* cc_proxy;
			s_log("[%ld] FileInode::%s (1)", s_tid, __func__);

		if (ref_) {
			cc_proxy = rw_ref()->proxy();	
			return cc_proxy->Lock(session, mode);
		}
	#endif
	return E_SUCCESS;
}




int
FileInode::Lock(::client::Session* session, Inode* parent_inode, lock_protocol::Mode mode)
{
	#ifdef FAST_SPIDER_LOCK_PATH
		pthread_mutex_lock(&spider_lock);
	#else
	osd::containers::client::ByteContainer::Proxy* cc_proxy;
	osd::containers::client::NameContainer::Proxy* cc_proxy_parent;
		s_log("[%ld] FileInode::%s (2)", s_tid, __func__);

	if (ref_) {
		cc_proxy = rw_ref()->proxy();	
		if (parent_inode->ref_) {
			// the parent_inode must always be of DirInode type, 
			// otherwise we can't do hierarchical locking right?
			DirInode* parent_dir_inode = dynamic_cast<DirInode*>(parent_inode);
			cc_proxy_parent = parent_dir_inode->rw_ref()->proxy();	
			return cc_proxy->Lock(session, cc_proxy_parent, mode, osd::cc::client::HLock::FLG_STICKY);
		s_log("[%ld] FileInode::%s (2) <a>", s_tid, __func__);
		} else {
			return cc_proxy->Lock(session, mode);
		s_log("[%ld] FileInode::%s (2) <b>", s_tid, __func__);
		}
	}
	#endif
	return E_SUCCESS;
}


int
FileInode::Unlock(::client::Session* session)
{
	#ifdef FAST_SPIDER_LOCK_PATH
		pthread_mutex_unlock(&spider_lock);
	#else
	osd::containers::client::ByteContainer::Proxy* cc_proxy;
		s_log("[%ld] FileInode::%s ", s_tid, __func__);

	if (ref_) {
		cc_proxy = rw_ref()->proxy();	
		return cc_proxy->Unlock(session);
	}
	#endif
	return E_SUCCESS;
}
 

int 
FileInode::Sync(::client::Session* session)
{
	return rw_ref()->proxy()->interface()->vUpdate(session);
}


int 
FileInode::ioctl(::client::Session* session, int request, void* info)
{
	int ret = E_SUCCESS;
	switch (request)
	{
		case kIsEmpty: {
			bool isempty = (rw_ref()->proxy()->interface()->Size(session) == 0) ? false: true;
			*((bool *) info) = isempty;
		} break;
		case kSize: {
			uint64_t size = rw_ref()->proxy()->interface()->Size(session);
			*((uint64_t *) info) = size;
		} break;
	}
	return ret;
}


} // namespace client
} // namespace mfs
