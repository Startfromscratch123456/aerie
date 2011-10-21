#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include "tool/testfw/unittest.h"
#include "mfs/hashtable.h"
#include "common/errno.h"

class Context;

class StorageManager {
public:
	int Alloc(Context* ctx, size_t nbytes, std::type_info const& typid, void** ptr)
	{
		*ptr = malloc(nbytes);
		return E_SUCCESS;
	}
};


class Context {
public:
	StorageManager* sm;
};

struct ContextFixture 
{
	ContextFixture() 
		: ctx(NULL)
	{ }
	
	Context* ctx;
};



SUITE(HashTableEntry)
{
	const int buffer_size = 128;

	TEST_FIXTURE(ContextFixture, TestMakeEntry1)
	{
		char            b[buffer_size];
		Entry<Context>* entry; 

		entry = Entry<Context>::MakeEntry(b);
		CHECK(entry->Init(true, buffer_size-TAG_SIZE) == 0);
		CHECK(entry->IsFree() == true);
		CHECK(entry->get_size() == (buffer_size));
		CHECK(entry->get_payload_size() == (buffer_size-TAG_SIZE));

		entry = Entry<Context>::MakeEntry(b);
		CHECK(entry->IsFree() == true);
		CHECK(entry->get_size() == (buffer_size));
		CHECK(entry->get_payload_size() == (buffer_size-TAG_SIZE));

		entry = Entry<Context>::MakeEntry(b);
		CHECK(entry->Init(false, buffer_size-TAG_SIZE) == 0);
		CHECK(entry->IsFree() == false);
		CHECK(entry->get_size() == (buffer_size));
		CHECK(entry->get_payload_size() == (buffer_size-TAG_SIZE));

		entry = Entry<Context>::MakeEntry(b);
		CHECK(entry->IsFree() == false);
		CHECK(entry->get_size() == (buffer_size));
		CHECK(entry->get_payload_size() == (buffer_size-TAG_SIZE));
	
	}

	TEST_FIXTURE(ContextFixture, TestMakeEntry2)
	{
		char            b[buffer_size];
		Entry<Context>* entry; 
		Entry<Context>* entry2; 

		entry = Entry<Context>::MakeEntry(b);
		CHECK(entry->Init(true, buffer_size-TAG_SIZE) == 0);
		CHECK(entry->IsFree() == true);
		CHECK(entry->get_size() == (buffer_size));
		CHECK(entry->get_payload_size() == (buffer_size-TAG_SIZE));

		entry2 = Entry<Context>::MakeEntry(entry, buffer_size-TAG_SIZE+1);
		CHECK(entry2==NULL);
		entry2 = Entry<Context>::MakeEntry(entry, 18);
		CHECK(entry2!=NULL);
		CHECK(entry2->IsFree() == true);
		CHECK(entry2->get_size() == (entry->get_size() - 18));
	}

}


SUITE(HashTablePage)
{
	TEST_FIXTURE(ContextFixture, TestInsert1)
	{
		char*         key1 = (char*) "key1";
		uint64_t      val;
		Page<Context> page;

		CHECK(page.Insert(ctx, key1, strlen(key1)+1, 0xCAFE) == 0);
		CHECK(page.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
	}

	TEST_FIXTURE(ContextFixture, TestInsert2)
	{
		char*         key1 = (char*) "key1";
		char*         key2 = (char*) "key2";
		char*         key3 = (char*) "key3";
		uint64_t      val;
		Page<Context> page;

		CHECK(page.Insert(ctx, key1, strlen(key1)+1, 0xCAFE) == 0);
		CHECK(page.Insert(ctx, key2, strlen(key2)+1, 0xC1A) == 0);
		CHECK(page.Insert(ctx, key3, strlen(key3)+1, 0xFB1) == 0);
		CHECK(page.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page.Search(ctx, key2, strlen(key2)+1, &val) == 0);
		CHECK(val == 0xC1A);
		CHECK(page.Search(ctx, key3, strlen(key3)+1, &val) == 0);
		CHECK(val == 0xFB1);
	}

	TEST_FIXTURE(ContextFixture, TestInsert3)
	{
		char*         key1 = (char*) "key1";
		char*         key2 = (char*) "key2";
		char          longkey3[PAGE_SIZE];
		int           longkey3_size = PAGE_SIZE - 18;
		uint64_t      val;
		Page<Context> page;

		CHECK(page.Insert(ctx, longkey3, longkey3_size, 0xFB1) == 0);
		CHECK(page.Search(ctx, longkey3, longkey3_size, &val) == 0);
		CHECK(val == 0xFB1);
	}


	TEST_FIXTURE(ContextFixture, TestInsert4)
	{
		char*         key1 = (char*) "key1";
		char*         key2 = (char*) "key2";
		char          longkey3[PAGE_SIZE];
		int           longkey3_size = PAGE_SIZE - 18;
		uint64_t      val;
		Page<Context> page;

		CHECK(page.Insert(ctx, key1, strlen(key1)+1, 0xCAFE) == 0);
		CHECK(page.Insert(ctx, key2, strlen(key2)+1, 0xC1A) == 0);
		CHECK(page.Insert(ctx, longkey3, longkey3_size, 0xFB1) < 0);
		CHECK(page.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page.Search(ctx, key2, strlen(key2)+1, &val) == 0);
		CHECK(val == 0xC1A);
		CHECK(page.Search(ctx, longkey3, longkey3_size, &val) < 0);
	}


	TEST_FIXTURE(ContextFixture, TestInsert5)
	{
		char*         key1 = (char*) "key1";
		char*         key2 = (char*) "key2";
		char          longkey3[PAGE_SIZE];
		int           longkey3_size = PAGE_SIZE - 18;
		uint64_t      val;
		Page<Context> page;

		CHECK(page.Insert(ctx, key1, strlen(key1)+1, 0xCAFE) == 0);
		CHECK(page.Insert(ctx, key2, strlen(key2)+1, 0xC1A) == 0);
		CHECK(page.Insert(ctx, longkey3, longkey3_size, 0xFB1) < 0);
		CHECK(page.Search(ctx, longkey3, longkey3_size, &val) < 0);
		CHECK(page.Delete(ctx, key1, strlen(key1)+1) == 0);
		CHECK(page.Delete(ctx, key2, strlen(key2)+1) == 0);
		CHECK(page.Insert(ctx, longkey3, longkey3_size, 0xFB1) == 0);
		CHECK(page.Search(ctx, longkey3, longkey3_size, &val) == 0);
	}


	TEST_FIXTURE(ContextFixture, TestInsert6)
	{
		char*         key1 = (char*) "key1";
		char*         key2 = (char*) "key2";
		char*         key3 = (char*) "key3";
		char*         key4 = (char*) "key3";
		int           key1_size = (PAGE_SIZE-5*sizeof(uint64_t))/3;
		int           key2_size = (PAGE_SIZE-5*sizeof(uint64_t))/3;
		int           key3_size = (PAGE_SIZE-5*sizeof(uint64_t))/3;
		int           key4_size = 2*key1_size-10;
		uint64_t      val;
		Page<Context> page;

		CHECK(page.Insert(ctx, key1, key1_size, 0xCAFE) == 0);
		CHECK(page.Insert(ctx, key2, key2_size, 0xC1A) == 0);
		CHECK(page.Insert(ctx, key3, key3_size, 0xFB1) == 0);
		CHECK(page.Insert(ctx, key4, key4_size, 0xBEEF) < 0);
		CHECK(page.Search(ctx, key1, key1_size, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page.Search(ctx, key2, key2_size, &val) == 0);
		CHECK(val == 0xC1A);
		CHECK(page.Search(ctx, key3, key3_size, &val) == 0);
		CHECK(val == 0xFB1);
		CHECK(page.Delete(ctx, key2, key2_size) == 0);
		CHECK(page.Insert(ctx, key4, key4_size, 0xBEEF) < 0);
		CHECK(page.Insert(ctx, key2, key2_size, 0xB00) == 0);
		CHECK(page.Search(ctx, key2, key2_size, &val) == 0);
		CHECK(val == 0xB00);
		CHECK(page.Delete(ctx, key2, key2_size) == 0);
		CHECK(page.Insert(ctx, key4, key4_size, 0xBEEF) < 0);
		CHECK(page.Delete(ctx, key1, key2_size) == 0);
		CHECK(page.Insert(ctx, key4, key4_size, 0xBEEF) == 0);
	}



	TEST_FIXTURE(ContextFixture, TestDelete1)
	{
		char*         key1 = (char*) "key1";
		char*         key2 = (char*) "key2";
		char*         key3 = (char*) "key3";
		char*         key4 = (char*) "key4";
		uint64_t      val;
		Page<Context> page;

		CHECK(page.Insert(ctx, key1, strlen(key1)+1, 0xCAFE) == 0);
		CHECK(page.Insert(ctx, key2, strlen(key2)+1, 0xC1A) == 0);
		CHECK(page.Insert(ctx, key3, strlen(key3)+1, 0xFB1) == 0);

		CHECK(page.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page.Search(ctx, key2, strlen(key2)+1, &val) == 0);
		CHECK(val == 0xC1A);
		CHECK(page.Search(ctx, key3, strlen(key3)+1, &val) == 0);
		CHECK(val == 0xFB1);

		CHECK(page.Delete(ctx, key2, strlen(key2)+1) == 0);
		CHECK(page.Search(ctx, key2, strlen(key2)+1, &val) < 0);

		CHECK(page.Insert(ctx, key4, strlen(key4)+1, 0x18ACF9) == 0);

		CHECK(page.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page.Search(ctx, key2, strlen(key2)+1, &val) < 0);
		CHECK(page.Search(ctx, key3, strlen(key3)+1, &val) == 0);
		CHECK(val == 0xFB1);
		CHECK(page.Search(ctx, key4, strlen(key4)+1, &val) == 0);
		CHECK(val == 0x18ACF9);
	}


	TEST_FIXTURE(ContextFixture, TestSplitHalf1)
	{
		char*         key1 = (char*) "key1_xxxx_";
		char*         key2 = (char*) "key2_xxxx_";
		char*         key3 = (char*) "key3_xxxx_";
		char*         key4 = (char*) "key4_xxxx_";
		uint64_t      val;
		Page<Context> page;
		Page<Context> newpage;

		CHECK(page.Insert(ctx, key1, strlen(key1)+1, 0xCAFE) == 0);
		CHECK(page.Insert(ctx, key2, strlen(key2)+1, 0xC1A) == 0);
		CHECK(page.Insert(ctx, key3, strlen(key3)+1, 0xFB1) == 0);
		CHECK(page.Insert(ctx, key4, strlen(key4)+1, 0xFAFA) == 0);

		CHECK(page.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page.Search(ctx, key2, strlen(key2)+1, &val) == 0);
		CHECK(val == 0xC1A);
		CHECK(page.Search(ctx, key3, strlen(key3)+1, &val) == 0);
		CHECK(val == 0xFB1);
		CHECK(page.Search(ctx, key4, strlen(key4)+1, &val) == 0);
		CHECK(val == 0xFAFA);

		CHECK(page.SplitHalf(ctx, &newpage) == 0);

		CHECK(page.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page.Search(ctx, key2, strlen(key2)+1, &val) == 0);
		CHECK(val == 0xC1A);
		CHECK(page.Search(ctx, key3, strlen(key3)+1, &val) != 0);
		CHECK(page.Search(ctx, key4, strlen(key4)+1, &val) != 0);

		CHECK(newpage.Search(ctx, key3, strlen(key3)+1, &val) == 0);
		CHECK(val == 0xFB1);
		CHECK(newpage.Search(ctx, key4, strlen(key4)+1, &val) == 0);
		CHECK(val == 0xFAFA);
	}


	TEST_FIXTURE(ContextFixture, TestSplitHalf2)
	{
		char*         key1 = (char*) "key1_xxxx_";
		char*         key2 = (char*) "key2_xxxx_";
		char*         key3 = (char*) "key3_xxxx_";
		char*         key4 = (char*) "key4_xxxx_";
		char*         key5 = (char*) "key5_xxxx_";
		uint64_t      val;
		Page<Context> page;
		Page<Context> newpage;

		CHECK(page.Insert(ctx, key1, strlen(key1)+1, 0xCAFE) == 0);
		CHECK(page.Insert(ctx, key2, strlen(key2)+1, 0xC1A) == 0);
		CHECK(page.Insert(ctx, key3, strlen(key3)+1, 0xFB1) == 0);
		CHECK(page.Insert(ctx, key4, strlen(key4)+1, 0xFAFA) == 0);
		CHECK(page.Insert(ctx, key5, strlen(key5)+1, 0xBEEF) == 0);

		CHECK(page.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page.Search(ctx, key2, strlen(key2)+1, &val) == 0);
		CHECK(val == 0xC1A);
		CHECK(page.Search(ctx, key3, strlen(key3)+1, &val) == 0);
		CHECK(val == 0xFB1);
		CHECK(page.Search(ctx, key4, strlen(key4)+1, &val) == 0);
		CHECK(val == 0xFAFA);
		CHECK(page.Search(ctx, key5, strlen(key5)+1, &val) == 0);
		CHECK(val == 0xBEEF);

		CHECK(page.Delete(ctx, key2, strlen(key2)+1) == 0);

		CHECK(page.SplitHalf(ctx, &newpage) == 0);

		CHECK(page.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page.Search(ctx, key2, strlen(key2)+1, &val) != 0);
		CHECK(page.Search(ctx, key3, strlen(key3)+1, &val) == 0);
		CHECK(page.Search(ctx, key4, strlen(key4)+1, &val) != 0);
		CHECK(page.Search(ctx, key5, strlen(key5)+1, &val) != 0);

		CHECK(newpage.Search(ctx, key4, strlen(key4)+1, &val) == 0);
		CHECK(val == 0xFAFA);
		CHECK(newpage.Search(ctx, key5, strlen(key5)+1, &val) == 0);
		CHECK(val == 0xBEEF);
	}


	TEST_FIXTURE(ContextFixture, TestMerge1)
	{
		char*         key1 = (char*) "key1_xxxx_";
		char*         key2 = (char*) "key2_xxxx_";
		char*         key3 = (char*) "key3_xxxx_";
		char*         key4 = (char*) "key4_xxxx_";
		char*         key5 = (char*) "key5_xxxx_";
		uint64_t      val;
		Page<Context> page1;
		Page<Context> page2;

		CHECK(page1.Insert(ctx, key1, strlen(key1)+1, 0xCAFE) == 0);
		CHECK(page1.Insert(ctx, key2, strlen(key2)+1, 0xC1A) == 0);
		CHECK(page1.Insert(ctx, key3, strlen(key3)+1, 0xFB1) == 0);
		CHECK(page2.Insert(ctx, key4, strlen(key4)+1, 0xFAFA) == 0);
		CHECK(page2.Insert(ctx, key5, strlen(key5)+1, 0xBEEF) == 0);

		CHECK(page1.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page1.Search(ctx, key2, strlen(key2)+1, &val) == 0);
		CHECK(val == 0xC1A);
		CHECK(page1.Search(ctx, key3, strlen(key3)+1, &val) == 0);
		CHECK(val == 0xFB1);
		CHECK(page2.Search(ctx, key4, strlen(key4)+1, &val) == 0);
		CHECK(val == 0xFAFA);
		CHECK(page2.Search(ctx, key5, strlen(key5)+1, &val) == 0);
		CHECK(val == 0xBEEF);

		CHECK(page1.Merge(ctx, &page2) == 0);

		CHECK(page1.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page1.Search(ctx, key2, strlen(key2)+1, &val) == 0);
		CHECK(val == 0xC1A);
		CHECK(page1.Search(ctx, key3, strlen(key3)+1, &val) == 0);
		CHECK(val == 0xFB1);
		CHECK(page1.Search(ctx, key4, strlen(key4)+1, &val) == 0);
		CHECK(val == 0xFAFA);
		CHECK(page1.Search(ctx, key5, strlen(key5)+1, &val) == 0);
		CHECK(val == 0xBEEF);
	}


	TEST_FIXTURE(ContextFixture, TestMergeOverflow)
	{
		char*         key1 = (char*) "key1_0000_1111_2222";
		char*         key2 = (char*) "key2_0000_1111_2222";
		char*         key3 = (char*) "key3_0000_1111_2222";
		char*         key4 = (char*) "key4_0000_1111_2222";
		char*         key5 = (char*) "key5_0000_1111_2222";
		uint64_t      val;
		Page<Context> page1;
		Page<Context> page2;

		CHECK(page1.Insert(ctx, key1, strlen(key1)+1, 0xCAFE) == 0);
		CHECK(page1.Insert(ctx, key2, strlen(key2)+1, 0xC1A) == 0);
		CHECK(page1.Insert(ctx, key3, strlen(key3)+1, 0xFB1) == 0);
		CHECK(page2.Insert(ctx, key4, strlen(key4)+1, 0xFAFA) == 0);
		CHECK(page2.Insert(ctx, key5, strlen(key5)+1, 0xBEEF) == 0);

		CHECK(page1.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page1.Search(ctx, key2, strlen(key2)+1, &val) == 0);
		CHECK(val == 0xC1A);
		CHECK(page1.Search(ctx, key3, strlen(key3)+1, &val) == 0);
		CHECK(val == 0xFB1);
		CHECK(page2.Search(ctx, key4, strlen(key4)+1, &val) == 0);
		CHECK(val == 0xFAFA);
		CHECK(page2.Search(ctx, key5, strlen(key5)+1, &val) == 0);
		CHECK(val == 0xBEEF);

		CHECK(page1.Merge(ctx, &page2) == -E_NOMEM);

		CHECK(page1.Search(ctx, key1, strlen(key1)+1, &val) == 0);
		CHECK(val == 0xCAFE);
		CHECK(page1.Search(ctx, key2, strlen(key2)+1, &val) == 0);
		CHECK(val == 0xC1A);
		CHECK(page1.Search(ctx, key3, strlen(key3)+1, &val) == 0);
		CHECK(val == 0xFB1);
		CHECK(page1.Search(ctx, key4, strlen(key4)+1, &val) == 0);
		CHECK(val == 0xFAFA);
		CHECK(page1.Search(ctx, key5, strlen(key5)+1, &val) != 0);
		CHECK(page2.Search(ctx, key5, strlen(key5)+1, &val) == 0);
		CHECK(val == 0xBEEF);
	}


}

std::string gen_rand_str(const int minlen, const int maxlen) {
    static const char alphanum[] =
	   "0123456789"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	   "abcdefghijklmnopqrstuvwxyz";
	int               len;
	char              s[128];

	assert(maxlen < 128);

	len = minlen+rand() % (maxlen - minlen+1);

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

    s[len] = 0;

	return std::string(s);
}

class TestContext {

};

SUITE(HashTable)
{
	TEST_FIXTURE(ContextFixture, TestInsert)
	{
		int                                                        i;
		HashTable<Context>                                         ht;
		std::map<std::string, uint64_t>                            kvmap;
		std::map<std::string, uint64_t>::iterator                  it;
		std::pair<std::map<std::string, uint64_t>::iterator, bool> ret;
		std::string                                                key;
		uint64_t                                                   val;
		int                                                        rv;

		srand(0);

		for (i=0; i<128; i++) {
			key=gen_rand_str(16, 16);
			kvmap.insert(std::pair<std::string, uint64_t>(key, i));
		}	

		for (it=kvmap.begin(); it != kvmap.end(); it++) {
			key = (*it).first;
			val = (*it).second;
			CHECK(ht.Insert(ctx, key.c_str(), strlen(key.c_str())+1, val) == 0);
		}

		for (it=kvmap.begin(); it != kvmap.end(); it++) {
			key = (*it).first;
			CHECK(ht.Search(ctx, key.c_str(), strlen(key.c_str())+1, &val) == 0);
			CHECK(val == (*it).second);
		}

	}
}
