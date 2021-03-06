#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tool/testfw/integrationtest.h"
#include "tool/testfw/testfw.h"
#include "tool/testfw/util.h"
#include "pxfs/client/client_i.h"
#include "pxfs/client/libfs.h"
#include "mfs.fixture.h"

using namespace client;

static const char* storage_pool_path = "/tmp/stamnos_pool";
static const char* test_str1 = "TEST_CREATE";

SUITE(MFSFile)
{
	TEST_FIXTURE(MFSFixture, TestCreate)
	{
		int  fd;
		char buf[512];

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK(libfs_mkdir("/home/hvolos/dir", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_CREAT|O_RDWR)) > 0);
		CHECK(libfs_write(fd, test_str1, strlen(test_str1)+1) > 0);
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}
	
	TEST_FIXTURE(MFSFixture, TestCreateUnlinkLoop)
	{
		int  fd;
		char buf[512];

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK(libfs_mkdir("/home/hvolos/dir", 0) == 0);
		for (int i=0; i<1024; i++) {
			CHECK((fd = libfs_open("/home/hvolos/dir/file", O_CREAT|O_RDWR)) > 0);
			CHECK(libfs_close(fd) == 0);
			CHECK(libfs_unlink("/home/hvolos/dir/file") == 0);
		}
		EVENT("E2");
		EVENT("E3");
	}


	TEST_FIXTURE(MFSFixture, TestUnlink)
	{
		int  fd;
		char buf[512];

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK(libfs_unlink("/home/hvolos/dir/file") == 0);
		EVENT("E2");
		EVENT("E3");
	}


	TEST_FIXTURE(MFSFixture, TestCreateUnlink)
	{
		int  fd;
		char buf[512];

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK(libfs_mkdir("/home/hvolos/dir", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_CREAT|O_RDWR)) > 0);
		CHECK(libfs_write(fd, test_str1, strlen(test_str1)+1) > 0);
		CHECK(libfs_close(fd) == 0);
		CHECK(libfs_unlink("/home/hvolos/dir/file") == 0);
		EVENT("E2");
		EVENT("E3");
	}

	TEST_FIXTURE(MFSFixture, TestUnlinkCreate)
	{
		int  fd;
		char buf[512];

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK(libfs_unlink("/home/hvolos/dir/file") == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_CREAT|O_RDWR)) > 0);
		CHECK(libfs_write(fd, test_str1, strlen(test_str1)+1) > 0);
		CHECK(libfs_close(fd) == 0);
		CHECK(libfs_unlink("/home/hvolos/dir/file") == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_CREAT|O_RDWR)) > 0);
		CHECK(libfs_write(fd, test_str1, strlen(test_str1)+1) > 0);
		CHECK(libfs_close(fd) == 0);
		CHECK(libfs_unlink("/home/hvolos/dir/file") == 0);
		EVENT("E2");
		EVENT("E3");
	}


	TEST_FIXTURE(MFSFixture, TestOpen)
	{
		int  fd;
		char buf[512];

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_RDWR)) > 0);
		CHECK(libfs_read(fd, buf, strlen(test_str1)+1) > 0);
		CHECK(strcmp(buf, test_str1) == 0);
		CHECK(libfs_close(fd) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_RDWR)) > 0);
		CHECK(libfs_read(fd, buf, strlen(test_str1)+1) > 0);
		CHECK(strcmp(buf, test_str1) == 0);
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}


	TEST_FIXTURE(MFSFixture, TestOpenCloseLoop1)
	{
		int  fd;
		char buf[512];

		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK(libfs_mkdir("/home/hvolos/dir", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_CREAT|O_RDWR)) > 0);
		CHECK(libfs_close(fd) == 0);
		CHECK(libfs_sync() == 0);
		EVENT("E1");
		for (int i=0; i<1024; i++) {
			CHECK((fd = libfs_open("/home/hvolos/dir/file", O_RDWR)) > 0);
			CHECK(libfs_close(fd) == 0);
		}
		EVENT("E2");
		EVENT("E3");
	}

	TEST_FIXTURE(MFSFixture, TestOpenCloseLoop2)
	{
		int  fd;
		char buf[512];

		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		EVENT("E1");
		for (int i=0; i<1024; i++) {
			CHECK((fd = libfs_open("/home/hvolos/dir/file", O_RDWR)) > 0);
			CHECK(libfs_close(fd) == 0);
		}
		EVENT("E2");
		EVENT("E3");
	}


	TEST_FIXTURE(MFSFixture, TestCreate2)
	{
		int   fd;
		char* buf = (char*) malloc(1024*1024);
		fillbuf(buf, 1024*1024, 1);

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK(libfs_mkdir("/home/hvolos/dir", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_CREAT|O_RDWR)) > 0);
		CHECK(libfs_write(fd, buf, 8*4096+1024) > 0);
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}
	
	TEST_FIXTURE(MFSFixture, TestRead2)
	{
		int   ret;
		int   fd;
		char* buf = (char*) malloc(1024*1024);;
		char* correct = (char*) malloc(1024*1024);;
		fillbuf(correct, 1024*1024, 1);

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_RDWR)) > 0);
		// we actually wrote less but the server keeps track the size at page granularity
		CHECK((ret=libfs_read(fd, buf, 1024*1024)) == 8*4096+4096); 
		CHECK(memcmp(buf, correct, 8*4096+1024) == 0);
		CHECK((ret=libfs_read(fd, buf, 1024*1024)) == 0);
		CHECK((ret=libfs_read(fd, buf, 1024*1024)) == 0);
		CHECK((ret=libfs_read(fd, buf, 1024*1024)) == 0);
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}


	TEST_FIXTURE(MFSFixture, TestCreate3)
	{
		int   ret;
		int   fd;
		char* correct = (char*) malloc(1024*1024);
		char* buf = (char*) malloc(1024*1024);
		
		fillbuf(correct, 1024*1024, 1);
		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK(libfs_mkdir("/home/hvolos/dir", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_CREAT|O_RDWR)) > 0);
		for (int i=0; i<16; i++) {
			CHECK(libfs_write(fd, correct, 1024*1024) > 0);
		}
		for (int i=0; i<16; i++) {
			for (int j=0; j < 1024*1024; j+=4096) {
				CHECK((ret=libfs_pread(fd, buf, 4096, j)) <= 4096); 
				CHECK (memcmp(buf, &correct[j], ret) == 0);
			}
		}
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}

	TEST_FIXTURE(MFSFixture, TestRead3)
	{
		int   ret;
		int   fd;
		char* buf = (char*) malloc(1024*1024);;
		char* correct = (char*) malloc(2*1024*1024);;
		
		fillbuf(correct, 1024*1024, 1);
		memcpy(&correct[1024*1024], correct, 1024*1024);
		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_RDWR)) > 0);
		srand(1);
#if 1		
		int chunk_size = 8192*4;
		for (int c=0; c<1024; c++) {
			int index = rand() % (16*1024*1024);
			CHECK((ret=libfs_pread(fd, buf, chunk_size, index)) <= chunk_size); 
			CHECK(memcmp(buf, &correct[index % (1024*1024)], ret) == 0);
			if (index<16*1024*1024 - chunk_size) {
				CHECK(ret == chunk_size);
			} else {
				CHECK(ret == 16*1024*1024-index);
			}
			if (memcmp(buf, &correct[index % (1024*1024)], ret) != 0) {
				printf("c=%d, ret=%d\n", c, ret);
				for (int i=0; i<ret; i++) {
					if ( buf[i] != correct[index % (1024*1024) + i]) {
						printf("[%d][%i] %x==%x %c\n", index, i, buf[i], correct[index % (1024*1024) + i], buf[i] == correct[index % (1024*1024) + i] ? 'Y': 'N');
						break;
					}
				}
			}
		}
		for (int i=0; i<16; i++) {
			for (int j=0; j < 1024*1024; j+=chunk_size) {
				CHECK((ret=libfs_pread(fd, buf, chunk_size, i*1024*1024+j)) <= chunk_size); 
				CHECK(memcmp(buf, &correct[j], ret) == 0);
				/*
				if (ret > 4096) {
					printf("%d: READ_FAILED: %d\n", i*1024*1024+j, ret);
				}
				if (memcmp(buf, &correct[j], ret) != 0) {
					printf("%d: READ_FAILED_CMP\n", i*1024*1024+j);
				}*/
			}
		}
#endif
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}


#define WRITE(off, n)                                     \
  CHECK(libfs_pwrite(fd, &buf[off], n, off) > 0);

#define COMPARE(off, n)                                   \
    CHECK(libfs_pread(fd, buf, n, off) > 0);              \
    CHECK(memcmp(buf, &correct[off], n) == 0);

#define ISZERO(off, n)                                    \
    CHECK(libfs_pread(fd, buf, n, off) > 0);              \
    do {                                                  \
      bool iszero = true;                                 \
      for (int i=0; i<n; i++) {                           \
        if (buf[i] != 0) {                                \
          iszero=false;                                   \
          break;                                          \
        }                                                 \
      }                                                   \
      CHECK(iszero == true);                              \
    } while (0);

	TEST_FIXTURE(MFSFixture, TestCreate4)
	{
		int   fd;
		char* buf = (char*) malloc(1024*1024);
		char* correct = (char*) malloc(1024*1024);;

		fillbuf(buf, 1024*1024, 1);
		fillbuf(correct, 1024*1024, 1);

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/file", O_CREAT|O_RDWR)) > 0);
		WRITE(0, 512);
		WRITE(4096, 512);
		WRITE(22*4096 + 512, 512);
		WRITE(24*4096, 512);
		WRITE(4096, 12*4096);
		WRITE(14*4096, 4096);
		COMPARE(0, 512);
		COMPARE(4096, 512);
		COMPARE(22*4096 + 512, 512);
		COMPARE(4096, 4096);
		COMPARE(4096, 4096+1);
		COMPARE(4096, 11*4096);
		COMPARE(4096, 12*4096);
		COMPARE(24*4096, 512);
    		CHECK(libfs_pread(fd, buf, 512, 14*4096) > 0);
		ISZERO(15*4096, 512);
		ISZERO(16*4096, 3*4096);
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}


	TEST_FIXTURE(MFSFixture, TestRead4)
	{
		int   ret;
		int   fd;
		char* buf = (char*) malloc(1024*1024);;
		char* correct = (char*) malloc(1024*1024);;
		fillbuf(correct, 1024*1024, 1);

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/file", O_RDWR)) > 0);
		// we actually wrote less but the server keeps track the size at page granularity
		CHECK((ret=libfs_pread(fd, buf, 512, 0)) <= 4096); 
		CHECK(memcmp(buf, correct, 512) == 0);
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}


	TEST_FIXTURE(MFSFixture, TestCreate5)
	{
		int   fd;
		char* buf = (char*) malloc(1024*1024);
		char* correct = (char*) malloc(1024*1024);;

		fillbuf(buf, 1024*1024, 1);
		fillbuf(correct, 1024*1024, 1);

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/file", O_CREAT|O_RDWR)) > 0);
		WRITE(0, 512);
		WRITE(4096, 512);
		WRITE(22*4096 + 512, 512);
		WRITE(24*4096, 512);
		COMPARE(0, 512);
		COMPARE(4096, 512);
		COMPARE(22*4096 + 512, 512);
		COMPARE(24*4096, 512);
		ISZERO(15*4096, 512);
		ISZERO(16*4096, 3*4096);
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}


	TEST_FIXTURE(MFSFixture, TestRead5)
	{
		int   ret;
		int   fd;
		char* buf = (char*) malloc(1024*1024);;
		char* correct = (char*) malloc(1024*1024);;
		fillbuf(correct, 1024*1024, 1);

		EVENT("E1");
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/file", O_RDWR)) > 0);
		// we actually wrote less but the server keeps track the size at page granularity
		CHECK((ret=libfs_pread(fd, buf, 512, 0)) <= 4096); 
		CHECK(memcmp(buf, correct, 512) == 0);
		CHECK(libfs_close(fd) == 0);
		EVENT("E2");
		EVENT("E3");
	}









	TEST_FIXTURE(MFSFixture, TestCreateWriteConcurrent)
	{
		int  fd;
		char buf[512];
		
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		CHECK(libfs_mkdir("/home/hvolos/dir", 0) == 0);
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_CREAT|O_RDWR)) > 0);
		CHECK(libfs_close(fd) == 0);
		CHECK(libfs_sync() == 0);
		EVENT("E1");
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_RDWR)) > 0);
		CHECK(libfs_pwrite(fd, test_str1, strlen(test_str1)+1, 0) > 0);
		EVENT("E2");
		EVENT("E3");
		CHECK(libfs_pwrite(fd, test_str1, strlen(test_str1)+1, 0) > 0);
		EVENT("E4");
		CHECK(libfs_close(fd) == 0);
		EVENT("E5");
	}

	TEST_FIXTURE(MFSFixture, TestOpenWriteConcurrent)
	{
		int  fd;
		char buf[512];
		
		CHECK(libfs_mount(storage_pool_path, "/home/hvolos", "mfs", 0) == 0);
		EVENT("E1");
		CHECK((fd = libfs_open("/home/hvolos/dir/file", O_RDWR)) > 0);
		CHECK(libfs_pwrite(fd, test_str1, strlen(test_str1)+1, 0) > 0);
		EVENT("E2");
		EVENT("E3");
		CHECK(libfs_pwrite(fd, test_str1, strlen(test_str1)+1, 0) > 0);
		EVENT("E4");
		CHECK(libfs_close(fd) == 0);
		EVENT("E5");
	}

	TEST_FIXTURE(MFSFixture, TestFileSystemCreate)
	{
		int               fd;
		char              buf[512];
		std::string       filename;

		CHECK(libfs_mount(storage_pool_path, "/pxfs", "mfs", 0) == 0);
		MapFileSystemImage(this, session, SELF, "/pxfs", 20, 10, 2);
		EVENT("E1");
		EVENT("EEND");
	}

	TEST_FIXTURE(MFSFixture, TestFileSystemStress1)
	{
		int               fd;
		char              buf[4096];
		std::string       filename;

		CHECK(libfs_mount(storage_pool_path, "/pxfs", "mfs", 0) == 0);
		MapFileSystemImage(this, session, SELF, "/pxfs", 20, 10, 2);
		EVENT("E1");
		for (int i=0; i<1024; i++) {
			fileset_exists.GetRandom(&filename); 
			CHECK((fd = libfs_open(filename.c_str(), O_RDWR|O_APPEND)) > 0);
			libfs_write(fd, buf, 4096);
			libfs_close(fd);
		}
		EVENT("EEND");
	}

	TEST_FIXTURE(MFSFixture, TestFileSystemStress2)
	{
		int               fd;
		char              buf[4096];
		std::string       filename;

		CHECK(libfs_mount(storage_pool_path, "/pxfs", "mfs", 0) == 0);
		MapFileSystemImage(this, session, SELF, "/pxfs", 20, 10, 2);
		EVENT("E1");
		for (int i=0; i<1024; i++) {
			fileset_exists.EraseRandom(&filename); 
			libfs_unlink(filename.c_str());
			fileset_notexists.Put(filename); 
			fileset_notexists.EraseRandom(&filename); 
			CHECK((fd = libfs_open(filename.c_str(), O_CREAT|O_RDWR)) > 0);
			libfs_close(fd);
			fileset_exists.Put(filename); 
		}
		EVENT("EEND");
	}


}
