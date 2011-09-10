#include "chunkstore.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include "rpc/rpc.h"
#include "server/api.h"
#include "common/util.h"
#include "common/vistaheap.h"
#include "chunkdsc.h"


static const uint64_t kChunkStoreSize = 1024*1024*64;
static const char*    kChunkStoreName = "chunkstore.vistaheap";


ChunkStore::ChunkStore(rpcc* c, id_t principal_id):
	_client(c), 
	_principal_id(principal_id)
{
}


void 
ChunkStore::Init()
{
	int        ret;
	PHeap*     pheap;

	_pheap = new PHeap();
	_pagepheap = new PHeap();

	while(_pagepheap->Map("chunkstore.page.pheap", kChunkStoreSize, PROT_READ|PROT_WRITE) != 0);
	_pheap->Map("chunkstore.pheap", 1024*1024, PROT_READ| PROT_WRITE);

	_pheap_root = (ChunkStoreRoot*) _pheap->get_root();
}


int
ChunkStore::CreateChunk(size_t size, int type, ChunkDescriptor** chunkdscp)
{
	int                intret; 
	unsigned long long chunkdsc_id;
	unsigned long long r;
	unsigned long long usize = (unsigned long long) size;

	intret = _client->call(RPC_CHUNK_CREATE, _principal_id, usize, type, r);

	if (r == 0) {
		return -1;
	}

	chunkdsc_id = r;
	*chunkdscp = (ChunkDescriptor*) chunkdsc_id;

	return 0;
}


int 
ChunkStore::DeleteChunk(ChunkDescriptor* chunkdsc)
{
	int                intret; 
	unsigned long long chunkdsc_id = (unsigned long long) chunkdsc;
	int                r;

	if (!chunkdsc) {
		return -1;
	}

	intret = _client->call(RPC_CHUNK_DELETE, _principal_id, chunkdsc_id, r);

	return 0;
}


int 
ChunkStore::AccessChunkList(ChunkDescriptor* chunkdsc[], size_t nchunkdsc, int prot_flags)
{
	int                             intret;
	int                             r;
	unsigned long long              chunkdsc_id;
	unsigned long long              uaddr;
	std::vector<unsigned long long> vuchunkdsc;
	int                             i;

	for (i=0; i<nchunkdsc; i++) {
		vuchunkdsc.push_back((unsigned long long) chunkdsc[i]);
	}

	intret = _client->call(RPC_CHUNK_ACCESS, _principal_id, vuchunkdsc, prot_flags, r);

	if (r != 0) {
		return r;
	}

	/* do mprotect */

	for (i=0; i<nchunkdsc; i++) {
		intret = mprotect(chunkdsc[i]->chunk_, chunkdsc[i]->_size, prot_flags);
		assert(intret == 0);
	}

	return 0;
}


int 
ChunkStore::AccessChunk(ChunkDescriptor* chunkdsc, int prot_flags)
{
	ChunkDescriptor* chunkdsc_array[1];

	chunkdsc_array[0] = chunkdsc;

	return AccessChunkList(chunkdsc_array, 1, prot_flags);
}


int 
ChunkStore::ReleaseChunkList(ChunkDescriptor* chunkdsc[], size_t nchunkdsc)
{
	int                             intret;
	int                             r;
	unsigned long long              chunkdsc_id;
	unsigned long long              uaddr;
	std::vector<unsigned long long> vuchunkdsc;
	int                             i;

	for (i=0; i<nchunkdsc; i++) {
		vuchunkdsc.push_back((unsigned long long) chunkdsc[i]);
	}

	intret = _client->call(RPC_CHUNK_RELEASE, _principal_id, vuchunkdsc, r);

	if (r != 0) {
		return r;
	}

	/* do mprotect */

	for (i=0; i<nchunkdsc; i++) {
		intret = mprotect(chunkdsc[i]->chunk_, chunkdsc[i]->_size, PROT_NONE);
		assert(intret == 0);
	}

	return 0;
}


int 
ChunkStore::ReleaseChunk(ChunkDescriptor* chunkdsc)
{
	ChunkDescriptor* chunkdsc_array[1];

	chunkdsc_array[0] = chunkdsc;

	return ReleaseChunkList(chunkdsc_array, 1);
}



int 
ChunkStore::AccessAddr(void* addr)
{
	int                prot_flags;
	int                intret;
	int                r;
	unsigned long long uaddr;
	std::vector<unsigned long long> vaddr;

	assert(0 && "ChunkStore::AccessAddr not implemented");


	//vaddr.push_back((unsigned long long) addr);

	//intret = _client->call(24, _principal_id, vaddr, r);

	//if (r != 0) {
	//	return r;
	//}


	// TODO
	/* do mprotect */

	return 0;
}